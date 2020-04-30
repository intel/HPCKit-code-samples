CXX = dpcpp
CXXFLAGS = -O2 -g -fsycl -std=c++17
LDFLAGS = -lOpenCL -lsycl
EXE_NAME = 1d_HeatTransfer
SOURCES = src/1d_HeatTransfer.cpp

all: main

main:
	$(CXX) $(CXXFLAGS) -o $(EXE_NAME) $(SOURCES) $(LDFLAGS)

run: 
	./$(EXE_NAME) 100 1000

clean: 
	rm -rf $(EXE_NAME) 
