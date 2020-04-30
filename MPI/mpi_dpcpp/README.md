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
    
    cd mpi_dpcpp &&   
    make -j 

   * Run the program

    make run  
   
   * Clean the program  
    make clean

### On a Windows* System Using a Command Line Interface
   * Select **Programs** > **Intel oneAPI 2021** > **Intel oneAPI Command Prompt** to launch a command window.
   * Build the program  mpi_dpcpp.exe using the following `nmake` commands:

    cd mpi_dpcpp &&
    WinScript.bat

   * Run the program

    mpiexec -n 2 -host localhost ./mpi_dpcpp.exe

   * Clean the program  

    del mpi_dpcpp*

## How to Run  
   * Application Parameters   
	
        Usage: mpirun -n <num> -host localhost ./mpi_dpcpp
        <num> : number of MPI rank.

