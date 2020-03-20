//==============================================================
// Copyright © 2020 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include <chrono>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>

#define DT 0.002f
#define DXYZ 50.0f
#define HALF_LENGTH 8

#define MIN(a, b) (a) < (b) ? (a) : (b)
#define STENCIL_LOOKUP(ptr_prev, ix, ir, n1, dimn1n2)               \
  (coeff[ir] * ((ptr_prev[ix + ir] + ptr_prev[ix - ir]) +           \
                (ptr_prev[ix + ir * n1] + ptr_prev[ix - ir * n1]) + \
                (ptr_prev[ix + ir * dimn1n2] + ptr_prev[ix - ir * dimn1n2])))

void usage(std::string);

void printStats(double, size_t, size_t, size_t, unsigned int);

bool within_epsilon(float *, float *, const size_t, const size_t, const size_t,
                    const unsigned int, const int, const float);

void initialize(float *, float *, float *, size_t, size_t, size_t);

bool verifyResults(float *, float *, float *, float *, const int, const int,
                   const int, const int, const int, const int, const int);
bool validateInput(size_t, size_t, size_t, size_t, size_t, size_t, size_t);
