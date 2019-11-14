//==============================================================
// Copyright © 2019 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include "mandelSerial.h"
#include "mandelParallel.h"
#include "syclObject.h"

int main(int argc, const char *argv[]) 
{

	const int ROW_SIZE = 120;
	const int COL_SIZE = 120;
	const int MAX_ITERATIONS = 100;

    SYCLObj syclObj;
    syclObj.setUpQueue(); 
    syclObj.showDevice();

    MandelParallel<ROW_SIZE, COL_SIZE, MAX_ITERATIONS> mandelParallel; 
    mandelParallel.Evaluate(syclObj); 
    mandelParallel.Print();

	// validating with serial result
    MandelSerial mandelSerial(ROW_SIZE, COL_SIZE, MAX_ITERATIONS);   
	mandelSerial.Evaluate();  
	// mandelSerial.Print();

	// validating
	int ret = mandelParallel.ValidatingResult(mandelSerial.GetBuffer(), mandelSerial.GetRowCount(), mandelSerial.GetColCount());
	if (ret == 0)
		std::cout << "Success!" << std::endl;
	else
		std::cout << "Failed!" << std::endl;

	return ret; 
}
