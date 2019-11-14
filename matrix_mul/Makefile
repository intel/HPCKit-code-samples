DPCPP_CXX = dpcpp
DPCPP_CXXFLAGS = -o
DPCPP_LDFLAGS = 
DPCPP_EXE_NAME = matrix_mul_dpc
DPCPP_SOURCES = src/matrix_mul_dpcpp.cpp

CXX = icc
OMP_CXXFLAGS = -qnextgen -fiopenmp -fopenmp-targets=spir64 -D__STRICT_ANSI__ -o
OMP_LDFLAGS = 
OMP_EXE_NAME = matrix_mul_omp
OMP_SOURCES = src/matrix_mul_omp.cpp

MKL_CXXFLAGS = -I$(MKLROOT)/include -DMKL_ILP64
MKL_LDFLAGS = ${MKLROOT}/lib/intel64/libmkl_sycl.a  -L${MKLROOT}/lib/intel64 -lmkl_intel_ilp64 -lmkl_sequential -lmkl_core -lOpenCL
MKL_EXE_NAME = matrix_mul_mkl
MKL_SOURCES = src/matrix_mul_mkl.cpp

all:
	$(DPCPP_CXX) $(DPCPP_CXXFLAGS) $(DPCPP_EXE_NAME) $(DPCPP_SOURCES) $(DPCPP_LDFLAGS)

build_dpcpp:
	$(DPCPP_CXX) $(DPCPP_CXXFLAGS) $(DPCPP_EXE_NAME) $(DPCPP_SOURCES) $(DPCPP_LDFLAGS)

build_omp:
	$(CXX) $(OMP_CXXFLAGS) $(OMP_EXE_NAME) $(OMP_SOURCES) $(OMP_LDFLAGS)

build_mkl:
	$(DPCPP_CXX) $(MKL_CXXFLAGS) $(DPCPP_CXXFLAGS) $(MKL_EXE_NAME) $(MKL_SOURCES) $(MKL_LDFLAGS)


run:
	./$(DPCPP_EXE_NAME)

run_dpcpp:
	./$(DPCPP_EXE_NAME)

run_omp:
	./$(OMP_EXE_NAME)

run_mkl:
	./$(MKL_EXE_NAME)


clean: 
	rm -rf $(DPCPP_EXE_NAME) $(OMP_EXE_NAME) $(MKL_EXE_NAME)



