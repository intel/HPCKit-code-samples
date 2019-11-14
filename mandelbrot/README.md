# Mandelbrot 
  
## Key implementation details 
Mandelbrot is a set that contains all complex numbers c such that z(n + 1) = z(n)^2 + c remains bounded and does not diverge. n starts from 0 and the plot generated is unique for every n. 

mandelCPP.hpp contains the serial implementation: we start by initializing a matrix of 120 x 120. Then, for each element in the matrix, we calculate a point by generating a complex number corresponding to the element's row and column indices and returning the number of iterations such that the absolute value of the complex number is less than 2. The maximum number of iterations allowed is 100 for sake of computation time. Then, we draw a point if the number of iterations is greater than or equal to the maximum number of iterations, aka if the complex number didn't diverge.  

mandleSYCL.hpp contains the parallel implementation. Note that the matrix is still 120 x 120 and the maximum number of iterations is 100. When iterating through the matrix, we use parallel_for to invoke the kernel function. The global range is passed in and the local range is chosen at runtime. The global id of the current work-item the kernel is executed for is also passed in to parallel_for. Then, each work-item, which processes one element in the matrix, executes the kernel code -- a complex number is generated and the number of iterations until divergence is computed. Displaying the image is done in serial.  

| Optimized for                       | Description
|:---                               |:---
| OS                                | Linux* Ubuntu* 18.04; Windows 10
| Hardware                          | Skylake with GEN9 or newer
| Software                          | IIntel&reg; oneAPI DPC++ Compiler beta;
| What you will learn               | How to offload the computation to GPU using Intel DPC++ compiler
| Time to complete                  | 15 minutes

## License  
This code sample is licensed under MIT license. 

## How to Build for CPU and GPU 

### on Linux
*  Build the program using CMake
     cd mandelbrot && 
     mkdir build &&
     cd build &&
     cmake .. &&
     make

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
      Run - MSBuild mandelbrot.sln /t:Rebuild /p:Configuration="Release"
