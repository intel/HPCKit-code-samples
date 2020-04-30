//==============================================================
// Copyright © 2019 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#pragma once

#include <complex>
#include <exception>
#include <iomanip>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

using namespace cl::sycl;

constexpr int row_size = 512;
constexpr int col_size = 512;
constexpr int max_iterations = 100;
constexpr int repetitions = 100;

struct MandelParameters {
  int row_count_;
  int col_count_;
  int max_iterations_;

  typedef std::complex<float> ComplexF;

  MandelParameters(int row_count, int col_count, int max_iterations)
      : row_count_(row_count),
        col_count_(col_count),
        max_iterations_(max_iterations) { }

  int row_count() const { return row_count_; }
  int col_count() const { return col_count_; }
  int max_iterations() const { return max_iterations_; } 

  // scale from 0..row_count to -1.5..0.5
  float ScaleRow(int i) const { return -1.5f + (i * (2.0f / row_count_)); }

  // scale from 0..col_count to -1..1
  float ScaleCol(int i) const { return -1.0f + (i * (2.0f / col_count_)); }

  // mandelbrot set are points that do not diverge within max_iterations
  int Point(const ComplexF& c) const {
    int count = 0;
    ComplexF z = 0;
    for (int i = 0; i < max_iterations_; ++i) {
      auto r = z.real();
      auto im = z.imag();
      // leave loop if diverging
      if (((r * r) + (im * im)) >= 4.0) {
        break;
      }
      z = z * z + c;
      count++;
    }
    return count;
  }
};

class Mandel {
 private:
  MandelParameters p_;
  int *data_;  // [p_.row_count_][p_.col_count_];

 public:

  Mandel(int row_count, int col_count, int max_iterations)
      : p_(row_count, col_count, max_iterations) {
    data_ = new int[ p_.row_count() * p_.col_count() ];
  }

  ~Mandel() { delete[] data_; }

  MandelParameters GetParameters() const { return p_; }
  

   void writeImage()
  {
    constexpr int channel_num { 3 };
    int row_count_ = p_.row_count();
    int col_count_ = p_.col_count();

    uint8_t *pixels_ = new uint8_t[col_count_ * row_count_ * channel_num];

    int index = 0;

    for (int j = 0; j < row_count_; ++j)
    {
      for (int i = 0; i < col_count_; ++i)
      {
       float r = data_[i * col_count_ + j]/255.f;
       float g = 0.0f;
       float b = 0.0f;

        int ir = int(255.99 * r);
        int ig = int(255.99 * g);
        int ib = int(255.99 * b);

        pixels_[index++] = ir;
        pixels_[index++] = ig;
        pixels_[index++] = ib;
      }
    }

    stbi_write_png("mandelbrot.png", row_count_, col_count_, channel_num, pixels_, col_count_ * channel_num);
  }



  // use only for debugging with small dimensions
  void Print() {
    if (p_.row_count() > 128 || p_.col_count() > 128) {
      std::cout << "No output to console due to large size. Output saved to mandelbrot.png. " << std::endl;
      return;
    }
    for (int i = 0; i < p_.row_count(); ++i) {
      for (int j = 0; j < p_.col_count_; ++j) {
        std::cout << std::setw(1) << ((GetValue(i,j) >= p_.max_iterations()) ? "x" : " ");
      }
      std::cout << std::endl;
    }
  }

  // accessors for data and count values
  int *data() const { return data_; }

  // accessors to read a value into the mandelbrot data matrix
  void SetValue(int i, int j, float v) { data_[i * p_.col_count_ + j] = v; }

  // accessors to store a value into the mandelbrot data matrix
  int GetValue(int i, int j) const { return data_[i * p_.col_count_ + j]; }

  // validate the results match
  void Verify(Mandel &m) {
    if ((m.p_.row_count() != p_.row_count_) || (m.p_.col_count() != p_.col_count_)) {
      std::cout << "Fail verification - matrix size is different" << std::endl;
      throw std::runtime_error("Verification failure");
    }

    int diff = 0;
    for (int i = 0; i < p_.row_count(); ++i) {
      for (int j = 0; j < p_.col_count(); ++j) {
        if (m.GetValue(i,j) != GetValue(i,j)) 
          diff++;
      }
    }

    double tolerance = 0.05;
    double ratio = (double)diff / (double)(p_.row_count() * p_.col_count());

#if _DEBUG
    std::cout << "diff: " << diff << std::endl;
    std::cout << "total count: " << p_.row_count() * p_.col_count() << std::endl;
#endif

    if (ratio > tolerance) {
      std::cout << "Fail verification - diff larger than tolerance"<< std::endl;
      throw std::runtime_error("Verification failure");
    }
#if _DEBUG
    std::cout << "Pass verification" << std::endl;
#endif
  }
};


class MandelSerial : public Mandel {
public:
  MandelSerial(int row_count, int col_count, int max_iterations)
    : Mandel(row_count, col_count, max_iterations) { }

  void Evaluate() {
    // iterate over image and compute mandel for each point
    MandelParameters p = GetParameters();

    for (int i = 0; i < p.row_count(); ++i) {
      for (int j = 0; j < p.col_count(); ++j) {
        auto c = MandelParameters::ComplexF(p.ScaleRow(i), p.ScaleCol(j));
        SetValue(i, j, p.Point(c));
      }
    }
  }
};

class MandelParallel : public Mandel {
public:
  MandelParallel(int row_count, int col_count, int max_iterations)
    : Mandel(row_count, col_count, max_iterations) { }

  void Evaluate(queue &q) {
    // iterate over image and check if each point is in mandelbrot set
    MandelParameters p = GetParameters();

    const int rows = p.row_count();
    const int cols = p.col_count();

    buffer<int, 2> data_buf(data(), range<2>(rows, cols));

    // we submit a comamand group to the queue
    q.submit([&](handler &h) {
      // get access to the buffer
      auto b = data_buf.get_access<access::mode::write>(h);

      // iterate over image and compute mandel for each point
      h.parallel_for(range<2>(rows, cols), [=](id<2> index) {
        int i = int(index[0]);
        int j = int(index[1]);
        auto c = MandelParameters::ComplexF(p.ScaleRow(i), p.ScaleCol(j));
        b[index] = p.Point(c);
      });
    });

    q.wait_and_throw();
  }
};
