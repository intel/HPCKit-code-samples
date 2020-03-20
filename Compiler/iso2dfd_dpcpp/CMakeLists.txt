# required cmake version
cmake_minimum_required(VERSION 3.5)

# CMakeLists.txt for ISO2DFD_DPCPP project
project (iso2dfd_dpcpp)

set(CMAKE_CXX_COMPILER "dpcpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fsycl -std=c++11")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lOpenCL -lsycl")

add_executable (iso2dfd src/iso2dfd.cpp)

add_custom_target (run 
	COMMAND iso2dfd 1000 1000 2000 
	WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

