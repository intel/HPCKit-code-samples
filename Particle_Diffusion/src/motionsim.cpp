/***************************************************************************
 *
Copyright 2020 Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 * motionsim.cpp
 *
 * Description:
 *   This code sample will implement a simple example of a Monte Carlo
 *   simulation of the diffusion of water molecules in tissue.
 *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <CL/sycl.hpp>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>

using namespace cl::sycl;
using std::cin;
using std::cout;
using std::endl;

// Helper functions

// This function displays correct usage and parameters
void usage(std::string programName) {
  cout << " Incorrect number of parameters " << std::endl;
  cout << " Usage: ";
  cout << programName << " <Numbeof Iterations> " << std::endl << std::endl;
}

// This function prints a 2D matrix
template <typename T>
void print_matrix(T** matrix, size_t size_X, size_t size_Y) {
  std::cout << std::endl;
  for (size_t i = 0; i < size_X; ++i) {
    for (size_t j = 0; j < size_Y; ++j) {
      cout << std::setw(3) << matrix[i][j] << " ";
    }
    cout << std::endl;
  }
}

// This function prints a vector
template <typename T>
void print_vector(T* vector, size_t n) {
  std::cout << std::endl;
  for (size_t i = 0; i < n; ++i) {
    std::cout << vector[i] << " ";
  }
  std::cout << std::endl;
}

// This function distributes simulation work across workers
void motion_device_1(cl::sycl::queue& q, float* particleX, float* particleY,
                     float* randomX, float* randomY, int** grid, int grid_size,
                     size_t n_particles, unsigned int nIterations, float radius,
                     size_t* map) {
  auto device = q.get_device();
  auto maxBlockSize =
      device.get_info<cl::sycl::info::device::max_work_group_size>();
  auto maxEUCount =
      device.get_info<cl::sycl::info::device::max_compute_units>();

  // Scale of random numbers
  const size_t scale = 100;

  std::cout << " Running on:: "
            << device.get_info<cl::sycl::info::device::name>() << std::endl;
  std::cout << " The Device Max Work Group Size is : " << maxBlockSize
            << std::endl;
  std::cout << " The Device Max EUCount is : " << maxEUCount << std::endl;
  std::cout << " The number of iterations is : " << nIterations << std::endl;
  std::cout << " The number of particles is : " << n_particles << std::endl;

  // Set the seed for rand() function.
  // Use a fixed seed for reproducibility/debugging
  srand(time(0));
  // srand(17);

  // Compute vectors of random values for X and Y directions
  for (size_t i = 0; i < n_particles * nIterations; i++) {
    randomX[i] = rand() % scale;
    randomY[i] = rand() % scale;
  }

  // Begin scope for buffers
  {
    buffer<float, 1> b_randomX(randomX, range<1>{n_particles * nIterations});
    buffer<float, 1> b_randomY(randomY, range<1>{n_particles * nIterations});
    buffer<float, 1> b_particleX(particleX, range<1>{n_particles});
    buffer<float, 1> b_particleY(particleY, range<1>{n_particles});

    size_t MAP_SIZE = n_particles * grid_size * grid_size;
    buffer<size_t, 1> b_map(map, range<1>{MAP_SIZE});

    // Submit command group for execution
    q.submit([&](handler& cgh) {
      auto a_particleX = b_particleX.get_access<access::mode::read_write>(cgh);
      auto a_particleY = b_particleY.get_access<access::mode::read_write>(cgh);
      auto a_randomX = b_randomX.get_access<access::mode::read>(cgh);
      auto a_randomY = b_randomY.get_access<access::mode::read>(cgh);
      auto a_map = b_map.get_access<access::mode::read_write>(cgh);

      // Send a kernel (lambda) for parallel execution
      cgh.parallel_for<class Simulation>(
          range<1>{n_particles}, [=](cl::sycl::item<1> index) {
            int ii = index.get_id(0);
            size_t randnumX = 0;
            size_t randnumY = 0;
            float displacementX = 0.0f;
            float displacementY = 0.0f;

            // Start iterations
            // Each iteration:
            //  1. Updates the position of all water molecules
            //  2. Checks if water molecule is inside a cell or not.
            //  3. Updates counter in cells array
            size_t iter = 0;
            while (iter < nIterations) {
              // Computes random displacement for each molecule
              // This example shows random distances between
              // -0.05 units and 0.05 units in both X and Y directions
              // Moves each water molecule by a random vector

              randnumX = a_randomX[iter * n_particles + ii];
              randnumY = a_randomY[iter * n_particles + ii];

              // Transform the scaled random numbers into small displacements
              displacementX = (float)randnumX / 1000.0f - 0.0495;
              displacementY = (float)randnumY / 1000.0f - 0.0495;

              // Move particles using random displacements
              a_particleX[ii] += displacementX;
              a_particleY[ii] += displacementY;

              // Compute distances from particle position to grid point
              float dX = a_particleX[ii] - cl::sycl::trunc(a_particleX[ii]);
              float dY = a_particleY[ii] - cl::sycl::trunc(a_particleY[ii]);

              // Compute grid point indices
              int iX = cl::sycl::floor(a_particleX[ii]);
              int iY = cl::sycl::floor(a_particleY[ii]);

              // Check if particle is still in computation grid
              if ((a_particleX[ii] < grid_size) &&
                  (a_particleY[ii] < grid_size) && (a_particleX[ii] >= 0) &&
                  (a_particleY[ii] >= 0)) {
                // Check if particle is (or remained) inside cell.
                // Increment cell counter in map array if so
                if ((dX * dX + dY * dY <= radius * radius)) {
                  // The map array is organized as (particle, y, x)
                  a_map[ii * grid_size * grid_size + iY * grid_size + iX]++;
                }
              }

              iter++;

            }  // Next iteration
               // End iterations
          });  // End parallel for
    });        // End queue submit.

  }  // End scope for buffers

  // For every cell in the grid, add all the counters from different
  // particles (workers) which are stored in the 3rd dimension of the 'map'
  // array
  for (size_t i = 0; i < n_particles; ++i) {
    for (size_t y = 0; y < grid_size; y++) {
      for (size_t x = 0; x < grid_size; x++) {
        if (map[i * grid_size * grid_size + y * grid_size + x] > 0) {
          grid[y][x] += map[i * grid_size * grid_size + y * grid_size + x];
        }
      }
    }
  }  // End loop for number of particles

}  // End of function motion_device_1()

int main(int argc, char* argv[]) {
  // Cell and Particle parameters
  const size_t grid_size = 21;    // Size of square grid
  const size_t n_particles = 20;  // Number of particles
  const float radius = 0.5;       // Cell radius = 0.5*(grid spacing)

  // Default number of operations
  size_t nIterations = 50;

  // Read command-line arguments
  try {
    nIterations = std::stoi(argv[1]);
  }

  catch (...) {
    usage(argv[0]);
    return 1;
  }

  // Allocate arrays

  // Stores a grid of cells
  int** grid = new int*[grid_size];
  for (size_t i = 0; i < grid_size; i++) grid[i] = new int[grid_size];

  // Stores all random numbers to be used in the simulation
  float* randomX = new float[n_particles * nIterations];
  float* randomY = new float[n_particles * nIterations];

  // Stores X and Y position of particles in the cell grid
  float* particleX = new float[n_particles];
  float* particleY = new float[n_particles];

  // 'map' array replicates grid to be used by each particle
  size_t MAP_SIZE = n_particles * grid_size * grid_size;
  size_t* map = new size_t[MAP_SIZE];

  // Initialize arrays
  for (size_t i = 0; i < n_particles; i++) {
    // Initial position of particles in cell grid
    particleX[i] = 10.0;
    particleY[i] = 10.0;

    for (size_t y = 0; y < grid_size; y++) {
      for (size_t x = 0; x < grid_size; x++) {
        map[i * grid_size * grid_size + y * grid_size + x] = 0;
      }
    }
  }

  for (size_t y = 0; y < grid_size; y++) {
    for (size_t x = 0; x < grid_size; x++) {
      grid[y][x] = 0;
    }
  }

  // Create a SYCL queue using default or host/gpu selectors
  default_selector device_selector;
  // gpu_selector device_selector;
  // host_selector device_selector;

  // exception handler
  /*
    The exception_list parameter is an iterable list of std::exception_ptr
    objects. But those pointers are not always directly readable. So, we rethrow
    the pointer, catch it,  and then we have the exception itself. Note:
    depending upon the operation there may be several exceptions.
  */
  auto exception_handler = [](exception_list exceptionList) {
    for (std::exception_ptr const& e : exceptionList) {
      try {
        std::rethrow_exception(e);
      } catch (exception const& e) {
        std::terminate();
      }
    }
  };

  // Create a device queue
  queue q(device_selector, exception_handler);

  // Start timers
  auto start = std::chrono::steady_clock::now();

  // Call simulation function
  motion_device_1(q, particleX, particleY, randomX, randomY, grid, grid_size,
                  n_particles, nIterations, radius, map);

  q.wait_and_throw();

  auto end = std::chrono::steady_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();
  cout << std::endl;
  cout << "SYCL: ";
  cout << "Time: " << time << std::endl;
  cout << std::endl;

  // Displays final grid only if grid small.
  if (grid_size <= 45) {
    cout << "\n ********************** OUTPUT GRID: " << std::endl;
    print_matrix<int>(grid, grid_size, grid_size);
  }

  // Cleanup
  for (size_t i = 0; i < grid_size; i++) delete grid[i];

  delete[] grid;
  delete[] particleX;
  delete[] particleY;
  delete[] randomX;
  delete[] randomY;
  delete[] map;

  return 0;
}
