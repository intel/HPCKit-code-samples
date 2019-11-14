//==============================================================
// Copyright © 2019 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include <CL/sycl.hpp>
#include <iostream>

using namespace std;
using namespace cl::sycl;

/**
 * Each element of the product matrix c[i][j] is computed from a unique row and
 * column of the factor matrices, a[i][k] and b[k][j]
 */
 
// Matrix size constants
#define SIZE     1200     // Must be a multiple of 8.
#define M        SIZE/8
#define N        SIZE/4
#define P        SIZE/2

/**
 * Perform the matrix multiplication on host to verify results from device. 
 */
int VerifyResult(double (*c_back)[P]);

int main() {
    // host memory buffer that device will write data back before destruction
    double (*c_back)[P] = new double[M][P];

    // intialize c_back
    for (int i=0; i<M; i++)
        for (int j=0; j<P; j++)
            c_back[i][j] = 0.0;

    // By sticking all the SYCL work in a {} block, we ensure
    // all SYCL tasks must complete before exiting the block,
    // because the destructor of resultBuf will wait.
    {
    
        // Initializing the devices queue with the default selector
        // The device queue is used to enqueue the kernels and encapsulates
        // all the states needed for execution  
        default_selector device_selector;
        queue device_queue(device_selector);
        

        std::cout << "Device: "
            << device_queue.get_device().get_info<info::device::name>()
            << std::endl;

        // Creating 2D buffers for matrices, buffer c is bound with host memory address c_back
        buffer<double, 2> a(range<2>{M, N});
        buffer<double, 2> b(range<2>{N, P});
        buffer<double, 2> c(reinterpret_cast<double*>(c_back), range<2>{M, P});

        cout << "Problem size: c(" << M << "," << P << ") = a(" << M << "," << N << ") * b(" << N << "," << P << ")" << std::endl;

        //Using three command groups to illustrate execution orders. 
        //The way to use the first two command groups to initialize matrices
        //is not the most efficient way. Just for demonstrating the implicit 
        //multiple command group execution ordering.

        // Submitting command group to queue to initialize matrix a
        device_queue.submit([&](handler &cgh) {
            // Getting write only access to the buffer on a device
            auto Accessor = a.get_access<access::mode::write>(cgh);
            // Executing kernel
            cgh.parallel_for<class FillBuffer_a>( range<2>{M, N}, [=](id<2> index) {
                // a is identity matrix
                Accessor[index] = 1.0;
            });
        });
    
        //Submitting command group to queue to initialize matrix b
        device_queue.submit([&](handler &cgh) {
            // Getting write only access to the buffer on a device
            auto Accessor = b.get_access<access::mode::write>(cgh);
            //Executing kernel
            cgh.parallel_for<class FillBuffer_b>( range<2>{N, P}, [=](id<2> index){
	        // each column of b is the sequence 1,2,...,N	    
                Accessor[index] = index[0] + 1.;
            });    
        });   

        //Submitting command group to queue to compute matrix mulitiplication c=a*b
        device_queue.submit([&](handler &cgh){
            // Read from a and b, write to c
            auto A = a.get_access<access::mode::read>(cgh);
            auto B = b.get_access<access::mode::read>(cgh);
            auto C = c.get_access<access::mode::write>(cgh);

            int WidthA = a.get_range()[1];

            //Executing kernel
            cgh.parallel_for<class MatrixMult>(range<2>{M, P}, [=](id<2> index){
	        //Get global position in Y direction
	        int row = index[0];
	        //Get global position in X direction
	        int col = index[1];

	        double sum = 0.0;
	        //Compute the result of one element in c
	        for (int i = 0; i < WidthA; i++) {
	            sum += A[row][i] * B[i][col];
	        }

	        C[index] = sum;
            });

        });
    }    //End of scope, so we wait for kernel producing result data to host memory c_back to complete
   
    int result; 
    result = VerifyResult(c_back);

    
    delete[] c_back;  

    return result;
}

int VerifyResult(double (*c_back)[P]) {
    // Check that the results are correct by comparing with host computing
    int i, j, k;

    // 2D arrays on host side
    double (*a_host)[N];
    double (*b_host)[P];
    double (*c_host)[P];

    a_host = new double[M][N];
    b_host = new double[N][P];
    c_host = new double[M][P];
    
    // a_host is identity matrix
    for (i=0; i<M; i++)
        for (j=0; j<N; j++)
            a_host[i][j] = 1.0;

    // each column of b_host is the sequence 1,2,...,N
    for (i=0; i<N; i++)
        for (j=0; j<P; j++)
            b_host[i][j] = i+1.;

    // c_host is initialized to zero
    for (i=0; i<M; i++)
        for (j=0; j<P; j++)
            c_host[i][j] = 0;

    
    for (i=0; i<M; i++) {
        for (k=0; k<N; k++) {
            // Each element of the product is just the sum 1+2+...+n
            for (j=0; j<P; j++) {
                c_host[i][j] += a_host[i][k]  * b_host[k][j];
            }
            
        }
    }

    bool MismatchFound = false;

    //compare host side results with the result buffer from device side: print fail data 5 times only. 
	int printf_count = 0; 
    for (i=0; i < M; i++) {
        for (j=0; j < P; j++) {
            if (c_back[i][j] != c_host[i][j]) {
                cout << "fail - The result is incorrect for element: [" << i << ", " << j
                << "], expected: " << c_host[i][j] << " , but got: " << c_back[i][j]
                << std::endl;
                MismatchFound = true;
				printf_count++; 
				if (printf_count >= 5)
					break; 
            }
        }
		if (printf_count >= 5)
			break;
	}

    delete[] a_host;
    delete[] b_host;
    delete[] c_host;

    if (!MismatchFound) {
        cout << "success - The results are correct!" << std::endl;
        return 0;
    }
    else {
        cout << "fail - The results mis-match!" << std::endl;
        return -1;
    }

}

