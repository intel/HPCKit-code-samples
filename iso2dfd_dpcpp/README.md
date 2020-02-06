# ISO2DFD sample
ISO2DFD is a finite difference stencil kernel for solving the 2D acoustic isotropic wave equation. Kernels in this sample are implemented as 2nd order in space, 2nd order in time scheme without boundary conditions. This sample code is implemented using Data Parallel C++ for CPU and GPU. 

Find more information about the algorithm and implementation in the associated tutorial at https://software.intel.com/en-us/articles/code-sample-two-dimensional-finite-difference-wave-propagation-in-isotropic-media-iso2dfd
  
| Optimized for                       | Description
|:---                               |:---
| OS                                | Linux Ubuntu 18.04
| Hardware                          | Skylake with GEN9 or newer
| Software                          | Intel&reg; oneAPI DPC++ Compiler (beta); Intel C++ Compiler (beta)
| What you will learn               | How to offload the computation to GPU using Intel DPC++ compiler
| Time to complete                  | 10 minutes

  
## Key implementation details [optional]
SYCL implementation explained. 

## License  
This code sample is licensed under MIT license  

## How to Build  

### on Linux  
   * Build iso2dfd_dpcpp  
    
    cd iso2dfd_dpcpp &&  
    mkdir build &&  
    cd build &&  
    cmake .. &&  
    make -j 

   * Run the program on Gen9 
    
    make run  
   
   * Clean the program  
    make clean  

## How to Run  
   * Application Parameters   
	
 	Usage: ./iso2dfd n1 n2 b1 b2 Iterations

	 n1 n2      : Grid sizes for the stencil
	 b1 b2      : NDrange
	 Iterations : No. of timesteps.

