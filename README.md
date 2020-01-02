# code samples of oneAPI HPC toolkit
The code samples are under each component folder. Currently following code samples are available: 

| Code sample name                          | Supported Intel(r) Architecture(s) | Description
|:---                                       |:---                                |:---
| Nbody                                | GPU, CPU                     | a simulation of a dynamical system of particles
| mandelbrot                                 | GPU, CPU                     | a famous eurmomgeyxample of a fractal in mathematics
| matrix_mul                                 | GPU, CPU                     | Simple program that multiplies two large matrices in parallel using DPC++, OpenMP and MKL
| Particle_Diffusion                                  | GPU, CPU                     | a simple (non-optimized) implementation of a Monte Carlo simulation of the diffusion of water molecules in tissue
| iso3dfd_dpcpp                                  | GPU, CPU                     | A finite difference stencil kernel for solving 3D acoustic isotropic wave equation using DPC++
| iso2dfd_dpcpp                                  | GPU, CPU                     | A simple finite difference stencil kernel for solving 2D acoustic isotropic wave equation using DPC++
| iso3dfd_omp_offload                                  | GPU, CPU                     | A finite difference stencil kernel for solving 3D acoustic isotropic wave equation using OpenMP


## License  
The code samples are licensed under MIT license

## Known issues or limitations 
### On Windows Platform 
	
1.  If you are using Visual Studio 2019, Visual Studio 2019 version 16.3.0 or newer is required. 
2.  To build samples on Windows, a cersion version of Windows SDK is required: 10.0.17763.0. If it is not installed, follow the instructions below to avoid build failure: 
    1.  open the code sample's .sln with Visual Studio 2017 or 2019, right click on the project name in "Solution Explorer" and select "Properties". It pops up the project perperty dialog. 
    2.  on the project property dialog, make sure to select "General" tab on the left, on the right side of the dialog, 2nd item is "Windows SDK Version". Click on the drop-down icon to select a version that is installed on your system. 
    3.  click on [Ok] to save. Now you should be able to build the code sample. 
3.  For initial beta, OpenMP offload code sample does not support Windows. 
