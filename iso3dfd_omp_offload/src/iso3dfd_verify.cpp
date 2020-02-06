//==============================================================
// Copyright © 2020 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include "../include/iso3dfd.h"

/*
 * Host-Code
 * OpenMP implementation for single iteration of iso3dfd kernel.
 * This function is used as reference implementation for verification and
 * also to compare OpenMP performance on CPU with the OpenMP Offload version
 */
void iso_3dfd_it_verify(float *ptr_next_base, float *ptr_prev_base,
                        float *ptr_vel_base, float *coeff, const int n1,
                        const int n2, const int n3, const int n1_Tblock,
                        const int n2_Tblock, const int n3_Tblock) {
  int dimn1n2 = n1 * n2;

  int n3End = n3 - HALF_LENGTH;
  int n2End = n2 - HALF_LENGTH;
  int n1End = n1 - HALF_LENGTH;

#pragma omp parallel default(shared)
#pragma omp for schedule(static) collapse(3)
  for (int bz = HALF_LENGTH; bz < n3End; bz += n3_Tblock) {
    for (int by = HALF_LENGTH; by < n2End; by += n2_Tblock) {
      for (int bx = HALF_LENGTH; bx < n1End; bx += n1_Tblock) {
        int izEnd = MIN(bz + n3_Tblock, n3End);
        int iyEnd = MIN(by + n2_Tblock, n2End);
        int ixEnd = MIN(n1_Tblock, n1End - bx);
        int ix;
        for (int iz = bz; iz < izEnd; iz++) {
          for (int iy = by; iy < iyEnd; iy++) {
            float *ptr_next = ptr_next_base + iz * dimn1n2 + iy * n1 + bx;
            float *ptr_prev = ptr_prev_base + iz * dimn1n2 + iy * n1 + bx;
            float *ptr_vel = ptr_vel_base + iz * dimn1n2 + iy * n1 + bx;
#pragma omp simd
            for (int ix = 0; ix < ixEnd; ix++) {
              float value = 0.0;
              value += ptr_prev[ix] * coeff[0];
              value += STENCIL_LOOKUP(ptr_prev, ix, 1, n1, dimn1n2);
              value += STENCIL_LOOKUP(ptr_prev, ix, 2, n1, dimn1n2);
              value += STENCIL_LOOKUP(ptr_prev, ix, 3, n1, dimn1n2);
              value += STENCIL_LOOKUP(ptr_prev, ix, 4, n1, dimn1n2);
              value += STENCIL_LOOKUP(ptr_prev, ix, 5, n1, dimn1n2);
              value += STENCIL_LOOKUP(ptr_prev, ix, 6, n1, dimn1n2);
              value += STENCIL_LOOKUP(ptr_prev, ix, 7, n1, dimn1n2);
              value += STENCIL_LOOKUP(ptr_prev, ix, 8, n1, dimn1n2);
              ptr_next[ix] =
                  2.0f * ptr_prev[ix] - ptr_next[ix] + value * ptr_vel[ix];
            }
          }
        }  // end of inner iterations
      }
    }
  }  // end of cache blocking
}

/*
 * Host-Code
 * Driver function for ISO3DFD OpenMP CPU code
 * Uses ptr_next and ptr_prev as ping-pong buffers to achieve
 * accelerated wave propogation
 */
void iso_3dfd_verify(float *ptr_next, float *ptr_prev, float *ptr_vel,
                     float *coeff, const int n1, const int n2, const int n3,
                     const int nreps, const int n1_Tblock, const int n2_Tblock,
                     const int n3_Tblock) {
  for (int it = 0; it < nreps; it += 1) {
    iso_3dfd_it_verify(ptr_next, ptr_prev, ptr_vel, coeff, n1, n2, n3,
                       n1_Tblock, n2_Tblock, n3_Tblock);

    // here's where boundary conditions and halo exchanges happen
    // Swap previous & next between iterations
    it++;
    if (it < nreps)
      iso_3dfd_it_verify(ptr_prev, ptr_next, ptr_vel, coeff, n1, n2, n3,
                         n1_Tblock, n2_Tblock, n3_Tblock);

  }  // time loop
}

bool verifyResults(float *next_base, float *prev_base, float *vel_base,
                   float *coeff, const int n1, const int n2, const int n3,
                   const int nIterations, const int n1_Tblock,
                   const int n2_Tblock, const int n3_Tblock) {
  std::cout << "Checking Results ... " << std::endl;
  size_t nsize = n1 * n2 * n3;
  bool error = false;

  float *temp = new float[nsize];
  if (nIterations % 2)
    memcpy(temp, next_base, nsize * sizeof(float));
  else
    memcpy(temp, prev_base, nsize * sizeof(float));

  initialize(prev_base, next_base, vel_base, n1, n2, n3);

  iso_3dfd_verify(next_base, prev_base, vel_base, coeff, n1, n2, n3,
                  nIterations, n1_Tblock, n2_Tblock, n3_Tblock);

  if (nIterations % 2)
    error = within_epsilon(temp, next_base, n1, n2, n3, HALF_LENGTH, 0, 0.1f);
  else
    error = within_epsilon(temp, prev_base, n1, n2, n3, HALF_LENGTH, 0, 0.1f);
  if (error)
    std::cout << "Error  = " << error << std::endl;
  else
    std::cout << "Results Match !!! " << std::endl;

  delete[] temp;

  return error;
}
