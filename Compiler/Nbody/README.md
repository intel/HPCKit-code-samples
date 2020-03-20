# nbody sample
An N-body simulation is a simulation of a dynamical system of particles, usually under the influence of physical forces, such as gravity. This nbody sample code is implemented using C++ and DPC++ language for Intel CPU and GPU. 
  
| Optimized for                       | Description
|:---                               |:---
| OS                                | Linux* Ubuntu* 18.04; Windows 10
| Hardware                          | Skylake with GEN9 or newer
| Software                          | IIntel&reg; oneAPI DPC++ Compiler (beta) 
| What you will learn               | How to offload the computation to GPU using Intel DPC++ compiler
| Time to complete                  | 15 minutes

## Key implementation details 
DPC++ implementation explained. 

## License  
This code sample is licensed under MIT license. 

### Original source
Source: https://github.com/fbaru-dev/particle-sim 
License: MIT license 

## How to Build  

### on Linux  
   * Build Nbody program  
    cd Nbody &&  
    mkdir build &&  
    cd build &&  
    cmake ../. &&  
    make VERBOSE=1  

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
      Run - MSBuild Nbody.sln /t:Rebuild /p:Configuration="Release"

## How to run Offload Advisor on Linux

### Profiling the application
    * export SAPPC=/opt/intel/inteloneapi/advisor/latest/perfmodels
      advixe-python $SAPPC/collect.py ./advisor_nbody_project -- make run
      
### Running Performance Modeling
    * advixe-python $SAPPC/analyze.py --config gen9 ./advisor_nbody_project -o ./advisor_nbody_result
    Open the report.html inside advisor_nbody_result directory to view the Offload Advisor report on identified potential offload regions and performance projections on GEN9 graphics

***Offload Advisor feature is not available on Windows**

## How to run Vtune GPU Hotspot on Linux
    * vtune -collect gpu-hotspots -target-duration-type=veryshort -allow-multiple-runs -app-working-dir . -- make run

## How to run Vtune GPU Hotspot on Windows
    * vtune -collect gpu-hotspots -target-duration-type=veryshort -allow-multiple-runs -app-working-dir . -- x64\Release\nbody.exe
