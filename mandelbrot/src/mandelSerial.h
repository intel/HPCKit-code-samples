//==============================================================
// Copyright © 2019 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#pragma once

#include <iostream>
#include <iomanip>
#include <complex>

class MandelSerial {
public:
    MandelSerial(int row_count, int col_count, int max_iterations);

    ~MandelSerial();

    // iterate over image and compute mandel for each point
    void Evaluate();

    //use only for debugging with small dimensions
    void Print();
	int** GetBuffer(); 
	int GetRowCount();
	int GetColCount();

private:
	int row_count;
	int col_count;
	int max_iterations;
  	int** p; // [row_count][col_count];

	// scale from 0..row_count to -1.5..0.5
	float scaleRow(int i);

	// scale from 0..col_count to -1..1
	float scaleCol(int i); 

  	// A point is in the Mandelbrot set if it does not diverge within max_iterations
  	int point(std::complex<float> c, int max_iteration); 
};
