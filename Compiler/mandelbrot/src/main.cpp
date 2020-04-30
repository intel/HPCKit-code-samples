//==============================================================
// Copyright © 2019 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include <chrono>
#include <iomanip>
#include <iostream>

#include "dpc_common.hpp"
#include "mandel.hpp"

using namespace std;
using namespace sycl;

void ShowDevice(queue &q) {
  // Output platform and device information.
  auto device = q.get_device();
  auto p_name = device.get_platform().get_info<info::platform::name>();
  cout << std::setw(20) << "Platform Name: " << p_name << "\n";
  auto p_version = device.get_platform().get_info<info::platform::version>();
  cout << std::setw(20) << "Platform Version: " << p_version << "\n"; 
  auto d_name = device.get_info<info::device::name>();
  cout << std::setw(20) << "Device Name: " << d_name << "\n";
  auto max_work_group = device.get_info<info::device::max_work_group_size>();
  cout << std::setw(20) << "Max Work Group: " << max_work_group << "\n";
  auto max_compute_units = device.get_info<info::device::max_compute_units>();
  cout << std::setw(20) << "Max Compute Units: " << max_compute_units << "\n";
}

void Execute(queue &q) {
  // Demonstrate the Mandelbrot calculation serial and parallel
  MandelParallel m_par(row_size, col_size, max_iterations);
  MandelSerial m_ser(row_size, col_size, max_iterations);

  // Run the code once to trigger JIT
  m_par.Evaluate(q);

  // Run the parallel version
  dpc_common::MyTimer t_par;
  // time the parallel computation
  for (int i = 0; i < repetitions; ++i) 
    m_par.Evaluate(q);
  dpc_common::Duration parallel_time = t_par.elapsed();

  // Print the results
  m_par.Print();
  m_par.writeImage();
  // Run the serial version
  dpc_common::MyTimer t_ser;
  m_ser.Evaluate();
  dpc_common::Duration serial_time = t_ser.elapsed();

  // Report the results
  cout << std::setw(20) << "serial time: " << serial_time.count() << "s\n";
  cout << std::setw(20) << "parallel time: " << (parallel_time / repetitions).count() << "s\n";

  // Validating
  m_par.Verify(m_ser);
}

void Usage(string program_name) {
  // Utility function to display argument usage
  cout << " Incorrect parameters\n";
  cout << " Usage: ";
  cout << program_name << "\n\n";
  exit(-1);
}

int main(int argc, char *argv[]) {
  if (argc != 1) {
    Usage(argv[0]);
  }

  try {

    // Create a queue on the default device
    // Set the SYCL_DEVICE_TYPE environment variable
    // to (CPU|GPU|FPGA|HOST) to change the device
    queue q (default_selector{},dpc_common::exception_handler);
    // display the device info
    ShowDevice(q);
    // launch the body of the application
    Execute(q);
  } catch (...) {
    // some other exception detected
    cout << "Failure\n";
    terminate();
  }
  cout << "Success\n";
  return 0;
}
