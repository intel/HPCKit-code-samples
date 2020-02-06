
//==============================================================
// Copyright © 2020 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include "../include/iso3dfd.h"

/*
 * Device-Code
 * OpenMP Offload implementation for single iteration of iso3dfd kernel.
 * This function uses the default distribution of work
 * It represents minimal changes to the CPU OpenMP code.
 * Inner most loop order is changed from CPU OpenMP version to represent
 * work-items in X-Y plane. And each work-item traverses the Z-plane
 */
void inline iso_3dfd_it(float *ptr_next_base, float *ptr_prev_base,
                        float *ptr_vel_base, float *coeff, const int n1,
                        const int n2, const int n3, const int n1_Tblock,
                        const int n2_Tblock, const int n3_Tblock) {
  int dimn1n2 = n1 * n2;
  int size = n3 * dimn1n2;

  int n3End = n3 - HALF_LENGTH;
  int n2End = n2 - HALF_LENGTH;
  int n1End = n1 - HALF_LENGTH;

  // Outer 3 loops just execute once if _Tblock sizes are same as grid sizes,
  // which is enforced here to demonstrate the default version.

  for (int bz = HALF_LENGTH; bz < n3End; bz += n3_Tblock) {
    for (int by = HALF_LENGTH; by < n2End; by += n2_Tblock) {
      for (int bx = HALF_LENGTH; bx < n1End; bx += n1_Tblock) {
        int izEnd = MIN(bz + n3_Tblock, n3End);
        int iyEnd = MIN(by + n2_Tblock, n2End);
        int ixEnd = MIN(bx + n1_Tblock, n1End);

#pragma omp target teams distribute parallel for simd collapse(3) \
    schedule(static, 1)
        for (int iz = bz; iz < izEnd; iz++) {
          for (int iy = by; iy < iyEnd; iy++) {
            for (int ix = bx; ix < ixEnd; ix++) {
              float *ptr_next = ptr_next_base + iz * dimn1n2 + iy * n1;
              float *ptr_prev = ptr_prev_base + iz * dimn1n2 + iy * n1;
              float *ptr_vel = ptr_vel_base + iz * dimn1n2 + iy * n1;

              float value = ptr_prev[ix] * coeff[0];
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
        }
      }
    }
  }
}

/*
 * Device-Code
 * OpenMP Offload implementation for single iteration of iso3dfd kernel.
 * This function uses the tiling approach for distribution of work
 * It represents minimal changes to the CPU OpenMP code.
 * OpenMP teams are created and distributed to work on a TILE
 * Inner most loop order is changed from CPU OpenMP version to represent
 * work-items in X-Y plane. And each work-item traverses the Z-plane
 */
void inline iso_3dfd_it_tiled(float *ptr_next_base, float *ptr_prev_base,
                              float *ptr_vel_base, float *coeff, const int n1,
                              const int n2, const int n3, const int n1_Tblock,
                              const int n2_Tblock, const int n3_Tblock) {
  int dimn1n2 = n1 * n2;
  int size = n3 * dimn1n2;

  int n3End = n3 - HALF_LENGTH;
  int n2End = n2 - HALF_LENGTH;
  int n1End = n1 - HALF_LENGTH;

#pragma omp target teams distribute collapse(3)                       \
    num_teams((n3 / n3_Tblock) * (n2 / n2_Tblock) * (n1 / n1_Tblock)) \
        thread_limit(n1_Tblock *n2_Tblock)
  {  // start of omp target
    for (int bz = HALF_LENGTH; bz < n3End; bz += n3_Tblock) {
      for (int by = HALF_LENGTH; by < n2End; by += n2_Tblock) {
        for (int bx = HALF_LENGTH; bx < n1End; bx += n1_Tblock) {
          int izEnd = MIN(bz + n3_Tblock, n3End);
          int iyEnd = MIN(by + n2_Tblock, n2End);
          int ixEnd = MIN(bx + n1_Tblock, n1End);

#pragma omp parallel for simd collapse(2) schedule(static, 1)
          for (int iy = by; iy < iyEnd; iy++) {
            for (int ix = bx; ix < ixEnd; ix++) {
              for (int iz = bz; iz < izEnd; iz++) {
                float *ptr_next = ptr_next_base + iz * dimn1n2 + iy * n1;
                float *ptr_prev = ptr_prev_base + iz * dimn1n2 + iy * n1;
                float *ptr_vel = ptr_vel_base + iz * dimn1n2 + iy * n1;

                float value = ptr_prev[ix] * coeff[0];
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
          }
        }
      }
    }
  }  // end of omp target
}

/*
 * Host-Code
 * Driver function for ISO3DFD OpenMP Offload code
 * Uses ptr_next and ptr_prev as ping-pong buffers to achieve
 * accelerated wave propogation
 * OpenMP Target region is declared and maintainted for all the
 * time steps
 */
void iso_3dfd(float *ptr_next, float *ptr_prev, float *ptr_vel, float *coeff,
              const int n1, const int n2, const int n3, const int nreps,
              const int n1_Tblock, const int n2_Tblock, const int n3_Tblock) {
  int dimn1n2 = n1 * n2;
  int size = n3 * dimn1n2;

  float *temp = NULL;

#pragma omp target data map(ptr_next [0:size], ptr_prev [0:size])        \
    map(ptr_vel [0:size], coeff [0:9], n1, n2, n3, n1_Tblock, n2_Tblock, \
        n3_Tblock)
  for (int it = 0; it < nreps; it += 1) {
#ifndef USE_TILED
    iso_3dfd_it(ptr_next, ptr_prev, ptr_vel, coeff, n1, n2, n3, n1, n2, n3);
#else
    iso_3dfd_it_tiled(ptr_next, ptr_prev, ptr_vel, coeff, n1, n2, n3, n1_Tblock,
                      n2_Tblock, n3_Tblock);
#endif
    // here's where boundary conditions and halo exchanges happen
    temp = ptr_next;
    ptr_next = ptr_prev;
    ptr_prev = temp;
  }
}

int main(int argc, char *argv[]) {
  float *prev_base;
  float *next_base;
  float *vel_base;

  bool error = false;

  size_t n1, n2, n3;
  size_t n1_Tblock, n2_Tblock, n3_Tblock;
  unsigned int nIterations;

  try {
    n1 = std::stoi(argv[1]) + (2 * HALF_LENGTH);
    n2 = std::stoi(argv[2]) + (2 * HALF_LENGTH);
    n3 = std::stoi(argv[3]) + (2 * HALF_LENGTH);
    n1_Tblock = std::stoi(argv[4]);
    n2_Tblock = std::stoi(argv[5]);
    n3_Tblock = std::stoi(argv[6]);
    nIterations = std::stoi(argv[7]);
  }

  catch (...) {
    usage(argv[0]);
    return 1;
  }

  if (validateInput(std::stoi(argv[1]), std::stoi(argv[2]), std::stoi(argv[3]),
                    n1_Tblock, n2_Tblock, n3_Tblock, nIterations)) {
    usage(argv[0]);
    return 1;
  }

  size_t nsize = n1 * n2 * n3;

  prev_base = new float[nsize];
  next_base = new float[nsize];
  vel_base = new float[nsize];

  // Initialize Coefficients
  float coeff[HALF_LENGTH + 1] = {
    -3.0548446,
    +1.7777778,
    -3.1111111e-1,
    +7.572087e-2,
#if (HALF_LENGTH == 4)
    -1.76767677e-2
#elif (HALF_LENGTH == 8)
    -1.76767677e-2,
    +3.480962e-3,
    -5.180005e-4,
    +5.074287e-5,
    -2.42812e-6
#endif
  };

  // Apply the DX DY and DZ to coefficients
  coeff[0] = (3.0f * coeff[0]) / (DXYZ * DXYZ);
  for (int i = 1; i <= HALF_LENGTH; i++) {
    coeff[i] = coeff[i] / (DXYZ * DXYZ);
  }

  initialize(prev_base, next_base, vel_base, n1, n2, n3);

  std::cout << "Grid Sizes: " << n1 - 2 * HALF_LENGTH << " "
            << n2 - 2 * HALF_LENGTH << " " << n3 - 2 * HALF_LENGTH << std::endl;
#ifdef USE_TILED
  std::cout << "Tile Sizes: " << n1_Tblock << " " << n2_Tblock << " "
            << n3_Tblock << std::endl;
#endif
  std::cout << "Memory Usage (MBytes): "
            << ((3 * nsize * sizeof(float)) / (1024 * 1024)) << std::endl;

  auto start = std::chrono::steady_clock::now();

  iso_3dfd(next_base, prev_base, vel_base, coeff, n1, n2, n3, nIterations,
           n1_Tblock, n2_Tblock, n3_Tblock);

  auto end = std::chrono::steady_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();
  std::cout << "Time: " << time << std::endl;

  printStats(time, n1, n2, n3, nIterations);

#ifdef VERIFY_RESULTS
  error = verifyResults(next_base, prev_base, vel_base, coeff, n1, n2, n3,
                        nIterations, n1_Tblock, n2_Tblock, n3_Tblock);
#endif
  delete[] prev_base;
  delete[] next_base;
  delete[] vel_base;

  return error ? 1 : 0;
}
