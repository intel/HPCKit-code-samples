//==============================================================
// Copyright © 2020 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include "syclObject.h"

SYCLObj::SYCLObj()
    : _queue(cl::sycl::default_selector()), _device(_queue.get_device()) {}

SYCLObj::SYCLObj(int selectedDeviceIndex) {
  setUpDevice(selectedDeviceIndex);
  setUpQueue();
}

// Set device by index
void SYCLObj::setUpDevice(int selectedDeviceIndex) {
  std::vector<cl::sycl::device> devices = cl::sycl::device::get_devices();
  for (int i = 0; i < devices.size(); i++) {
    if (i == selectedDeviceIndex) {
      _device = devices[i];
    }
  }
}

cl::sycl::queue SYCLObj::setUpQueue() {
  // including an async exception handler
  auto asyncHandler = [](cl::sycl::exception_list eL) {
    for (auto& e : eL) {
      try {
        std::rethrow_exception(e);
      } catch (cl::sycl::exception const& e) {
        std::cout << "fail" << std::endl;
        // std::terminate() will exit the process, return non-zero, and output a
        // message to the user about the exception
        std::terminate();
      }
    }
  };

  cl::sycl::property_list propList =
      cl::sycl::property_list{cl::sycl::property::queue::enable_profiling()};
  _queue = cl::sycl::queue(_device, asyncHandler, propList);
  return _queue;
}

void SYCLObj::showDevice() {
  // Output device and platform information.
  auto deviceName = _device.get_info<cl::sycl::info::device::name>();
  std::cout << " Device Name: " << deviceName << "\n";
  auto platformName =
      _device.get_platform().get_info<cl::sycl::info::platform::name>();
  std::cout << " Platform Name " << platformName << "\n";
}
