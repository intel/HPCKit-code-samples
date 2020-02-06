//==============================================================
// Copyright © 2020 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include "mandelSerial.h"

MandelSerial::MandelSerial(int row_count, int col_count, int max_iterations)
    : row_count(row_count),
      col_count(col_count),
      max_iterations(max_iterations) {
  p = new int*[row_count];
  for (int i = 0; i < row_count; i++) {
    p[i] = new int[col_count];
  }
}

MandelSerial::~MandelSerial() {
  for (int i = 0; i < row_count; i++) {
    delete[] p[i];
  }
  delete[] p;
}

// iterate over image and compute mandel for each point
void MandelSerial::Evaluate() {
  for (int i = 0; i < row_count; i++) {
    for (int j = 0; j < col_count; j++) {
      p[i][j] =
          point(std::complex<float>(scaleRow(i), scaleCol(j)), max_iterations);
    }
  }
}

// use only for debugging with small dimensions
void MandelSerial::Print() {
  for (int i = 0; i < row_count; i++) {
    for (int j = 0; j < col_count; j++) {
      std::cout << std::setw(1) << ((p[i][j] >= max_iterations) ? "x" : " ");
    }
    std::cout << std::endl;
  }
}

int** MandelSerial::GetBuffer() { return p; }

int MandelSerial::GetRowCount() { return row_count; }
int MandelSerial::GetColCount() { return col_count; }

float MandelSerial::scaleRow(int i) {  // scale from 0..row_count to -1.5..0.5
  return -1.5f + (i * (2.0f / float(row_count)));
}

float MandelSerial::scaleCol(int i) {  // scale from 0..col_count to -1..1
  return -1 + (i * (2.0f / float(col_count)));
}

// A point is in the Mandelbrot set if it does not diverge within max_iterations
int MandelSerial::point(std::complex<float> c, int max_iteration) {
  int count = 0;
  std::complex<float> z = 0;
  for (int i = 0; i < max_iteration; i++) {
    float r = z.real();
    float im = z.imag();
    // leave loop if diverging
    if (((r * r) + (im * im)) >= 4.0) {
      break;
    }
    z = z * z + c;
    count++;
  }
  return count;
}
