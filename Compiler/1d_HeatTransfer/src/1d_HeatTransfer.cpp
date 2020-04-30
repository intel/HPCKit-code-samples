//==============================================================
// Copyright © 2019 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================
//
// 1D HEAT TRANSFER: Using Intel® oneAPI DPC++ Language to simulate 1D Heat
// Transfer.
//
// The code sample simulates the heat propagation according to the following
// equation (case where there is no heat generation):
//
//    dU/dt = k * d2U/dx2
//    (u(x,t+DT) - u(x,t)) / DT = k * (u(x+DX,t)- 2u(x,t) + u(x-DX,t)) / DX2
//    U(i) = C * (U(i+1) - 2 * U(i) + U(i-1)) + U(i)
//
// where constant C = k * dt / (dx * dx)
//
// For comprehensive instructions regarding DPC++ Programming, go to
// https://software.intel.com/en-us/oneapi-programming-guide
// and search based on relevant terms noted in the comments.
//
// DPC++ material used in this code sample:
//
// Basic structures of DPC++:
//   DPC++ Queues (including device selectors and exception handlers)
//   DPC++ Buffers and accessors (communicate data between the host and the
//   device)
//   DPC++ Kernels (including parallel_for function and range<1> objects)
//
//******************************************************************************
// Content: (version 1.0)
//   1d_HeatTransfer.cpp
//
//******************************************************************************
#include <CL/sycl.hpp>

using namespace cl::sycl;

constexpr float dt = 0.002f;
constexpr float dx = 0.01f;
constexpr float k = 0.025f;
constexpr float temp = 100.0f;

//************************************
// Function description: display input parameters used for this sample.
//************************************
void Usage(std::string programName) {
  std::cout << " Incorrect parameters "
            << "\n";
  std::cout << " Usage: ";
  std::cout << programName << " <n> <i> "
            << "\n \n";
  std::cout << " n : Number of points to simulate "
            << "\n";
  std::cout << " i : Number of timesteps "
            << "\n";
}

//************************************
// Function description: initialize the array.
//************************************
void Initialize(float* array, unsigned int num, unsigned idx) {
  for (unsigned int i = idx; i < num; i++) array[i] = 0.0f;
}

//************************************
// Function description: compute the heat in the device (in parallel).
//************************************
float* ComputeHeatDeviceParallel(float* arr, float* arr_next, float C,
                                 unsigned int num_p, unsigned int num_iter,
                                 float t) {
  unsigned int i;
  float* swap;

  try {
    // Define the device queue
    queue q = default_selector{};
    std::cout << "kernel runs on "
              << q.get_device().get_info<info::device::name>() << "\n";

    // Set boundary condition at t0
    arr[0] = arr_next[0] = t;

    float* current_data_ptr;
    float* next_data_ptr;
    current_data_ptr = arr;
    next_data_ptr = arr_next;

    // Iterate over timesteps
    for (i = 1; i <= num_iter; i++) {
      // Buffer scope
      {
        if (i % 2 != 0) {
          current_data_ptr = arr;
          next_data_ptr = arr_next;
        } else {
          current_data_ptr = arr_next;
          next_data_ptr = arr;
        }

        buffer<float, 1> arr_buf(current_data_ptr, range<1>{num_p + 2});
        buffer<float, 1> arr_next_buf(next_data_ptr, range<1>{num_p + 2});

        q.submit([&](handler& h) {
          // The size of memory amount that will be given to the buffer.
          range<1> num_items{num_p + 2};

          auto arr_acc = arr_buf.get_access<access::mode::read_write>(h);
          auto arr_next_acc =
              arr_next_buf.get_access<access::mode::read_write>(h);

          h.parallel_for(num_items, [=](id<1> k) {
            size_t gid = k.get(0);

            if (gid == 0) {
              // Set boundary condition at one end (fixed number)
              arr_next_acc[k] = arr_acc[k] = 100.0f;
            } else if (gid == num_p + 1) {
              arr_next_acc[k] = arr_acc[k - 1];
            } else {
              arr_next_acc[k] =
                  C * (arr_acc[k + 1] - 2 * arr_acc[k] + arr_acc[k - 1]) +
                  arr_acc[k];
            }
          });  // end parallel for loop in kernel
        });    // end device queue
        q.wait();
      }  // End buffer scope
    }    // end iteration

    // Swap two arrays so that the latest result being in heat_next
    if (i % 2 == 0) {
      swap = arr;
      arr = arr_next;
      arr_next = swap;
    }

  } catch (cl::sycl::exception e) {
    std::cout << "SYCL exception caught: " << e.what() << "\n";
  }

  return arr;
}

//************************************
// Function description: compute the heat in the host (in serial).
//************************************
float* ComputeHeatHostSerial(float* arr, float* arr_next, float C,
                             unsigned int num_p, unsigned int num_iter,
                             float t) {
  unsigned int i, k;
  float* swap;

  // Set initial condition
  Initialize(arr, num_p + 2, 0);
  Initialize(arr_next, num_p + 2, 0);

  // Set boundary condition at t0
  arr[0] = arr_next[0] = t;

  // Iterate over timesteps
  for (i = 1; i <= num_iter; i++) {
    // Set boundary conditions at one end (fixed number)
    arr[0] = 100.0f;
    arr_next[0] = arr[0];

    for (k = 1; k <= num_p; k++)
      arr_next[k] = C * (arr[k + 1] - 2 * arr[k] + arr[k - 1]) + arr[k];

    arr_next[num_p + 1] = arr[num_p];

    // Update heat
    swap = arr;
    arr = arr_next;
    arr_next = swap;
  }

  return arr;
}

//************************************
// Function description: calculate the results computed by the host and by the
// device.
//************************************
bool CompareResults(float* device_results, float* host_results,
                    unsigned int num_point, float C) {
  float delta = 0.001f;
  float difference = 0.00f;
  float abs_delta = fabsf(delta);
  double norm2 = 0;
  bool err = false;

  FILE* fp = fopen("./error_diff.txt", "w");
  if (!fp) fp = stderr;

  fprintf(fp, " C = %e )\n", C);
  fprintf(fp, " \t idx\theat[i]\t\theat_CPU[i] \n");

  for (unsigned int i = 0; i < num_point + 2; i++) {
    fprintf(fp, " RESULT: (%d)\t%10.6f  %10.6f\n", i, device_results[i],
            host_results[i]);

    difference = fabsf(host_results[i] - device_results[i]);
    norm2 += difference * difference;

    if (difference > delta) {
      err = true;
      fprintf(fp, " ERROR: (%d)\t%e instead of %e (|e|=%e)\n", i,
              device_results[i], host_results[i], difference);
    }
  }

  return err;
}

int main(int argc, char* argv[]) {
  // Array pointers used to simulate the heat
  float* heat;
  float* heat_next;
  float* heat_CPU;
  float* heat_CPU_next;

  unsigned int n_point;  // The number of point in 1D space
  unsigned int
      n_iteration;  // The number of iteration to simulate the heat propagation

  // Read input parameters
  try {
    n_point = std::stoi(argv[1]);
    n_iteration = std::stoi(argv[2]);
  } catch (...) {
    Usage(argv[0]);
    return (-1);
  }

  // heat and heat_next store temperatures of the current and next iteration of
  // n_point (calculated in kernel)
  heat = new float[n_point + 2];
  heat_next = new float[n_point + 2];

  // heat_CPU and heat_next_CPU store temperatures of the current and next
  // iteration of n_point (calculated in CPU or comparison)
  heat_CPU = new float[n_point + 2];
  heat_CPU_next = new float[n_point + 2];

  // Constant used in the simulation
  float C = (k * dt) / (dx * dx);

  // Heat initial condition at t = 0
  Initialize(heat, n_point + 2, 0);
  Initialize(heat_next, n_point + 2, 0);

  // Start timer
  auto start = std::chrono::steady_clock::now();

  float* final_device = NULL;

  final_device =
      ComputeHeatDeviceParallel(heat, heat_next, C, n_point, n_iteration, temp);

  // Display time used by device
  auto end = std::chrono::steady_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();
  std::cout << "Computing time used in device: " << time << " ms"
            << "\n";
  std::cout << "\n";

  // Compute heat in CPU in (for comparision)
  float* final_CPU = NULL;

  final_CPU = ComputeHeatHostSerial(heat_CPU, heat_CPU_next, C, n_point,
                                    n_iteration, 100.0f);

  // Compare the results computed in device (in parallel) and in host (in
  // serial)
  bool err = CompareResults(final_device, final_CPU, n_point, C);

  if (err == true)
    std::cout << "Please check the error_diff.txt file ... "
              << "\n";
  else
    std::cout << "PASSED! There is no difference between the results computed "
                 "in host and in kernel."
              << "\n";

  // Cleanup
  delete[] heat;
  delete[] heat_next;
  delete[] heat_CPU;
  delete[] heat_CPU_next;

  return 0;
}
