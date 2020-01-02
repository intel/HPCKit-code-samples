//==============================================================
// Copyright © 2019 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include <CL/sycl.hpp>
using namespace cl::sycl;

#include <chrono>
#include <cmath>
#include <ctime>
#include <cstring>

/*
 * Parameters to define coefficients
 * HALF_LENGTH: Radius of the stencil
 * Sample source code is tested for HALF_LENGTH=1 resulting in 
 * 2nd order Stencil finite difference kernel 
 */
#define DT 0.002f
#define DXY 20.0f
#define HALF_LENGTH 1

void printTargetInfo(cl::sycl::queue& , unsigned int , unsigned int );
	
void usage(std::string);

