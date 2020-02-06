//==============================================================
// Copyright © 2020 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#pragma once

#include <string.h>
#include <CL/sycl.hpp>
#include <complex>
#include <iomanip>
#include <iostream>

#include "syclObject.h"

#define GET_KERNEL_TIME

using namespace cl::sycl;

template <int row_count, int col_count, int max_iterations>
class MandelParallel {
 private:
  buffer<int, 2> b;  // buffer lives in SYCL runtime
  int selected_device_index;

  static float scaleRow(int i) {  // scale from 0..row_count to -1.5..0.5
    return -1.5f + (i * (2.0f / float(row_count)));
  }

  static float scaleCol(int i) {  // scale from 0..col_count to -1..1
    return -1 + (i * (2.0f / float(col_count)));
  }

  static std::complex<float> complex_mul(std::complex<float> c0,
                                         std::complex<float> c1) {
    float ac = c0.real() * c1.real();
    float bd = c0.imag() * c1.imag();
    float ad = c0.real() * c1.imag();
    float bc = c0.imag() * c1.real();
    return std::complex<float>(ac - bd, ad + bc);
  }

  static std::complex<float> complex_add(std::complex<float> c0,
                                         std::complex<float> c1) {
    return std::complex<float>(c0.real() + c1.real(), c0.imag() + c1.imag());
  }

  // A point is in the Mandelbrot set if it does not diverge within
  // max_iterations
  static int point(std::complex<float> c, int max_iteration) {
    int count = 0;
    std::complex<float> z = 0;

    for (int i = 0; i < max_iteration; i++) {
      float r = z.real();
      float im = z.imag();

      // leave loop if diverging
      if (((r * r) + (im * im)) >= 4.0) {
        break;
      }

      z = complex_add(complex_mul(z, z), c);
      count++;
    }
    return count;
  }

 public:
  MandelParallel() : b(range<2>(row_count, col_count)){};  // allocate buffer

  MandelParallel(unsigned int device_index)
      : b(range<2>(row_count, col_count)),
        selected_device_index(device_index){};  // allocate buffer

  void Evaluate(SYCLObj syclObj) {
    cl::sycl::event queue_event;
    cl_ulong startk, endk;
    float kernel_time = 0.0;

    // create a queue to schedule kernels on a device
    queue myQueue = syclObj._queue;

    // we submit a comamand group to the queue
    queue_event = myQueue.submit([&](handler& cgh) {
      // get access to the buffer
      auto kb = b.template get_access<access::mode::write>(cgh);

      // iterate over image and compute mandel for each point
      cgh.parallel_for<class manSYCL>(
          range<2>(row_count, col_count), [=](id<2> index) {
            int i = int(index[0]);
            int j = int(index[1]);

            kb[index] = point(std::complex<float>(scaleRow(i), scaleCol(j)),
                              max_iterations);
          });
    });

    // call wait_and_throw to catch async exception
    myQueue.wait_and_throw();

#ifdef GET_KERNEL_TIME
    queue_event.wait();
    startk = queue_event.template get_profiling_info<
        cl::sycl::info::event_profiling::command_start>();
    endk = queue_event.template get_profiling_info<
        cl::sycl::info::event_profiling::command_end>();
    kernel_time = (float)(endk - startk) * 1e-6f;  // to milliseconds

    std::cout << "Kernel time: " << kernel_time << " ms\n";
#endif
  }

  // use only for debugging with small dimensions
  void Print() {
    auto kb = b.template get_access<access::mode::read>();

    for (int i = 0; i < row_count; i++) {
      for (int j = 0; j < col_count; j++) {
        std::cout << std::setw(1) << ((kb[i][j] >= max_iterations) ? "x" : " ");
      }
      std::cout << std::endl;
    }
  }

  int ValidatingResult(int** pBufferToCompare, int row_max, int col_max) {
    if ((row_max != row_count) || (col_max != col_count)) return -1;

    auto kb = b.template get_access<access::mode::read>();
    int diff = 0;

    for (int i = 0; i < row_count; i++) {
      for (int j = 0; j < col_count; j++) {
        if (pBufferToCompare[i][j] != kb[i][j]) diff++;
      }
    }
    double tolerance = 0.05;
    double ratio = (double)diff / (double)(row_count * col_count);

    std::cout << "diff: " << diff << std::endl;
    std::cout << "total count: " << row_count * col_count << std::endl;

    if (ratio < tolerance) {
      std::cout << "Pass verification\n";
      return 0;
    } else {
      std::cout << "Verification fail\n";
      return -1;
    }
  }
};
