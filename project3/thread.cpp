// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 1/12/2024
// PROGRAM ASSIGNMENT #3
// FILE NAME : thread.cpp
// PROGRAM PURPOSE :
//  This is the file that contains the class implementations for the
//  PrefixSum threads. 
//  It also contains the implementaion of Threadfunc
// -----------------------------------------------------

#include "thread.h"
#include <iostream>

#define BUF 20
#define PRINTBUFSIZE 241
// -----------------------------------------------------------
// FUNCTION Cannibal::Cannibal :
//  Constructor implementation for the Cannibal class
// PARAMETER USAGE :
//  arrarg - the pointer to the Array B
//	rownum - the row number in the array B this thread will work with also used to calculate gap
//	colnum - the column number in the array B this thread will work with 
// FUNCTION CALLED :
//  pow()
// -----------------------------------------------------------
PrefixSum::PrefixSum(int** arrarg, int rownum, int colnum) {
	B = arrarg;
	i = rownum;
	j = colnum;
}


// -----------------------------------------------------------
// FUNCTION: PrefixSum::ThreadFunc() 
//  computes the value that will be put in the global array and places it in
// PARAMETER USAGE :
//  N/A
// FUNCTION CALLED :
//	N/A
// -----------------------------------------------------------


void PrefixSum::ThreadFunc() {
	Thread::ThreadFunc();

	//prints threadnum
	char printBuf[PRINTBUFSIZE];
	snprintf(printBuf, PRINTBUFSIZE, "      Thread %d Created\n", j);
	write(1, printBuf, strlen(printBuf));

	//find gap
	int gap = (int)pow(2, i - 1);
	//make decision on what to write
	if (j - gap < 0) {
		B[i][j] = B[i - 1][j]; //copies from last row
		snprintf(printBuf, PRINTBUFSIZE, "      Thread %d copies x[%d]\n", j, j);
		write(1, printBuf, strlen(printBuf));
	}
	else {
		int threadSum = B[i - 1][j] + B[i - 1][j - gap]; //adds from last row in array to get value for this row.
		B[i][j] = threadSum;
		//prints out
		snprintf(printBuf, PRINTBUFSIZE, "      Thread %d computes x[%d] + x[%d]\n", j, j, j-gap);
		write(1, printBuf, strlen(printBuf));
	}
	
	//prints that the thread exits
	snprintf(printBuf, PRINTBUFSIZE, "      Thread %d Exits\n", j);
	write(1, printBuf, strlen(printBuf));

	Exit();
}