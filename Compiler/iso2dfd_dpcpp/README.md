# ISO2DFD sample
ISO2DFD: Intel® oneAPI DPC++ Language Basics Using 
2D-Finite-Difference-Wave Propagation

ISO2DFD is a finite difference stencil kernel for solving the 
2D acoustic isotropic wave equation. Kernels in this sample 
are implemented as 2nd order in space, 2nd order in time scheme 
without boundary conditions. 
Using Data Parallel C++, the sample will explicitly run on the 
GPU as well as CPU to calculate a result.
If successful, the output will include GPU device name.

A complete online tutorial for this code sample can be found at :
https://software.intel.com/en-us/articles/code-sample-two-dimensional-finite-difference-wave-propagation-in-isotropic-media-iso2dfd

For comprehensive instructions regarding DPC++ Programming, go to
https://software.intel.com/en-us/oneapi-programming-guide
and search based on relevant terms noted in the comments.

  
| Optimized for                     | Description
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
	
 	Usage: ./iso2dfd n1 n2 Iterations

	 n1 n2      : Grid sizes for the stencil
	 Iterations : Number of timesteps.

   * Find graphical output for sample execution in the online tutorial at:
     https://software.intel.com/en-us/articles/code-sample-two-dimensional-finite-difference-wave-propagation-in-isotropic-media-iso2dfd
