//==============================================================
// Copyright © 2020 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include "../include/iso3dfd.h"

/*
 * Host-Code
 * Utility function to get input arguments
 */
void usage(std::string programName) {
  std::cout << " Incorrect parameters " << std::endl;
  std::cout << " Usage: ";
  std::cout << programName << " n1 n2 n3 b1 b2 b3 Iterations" << std::endl
            << std::endl;
  std::cout << " n1 n2 n3      : Grid sizes for the stencil " << std::endl;
  std::cout << " b1 b2 b3      : cache block sizes for CPU" << std::endl;
  std::cout << " 	       : TILE sizes for OMP Offload" << std::endl;
  std::cout << " Iterations    : No. of timesteps. " << std::endl;
}

/*
 * Host-Code
 * Function used for initialization
 */
void initialize(float* ptr_prev, float* ptr_next, float* ptr_vel, size_t n1,
                size_t n2, size_t n3) {
  std::cout << "Initializing ... " << std::endl;
  size_t dim2 = n2 * n1;

  for (int i = 0; i < n3; i++) {
    for (int j = 0; j < n2; j++) {
      size_t offset = i * dim2 + j * n1;

      for (int k = 0; k < n1; k++) {
        ptr_prev[offset + k] = 0.0f;
        ptr_next[offset + k] = 0.0f;
        ptr_vel[offset + k] =
            2250000.0f * DT * DT;  // Integration of the v*v and dt*dt here
      }
    }
  }
  // Then we add a source
  float val = 1.f;
  for (int s = 5; s >= 0; s--) {
    for (int i = n3 / 2 - s; i < n3 / 2 + s; i++) {
      for (int j = n2 / 4 - s; j < n2 / 4 + s; j++) {
        size_t offset = i * dim2 + j * n1;
        for (int k = n1 / 4 - s; k < n1 / 4 + s; k++) {
          ptr_prev[offset + k] = val;
        }
      }
    }
    val *= 10;
  }
}

/*
 * Host-Code
 * Utility function to print stats
 */
void printStats(double time, size_t n1, size_t n2, size_t n3,
                unsigned int nIterations) {
  float throughput_mpoints = 0.0f, mflops = 0.0f, normalized_time = 0.0f;
  double mbytes = 0.0f;

  normalized_time = (double)time / nIterations;
  throughput_mpoints = ((n1 - 2 * HALF_LENGTH) * (n2 - 2 * HALF_LENGTH) *
                        (n3 - 2 * HALF_LENGTH)) /
                       (normalized_time * 1e3f);
  mflops = (7.0f * HALF_LENGTH + 5.0f) * throughput_mpoints;
  mbytes = 12.0f * throughput_mpoints;

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "time         : " << time / 1e3f << " secs" << std::endl;
  std::cout << "throughput   : " << throughput_mpoints << " Mpts/s"
            << std::endl;
  std::cout << "flops        : " << mflops / 1e3f << " GFlops" << std::endl;
  std::cout << "bytes        : " << mbytes / 1e3f << " GBytes/s" << std::endl;
  std::cout << std::endl
            << "--------------------------------------" << std::endl;
  std::cout << std::endl
            << "--------------------------------------" << std::endl;
}

/*
 * Host-Code
 * Utility function to calculate L2-norm between resulting buffer and reference
 * buffer
 */
bool within_epsilon(float* output, float* reference, const size_t dimx,
                    const size_t dimy, const size_t dimz,
                    const unsigned int radius, const int zadjust = 0,
                    const float delta = 0.01f) {
  FILE* fp = fopen("./error_diff.txt", "w");
  if (!fp) fp = stderr;

  bool error = false;
  float abs_delta = fabsf(delta);
  double norm2 = 0;

  for (size_t iz = 0; iz < dimz; iz++) {
    for (size_t iy = 0; iy < dimy; iy++) {
      for (size_t ix = 0; ix < dimx; ix++) {
        if (ix >= radius && ix < (dimx - radius) && iy >= radius &&
            iy < (dimy - radius) && iz >= radius &&
            iz < (dimz - radius + zadjust)) {
          float difference = fabsf(*reference - *output);
          norm2 += difference * difference;
          if (difference > delta) {
            error = true;
            fprintf(fp, " ERROR: (%zu,%zu,%zu)\t%e instead of %e (|e|=%e)\n",
                    ix, iy, iz, *output, *reference, difference);
          }
        }
        ++output;
        ++reference;
      }
    }
  }

  if (fp != stderr) fclose(fp);
  norm2 = sqrt(norm2);
  if (error) printf("error (Euclidean norm): %.9e\n", norm2);
  return error;
}

/*
 * Host-code
 * Validate input arguments
 */
bool validateInput(size_t n1, size_t n2, size_t n3, size_t n1_Tblock,
                   size_t n2_Tblock, size_t n3_Tblock, size_t nIterations) {
  bool error = false;

  if ((n1 < 8) || (n2 < 8) || (n3 < 8)) {
    std::cout << " Invalid grid size : n1, n2, n3 should be greater than 8"
              << std::endl;
    error = true;
  }
  if ((n1_Tblock <= 0) || (n2_Tblock <= 0) || (n3_Tblock <= 0)) {
    std::cout << " Invalid block sizes : n1_Tblock, n2_Tblock, n3_Tblock "
                 "should be greater than 0"
              << std::endl;
    error = true;
  }
  if (nIterations <= 0) {
    std::cout << " Invalid nIterations :  Iterations should be greater than 0"
              << std::endl;
    error = true;
  }
  return error;
}
