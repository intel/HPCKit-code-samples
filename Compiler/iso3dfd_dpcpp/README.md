# ISO3DFD sample
ISO3DFD: Intel® oneAPI DPC++ Language Basics Using 3D-Finite-Difference-Wave
Propagation

ISO3DFD is a finite difference stencil kernel for solving the 3D acoustic
isotropic wave equation which can be used as a proxy for propogating a seismic wave. 
Kernels in this sample are implemented as 16th order
in space, with symmetric coefficients, and 2nd order in time scheme without boundary conditions.. 
Using Data Parallel C++, the sample can explicitly run on the GPU and/or CPU to
propagate a seismic wave which is a compute intensive task.  
If successful, the output will print the device name
where the DPC++ code ran along with the grid computation metrics - flops
and effective throughput

For comprehensive instructions regarding DPC++ Programming, go to
https://software.intel.com/en-us/oneapi-programming-guide 
and search based on relevant terms noted in the comments.
  
| Optimized for                       | Description
|:---                               |:---
| OS                                | Linux Ubuntu 18.04
| Hardware                          | Skylake with GEN9 or newer
| Software                          | Intel&reg; oneAPI DPC++ Compiler (beta); Intel C++ Compiler xxx beta
| What you will learn               | How to offload the computation to GPU using Intel DPC++ compiler
| Time to complete                  | 15 minutes

Performance number tabulation [Dummy data for now]

| iso3dfd                           | Performance data
|:---                               |:---
| Scalar baseline -O2               | 1.0

  
## Key implementation details [optional]
SYCL implementation explained. 

You're encourage to use any infographics to increase readibility and clarity which is strongly recommended by UX team based on customer feedbacks. 


## License  
This code sample is licensed under MIT license  

## How to Build  

### on Linux  
   * Build iso3dfd_dpcpp  
    
    cd iso3dfd_dpcpp &&  
    mkdir build &&  
    cd build &&  
    cmake .. &&  
    make -j 

   * Run the program on Gen9 
    
    make run  
   
   * Run the program on CPU 
    
    make run_cpu  

   * Clean the program  
    make clean  

## How to Run  
   * Application Parameters   
	
	Usage: src/iso3dfd.exe n1 n2 n3 n1_Tblock n2_Tblock n3_Tblock Iterations [omp|sycl] [gpu|cpu]

 	n1 n2 n3                               : Grid sizes for the stencil
 	n1_Tblock n2_Tblock n3_Tblock          : cache block sizes for cpu openmp version.
 	OR n1_Tblock n2_Tblock                 : Thread block sizes in X and Y dimension for SYCL version.
        	and n3_Tblock                  : size of slice of work in z-dimension for SYCL version.
 	Iterations                             : No. of timesteps.
 	[omp|sycl]                             : Optional: Run the OpenMP or the SYCL variant. Default is to use both for validation
 	[gpu|cpu]                              : Optional: Device to run the SYCL version Default is to use the GPU if available, if not fallback to CPU


## Performance Tests
   * DPC++ on CPU
	
    numactl -c 0 -m 0 src/iso3dfd.exe 256 256 256 256 1 1 100 sycl cpu   
   * DPC++ on Gen9   
	
    src/iso3dfd.exe 256 256 256 32 8 64 100 sycl gpu   
	  

## Validation Tests
   * DPC++ on CPU
	
    numactl -c 0 -m 0 src/iso3dfd.exe 256 256 256 256 1 1 10 cpu   
   * DPC++ on Gen9   
    
    src/iso3dfd.exe 256 256 256 32 8 64 10 gpu   

### on Windows
    * Build the program using VS2017 or VS2019
      Right click on the solution file and open using either VS2017 or VS2019 IDE.
      Right click on the project in Solution explorer and select Rebuild.
      From top menu select Debug -> Start without Debugging.

    * Build the program using MSBuild
      Open "x64 Native Tools Command Prompt for VS2017" or "x64 Native Tools Command Prompt for VS2019"
      Run - MSBuild iso3dfd_dpcpp.sln /t:Rebuild /p:Configuration="Release"