# 1D-Heat-Transfer-DPCPP sample
This code sample demonstrates the simulation of one dimentional heat transfer using Intel
Data Parallel C++.

The equation used in this code:
U(i) = C * (U(i+1) - 2 * U(i) + U(i-1)) + U(i)
where constant C = k * dt / (dx * dx)

The kernel executes the calculation in parallel.

For comparison, the heat transfer is computed in serial in CPU. The results are stored in
a file.

For comprehensive instructions regarding DPC++ Programming, go to
https://software.intel.com/en-us/oneapi-programming-guide
and search based on relevant terms noted in the comments.

  
| Optimized for                     | Description
|:---                               |:---
| OS                                | Linux Ubuntu 18.04
| Hardware                          | Skylake with GEN9 or newer
| Software                          | Intel&reg; oneAPI DPC++ Compiler (beta); Intel C++ Compiler (beta)
| What you will learn               | How to simulate 1D Heat Transfer using Intel DPC++ compiler
| Time to complete                  | 10 minutes


## License  
This code sample is licensed under MIT license  

## How to Build  

### on a Linux* System  
   * Build 1d_HeatTransfer
    
    cd 1d_HeatTransfer &&  
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
      Open "x64 Native Tools Command Prompt for VS2017" or "x64 Native Tools Command Prompt for
 VS2019"
      Run - MSBuild 1d_HeatTransfer.sln /t:Rebuild /p:Configuration="Release"

## How to Run  
   * Application Parameters   
	
        Usage: 1d_HeatTransfer <num_point> <num_iteration>


