# ISO3DFD sample
ISO3DFD is a finite difference stencil kernel for solving 3D acoustic isotropic wave equation which can be used as a proxy for propogating a seismic wave. Kernels in this sample are implemented as 16th order in space, with symmetric coefficients, and 2nd order in time scheme without boundary conditions. This sample code is implemented using OpenMP Offload for GPU and OpenMP for CPU for comparison and validation.
  
| Optimized for                       | Description
|:---                               |:---
| OS                                | Linux Ubuntu 18.04; Windows 10 or Windows Server 2017
| Hardware                          | Kaby Lake with GEN9 or newer
| Software                          | Intel C++ Compiler xxx beta; Intel OpenMP Offload Compiler; oneAPI HPC Toolkit
| What you will learn               | How to offload the computation to GPU using OpenMP offload pragmas
| Time to complete                  | 15 minutes

  
## Key implementation details [optional]
OpenMP Offload implementation explained. 

You're encourage to use any infographics to increase readibility and clarity which is strongly recommended by UX team based on customer feedbacks. 

## License  
This code sample is licensed under MIT license 

## How to Build  

### on Linux  
   * Build iso3dfd_omp_offload  
    
    cd iso3dfd_omp_offload &&  
    mkdir build &&  
    cd build &&  
    cmake .. &&  
    make 

   * Run the program using OpenMP Offload 
    
    make run
   
   * Clean the program  
    
    make clean 

## How to Run  
   * Application Parameters   
	Usage: ./src/iso3dfd n1 n2 n3 b1 b2 b3 Iterations
	
	n1 n2 n3      : Grid sizes for the stencil
	b1 b2 b3      : Cache block sizes for cpu openmp version OR
		      : TILE sizes for OMP Offload
	Iterations    : No. of timesteps.


## Performance Tests
   * OpenMP Offload on GEN
	
    mkdir build
	cmake -DTILING=1 ..
	./src/iso3dfd 256 256 256 16 16 64 100  

## Validation Tests
   * OpenMP Offload on GEN
	
    mkdir build
	cmake -DVERIFY_RESULTS=1 ..
    ./src/iso3dfd 256 256 256 16 16 64 10
