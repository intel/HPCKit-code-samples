export I_MPI_CXX=dpcpp
CXX = mpiicpc
CXXFLAGS = -O2 -g -fsycl -std=c++17
LDFLAGS = -lOpenCL -lsycl
EXE_NAME = mpi_dpcpp
SOURCES = src/mpi_dpcpp.cpp

all: main

main:
	$(CXX) $(CXXFLAGS) -o $(EXE_NAME) $(SOURCES) $(LDFLAGS)

run: 
	mpirun -n 2 ./$(EXE_NAME)

clean: 
	rm -rf $(EXE_NAME) 
