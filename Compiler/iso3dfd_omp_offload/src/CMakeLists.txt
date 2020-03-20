OPTION(TILING "Use Tiling Optimizations" OFF)
OPTION(VERIFY_RESULTS "Use Results Validation" OFF)

set(CMAKE_BUILD_TYPE "RelWithDebInfo")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qnextgen -fiopenmp -std=c++11 -fopenmp-targets=spir64 -O3 -D__STRICT_ANSI__ ")

set(SOURCES iso3dfd.cpp utils.cpp)

if(TILING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_TILED")
endif(TILING)

if(VERIFY_RESULTS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERIFY_RESULTS")
	set(SOURCES ${SOURCES} iso3dfd_verify.cpp)
endif(VERIFY_RESULTS)


#add_executable (iso3dfd iso3dfd.cpp utils.cpp iso3dfd_verify.cpp)
add_executable (iso3dfd ${SOURCES})

add_custom_target (run 
	COMMAND iso3dfd 256 256 256 32 8 64 10
	WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

