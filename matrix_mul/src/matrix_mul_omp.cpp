//==============================================================
// Copyright © 2019 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include <iostream>
#include <float.h>
#include <math.h>
#include <omp.h>

using namespace std;

// Matrix size constants
#define SIZE     1200     // Must be a multiple of 8.
#define M        SIZE/8
#define N        SIZE/4
#define P        SIZE/2

/**
 * Each element of the product matrix c[i][j] is computed from a unique row and
 * column of the factor matrices, a[i][k] and b[k][j]
 */
double a[M][N]; 
double b[N][P]; 
double c[M][P];

/**
 * Perform the matrix multiplication on CPU with OpenMP. 
 */
void MatrixMulOpenMpCpu (double (*a)[N], double (*b)[P], double (*c)[P]);

/**
 * Perform the matrix multiplication on GPU with OpenMP offloading. 
 */
void __attribute__((noinline)) MatrixMulOpenMpGpuOffloading ();

/**
 * Perform the matrix multiplication on host to verify results from OpenMP. 
 */
int VerifyResult(double (*c_back)[P]);

int main(void)
{
    int Result1, Result2;

    cout << "Problem size: c(" << M << "," << P << ") = a(" << M << "," << N << ") * b(" << N << "," << P << ")" << endl;

    cout << "We have " << omp_get_num_devices() << " device(s)" << endl;
  
    cout << "The default device id: " << omp_get_default_device() << endl;

    MatrixMulOpenMpCpu(a, b, c);
    Result1 = VerifyResult(c);

    MatrixMulOpenMpGpuOffloading ();
    Result2 = VerifyResult(c);  

    return Result1 || Result2;
}

void MatrixMulOpenMpCpu (double (*a)[N], double (*b)[P], double (*c)[P])
{
    int i, j, k;
    // a is identity matrix
    for (i=0; i<M; i++)
        for (j=0; j<N; j++)
            a[i][j] = 1.0;

    // each column of b is the sequence 1,2,...,N
    for (i=0; i<N; i++)
        for (j=0; j<P; j++)
            b[i][j] = i+1.;

    for (i=0; i<M; i++)
        for (j=0; j<P; j++)
            c[i][j] = 0.0;
  
    // Parallelize by row.  The threads don't need to synchronize at
    // loop end, so "nowait" can be used.
    #pragma omp for nowait private(i,j,k)
    for (i=0; i<M; i++) {
        for (k=0; k<N; k++) {
            // Each element of the product is just the sum 1+2+...+n
            for (j=0; j<P; j++) {
                c[i][j]  += a[i][k]  * b[k][j];
            }
        }
    }

}


void __attribute__((noinline)) MatrixMulOpenMpGpuOffloading ()
{
    int i, j, k;
    
    // a is identity matrix
    for (i=0; i<M; i++)
        for (j=0; j<N; j++)
            a[i][j] = 1.0;

    // each column of b is the sequence 1,2,...,N
    for (i=0; i<N; i++)
        for (j=0; j<P; j++)
            b[i][j] = i+1.;

    // c is initialized to zero
    for (i=0; i<M; i++)
        for (j=0; j<P; j++)
            c[i][j] = 0;
  
    // Parallelize on target device
    #pragma omp target teams distribute parallel for map(to: a, b) map(tofrom: c) thread_limit(128)
    {
        for (i=0; i<M; i++) {
            for (k=0; k<N; k++) {
                // Each element of the product is just the sum 1+2+...+n
                for (j=0; j<P; j++) {
                    c[i][j]  += a[i][k]  * b[k][j];
                }
            }
        }
    }

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





