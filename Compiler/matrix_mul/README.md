# matrix_mul sample
matrix_mul is a simple program that multiplies together two large matrices and verifies the results. 
This program is implemented using two ways: 
    1. C++ and SYCL language;
    2. C++ and OpenMP (Linux* only);
If successful, the name of the offload device and a success message are displayed.

For comprehensive instructions regarding DPC++ Programming, go to https://software.intel.com/en-us/oneapi-programming-guide and search based on relevant terms noted in the comments.
  
| Optimized for                       | Description
|:---                               |:---
| OS                                | Linux* Ubuntu* 18.04, Windows 10*
| Hardware                          | Skylake with GEN9 or newer
| Software                          | Intel&reg; oneAPI DPC++ Compiler beta, Intel&reg; C/C++ Compiler beta
| What you will learn               | Offloads computations on 2D arrays to GPU using Intel DPC++ and OpenMP
| Time to complete                  | 15 minutes  

## Key implementation details
SYCL implementation explained. 
OpenMP offload implementation explained. 

## License  
This code sample is licensed under MIT license. 

## How to Build for DPCPP and SYCL 

### on Linux  
   * Build the program using Make  
    cd matrix_mul &&  
    make all  

   * Run the program  
    make run  

   * Clean the program  
    make clean 

### on Windows
The OpenMP offload target is not supported on Windows yet. 

#### Command Line using MSBuild
   * MSBuild matrix_mul.sln /t:Rebuild /p:Configuration="release"  

#### Command Line using nmake
   Build matrix_mul DPCPP version
   * nmake -f Makefile.win build_dpcpp  
   * nmake -f Makefile.win run_dpcpp  

#### Visual Studio IDE
   * Open Visual Studio 2017     
   * Select Menu "File > Open > Project/Solution", find "matrix_mul" folder and select "matrix_mul.sln" 
   * Select Menu "Project > Build" to build the selected configuration
   * Select Menu "Debug > Start Without Debugging" to run the program

## How to Build for OpenMP

### on Linux  
   * Build the program using Make  
    cd matrix_mul &&  
    make build_omp  

   * Run the program  
    make run_omp  

   * Clean the program  
    make clean

