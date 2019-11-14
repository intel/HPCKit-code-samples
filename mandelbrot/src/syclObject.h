//==============================================================
// Copyright © 2019 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#ifndef _SYCLOBJ_HPP_
#define _SYCLOBJ_HPP_

#include <CL/sycl.hpp>

#include <iostream>
#include <string>
#include <vector>

constexpr cl::sycl::access::mode sycl_read = cl::sycl::access::mode::read;
constexpr cl::sycl::access::mode sycl_write = cl::sycl::access::mode::write;
constexpr cl::sycl::access::mode sycl_read_write = cl::sycl::access::mode::read_write;
constexpr cl::sycl::access::target sycl_global_buffer = cl::sycl::access::target::global_buffer;
constexpr cl::sycl::access::target sycl_local = cl::sycl::access::target::local;

//class Kernel_naive;

//object for sycl context
class SYCLObj {
public:
    cl::sycl::queue _queue;
    cl::sycl::device _device;

    SYCLObj();
  
    SYCLObj(int selectedDeviceIndex);
    
    // Set device by index
    void setUpDevice(int selectedDeviceIndex);

    cl::sycl::queue setUpQueue();

    void showDevice();
};
#endif  // end of include guard
