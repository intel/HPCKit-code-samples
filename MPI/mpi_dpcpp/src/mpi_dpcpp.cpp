//==============================================================
// Copyright © 2019 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================
//
// MPI_DPCPP: Using Intel® oneAPI DPC++ Language in MPI program.
//
// Using Data Parallel C++, the code sample runs multiple MPI ranks to
// distribute the calculation of the number Pi. Each rank offloads the
// computation to an accelerator (GPU/CPU) using Intel DPC++ compiler to compute
// a partial compution of the number of Pi.
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
//      Calculate the number Pi in parallel using its integral representation.
//
//******************************************************************************
#include <mpi.h>
#include <CL/sycl.hpp>
#include <CL/sycl/intel/fpga_extensions.hpp>

using namespace std;
using namespace cl::sycl;

constexpr int kMaster = 0;
constexpr long kIteration = 1024;
constexpr long kScale = 45;
constexpr long kTotalNumStep = kIteration * kScale;
constexpr access::mode sycl_read = access::mode::read;
constexpr access::mode sycl_write = access::mode::write;

//******************************************************************************
// Function description: computes the number Pi partially in parallel using DPC++.
// Each MPI rank calls this function to computes the number Pi partially.
//******************************************************************************
void CalculatePiParallel(float* results, int rank_num, int num_procs);

int main(int argc, char* argv[]) {
  int i, id, num_procs;
  float total_pi;
  MPI_Status stat;

  // Start MPI.
  if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
    cout << "Failed to initialize MPI\n";
    exit(-1);
  }

  // Create the communicator, and retrieve the number of processes.
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  // Determine the rank of the process.
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  int num_step_per_rank = kTotalNumStep / num_procs;
  float* results_per_rank = new float[num_step_per_rank];

  for (size_t i = 0; i < num_step_per_rank; i++) results_per_rank[i] = 0.0;

  // Calculate the Pi number partially in parallel.
  CalculatePiParallel(results_per_rank, id, num_procs);

  float sum = 0.0;

  for (size_t i = 0; i < num_step_per_rank; i++) sum += results_per_rank[i];

  delete[] results_per_rank;

  MPI_Reduce(&sum, &total_pi, 1, MPI_FLOAT, MPI_SUM, kMaster, MPI_COMM_WORLD);

  if (id == kMaster) cout << "---> pi= " << total_pi << "\n";

  MPI_Finalize();

  return 0;
}

////////////////////////////////////////////////////////////////////////
//
// Compute the number Pi partially in DPC++ on device: the partial result is
// returned in "results".
//
////////////////////////////////////////////////////////////////////////
void CalculatePiParallel(float* results, int rank_num, int num_procs) {
  char machine_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  int num_step = kTotalNumStep / num_procs;
  float* x_pos_per_rank = new float[num_step];
  float dx, dx_2;

  // Get the machine name.
  MPI_Get_processor_name(machine_name, &name_len);

  dx = 1.0f / (float)kTotalNumStep;
  dx_2 = dx / 2.0f;

  for (size_t i = 0; i < num_step; i++)
    x_pos_per_rank[i] = ((float)rank_num / (float)num_procs) + i * dx + dx_2;

  default_selector device_selector;

  // exception handler
  //
  // The exception_list parameter is an iterable list of std::exception_ptr
  // objects. But those pointers are not always directly readable. So, we
  // rethrow the pointer, catch it,  and then we have the exception itself.
  // Note: depending upon the operation there may be several exceptions.
  //
  auto exception_handler = [&](exception_list exceptionList) {
    for (std::exception_ptr const& e : exceptionList) {
      try {
        std::rethrow_exception(e);
      } catch (cl::sycl::exception const& e) {
        std::cout << "Failure" << std::endl;
        std::terminate();
      }
    }
  };

  try {
    // Create a device queue using DPC++ class queue
    queue q(device_selector, exception_handler);

    cout << "Rank " << rank_num << " of " << num_procs
         << " runs on: " << machine_name
         << ", uses device: " << q.get_device().get_info<info::device::name>()
         << "\n";

    // The size of amount of memory that will be given to the buffer.
    range<1> num_items{kTotalNumStep / size_t(num_procs)};

    // Buffers are used to tell SYCL which data will be shared between the host
    // and the devices.
    buffer<float, 1> x_pos_per_rank_buf(
        x_pos_per_rank, range<1>(kTotalNumStep / size_t(num_procs)));
    buffer<float, 1> results_buf(results,
                                 range<1>(kTotalNumStep / size_t(num_procs)));
    buffer<int, 1> ranknum_buf(&rank_num, 1);
    buffer<int, 1> numprocs_buf(&num_procs, 1);

    // Submit takes in a lambda that is passed in a command group handler
    // constructed at runtime.
    q.submit([&](handler& h) {
      // Accessors are used to get access to the memory owned by the buffers.
      auto x_pos_per_rank_accessor =
          x_pos_per_rank_buf.get_access<access::mode::read>(h);
      auto results_accessor = results_buf.get_access<access::mode::write>(h);
      auto ranknum_accessor =
          ranknum_buf.template get_access<access::mode::read>(h);
      auto numprocs_accessor =
          numprocs_buf.template get_access<access::mode::read>(h);

      // Use parallel_for to calculate a partial of the number Pi in parallel.
      // This creates a number of instances of kernel function.
      h.parallel_for(num_items, [=](id<1> k) {
        float x, dx;

        dx = 1.0f / (float)kTotalNumStep;
        x = x_pos_per_rank_accessor[k];
        results_accessor[k] = (4.0f * dx) / (1.0f + x * x);
      });
    });
  } catch (...) {
    std::cout << "Failure" << std::endl;
  }

  // Cleanup.
  delete[] x_pos_per_rank;
}
