# water molecule diffusion sample
motionsim: Intel® oneAPI DPC++ Language Basics Using a Monte Carlo Simulation

This code sample implements a simple example of a Monte Carlo simulation of the diffusion of water molecules in tissue. This kind of computational experiment can be used to simulate acquisition of a diffusion signal for dMRI.

The model for the simulation consists of water molecules moving through a 2D array of cells in a tissue sample (water molecule diffusion). In this code sample, we use a uniform rectilinear 2D array of digital cells, where cells are spaced regularly along each direction and are represented by circles.

Water molecule diffusion is simulated by defining a number of particles P (simulated water molecules) at random positions in the grid, followed by random walks of these particles in the ensemble of cells in the grid. During the random walks, particles can move randomly inside or outside simulated cells. The positions of these particles at every time step in the simulation, the number of times they go through a cell membrane (in/out), as well as the time every particle spends inside and outside cells can be recorded. These measurements are a simple example of useful information that can be used to simulate an MR signal. 

For comprehensive instructions regarding DPC++ Programming, go to
https://software.intel.com/en-us/oneapi-programming-guide
and search based on relevant terms noted in the comments.

 For more information and details: https://software.intel.com/en-us/articles/vectorization-of-monte-carlo-simulation-for-diffusion-weighted-imaging-on-intel-xeon
  
| Optimized for                       | Description
|:---                               |:---
| OS                                | Linux Ubuntu 18.04; Windows 10 or Windows Server 2017
| Hardware                          | Kaby Lake with GEN9 or newer
| Software                          | Intel Data Parallel C++ Compiler (beta)
| What you will learn               | How to offload the computation to GPU using Intel DPC++ compiler
| Time to complete                  | 15 minutes

Performance number tabulation [if applicable]

| motionsim sample                      | Performance data
|:---                               |:---
| Scalar baseline -O2               | 1.0
| SYCL                              | 
| OpenMP offload                    | 

  
## Key implementation details [optional]
SYCL implementation explained. 


## How other tools (Intel libraries or Intel tools) are used [optional]
   

## License  
This code sample is licensed under MIT license  

## How to Build  

### on Windows - Visual Studio 2017 or newer
   * Build the program using VS2017 or VS2019  
    Right click on the solution file and open using either VS2017 or VS2019 IDE  
    Right click on the project in Solution explorer and select Rebuild  
    From top menu select Debug -> Start without Debugging  

   * Build the program using MSBuild  
    Open "x64 Native Tools Command Prompt for VS2017" or "x64 Native Tools Command Prompt for VS2019"  
    Run - MSBuild Particle_Diffusion.sln /t:Rebuild /p:Configuration="Release"  

### on Linux  
   * Build motionsim program  
    cd Particle_Diffusion &&  
    mkdir build &&  
    cd build &&  
    cmake ../. &&  
    make VERBOSE=1  

   * Run the program  
    make run  

   * Clean the program  
    make clean  

Example execution:

type src/motionsim.exe 10000 777 (or type 'make run')

$ src/motionsim.exe 10000 777

 Running on:: Intel(R) Gen9 HD Graphics NEO
 The Device Max Work Group Size is : 256
 The Device Max EUCount is : 72
 The number of iterations is : 10000
 The number of particles is : 20

Offload: Time: 138


 ********************** OUTPUT GRID:

  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0   0   0   0   0 367  27  16   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0   0   0   0  84 750  98  84   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0   0   0   0 669   0 116  55   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0 130 211 250 170  35  30 261   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0  10 353 539 243 809  61 878 174   0   0   0   0   0   0  
  0   0   0   0   0   0   1 118 1628 1050 1678 887 864 272  80 390   0   0   0   0   0  
  0   0   0   0   0   0   0  39 1173 1660 3549 1263 1155 2185 234   0   0   0   0   0   0  
  0   0   0   0   0   0 306 599 609 537 550 1134 1172 1261  13   0   0   0   0   0   0  
  0   0   0   0   0   0 283 120  92 282 851 512 658 872  40   0   0   0   0   0   0  
  0   0   0   0   0 157 284 133 817 151 175 271 147 286  57   0   0   0   0   0   0  
  0   0   0   0   0   0 294 428   0   0   0   0   0  17   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0 364   0   0   0   0   0   0   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0 182   0   0   0   0   0   0   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  
  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  

$

