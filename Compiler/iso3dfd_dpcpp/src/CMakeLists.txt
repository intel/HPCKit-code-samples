set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
#set(CMAKE_BUILD_TYPE "RelWithDebInfo")

OPTION(SHARED_KERNEL "Use SLM Kernel Version - Only for GPU" OFF)
if(SHARED_KERNEL)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_SHARED")
endif(SHARED_KERNEL)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

add_executable (iso3dfd.exe iso3dfd.cpp iso3dfd_kernels.cpp utils.cpp)
target_link_libraries(iso3dfd.exe OpenCL sycl)
if(WIN32)
        add_custom_target (run iso3dfd.exe 256 256 256 32 8 64 10 sycl gpu)
        add_custom_target (run_cpu iso3dfd.exe 256 256 256 256 1 1 10 sycl cpu)
else()
        add_custom_target (run iso3dfd.exe 256 256 256 32 8 64 10 sycl gpu)
        add_custom_target (run_cpu iso3dfd.exe 256 256 256 256 1 1 10 sycl cpu)
endif()
