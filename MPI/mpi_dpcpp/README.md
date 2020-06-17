# MPI-DPCPP sample
This code sample demonstrates how to incorporate Data Parallel PC++ into a MPI program. 
Using Data Parallel C++, the code sample runs multiple MPI ranks to distribute the
calculation of the number pi (~3.1416). Each rank offloads the computation to an accelerator
(GPU/CPU) using Intel DPC++ compiler to compute a partial compution of the number pi..

For comprehensive instructions regarding DPC++ Programming, go to
https://software.intel.com/en-us/oneapi-programming-guide
and search based on relevant terms noted in the comments.

  
| Optimized for                     | Description
|:---                               |:---
| OS                                | Linux Ubuntu 18.04
| Hardware                          | Skylake with GEN9 or newer
| Software                          | Intel&reg; oneAPI DPC++ Compiler (beta); Intel C++ Compiler (beta)
| What you will learn               | How to offload the computation to GPU using Intel DPC++ compiler in a
                                    | MPI program
| Time to complete                  | 10 minutes


## License  
This code sample is licensed under MIT license  

## How to Build  

### on a Linux* System  
   * Build mpi_dpcpp  

    export I_MPI_CXX=dpcpp
    cd mpi_dpcpp &&
    mkdir build &&
    cd build &&
    cmake .. &&
    make -j

   * Run the program

    make run  
   
   * Clean the program  
    make clean

### on Windows
    * Build the program using VS2017 or VS2019
      Right click on the solution file and open using either VS2017 or VS2019 IDE.
      Right click on the project in Solution explorer and select Rebuild.
      From top menu select Debug -> Start without Debugging.

    * Build the program using MSBuild
      Open "x64 Native Tools Command Prompt for VS2017" or "x64 Native Tools Command Prompt for VS2019"
      Run - MSBuild mpi_dpcpp.sln /t:Rebuild /p:Configuration="Release"

## How to Run  
   * Application Parameters   
	
        Usage: mpirun -n <num> -host localhost ./mpi_dpcpp
        <num> : number of MPI rank.

