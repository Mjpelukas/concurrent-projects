// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 1/12/2024
// PROGRAM ASSIGNMENT #3
// FILE NAME : thread-main.cpp
// PROGRAM PURPOSE :
//  This is the file the driver code for the PrefixSum threads. 
//  It takes an input array then calculates the prefix sum and prints it out.
// -----------------------------------------------------


#include "thread.h"

#define BUF 20
#define PRINTBUFSIZE 241
int** B;
// -----------------------------------------------------------
// FUNCTION main :
//  Initializes variables for the main driver code
//	Gets initial input
//  Initializes the threads
//	prints the relevant stuff
//  waits until all threads join and then prints and exits
// PARAMETER USAGE :
//  N/A
// FUNCTION CALLED :
//  log2()
//  PrefixSum()
//  Begin()
//	Join()
// -----------------------------------------------------------
int main(int argc, char* argv[]) {
	//used for printing
	char printBuf[PRINTBUFSIZE];
	int bufOffset = 0;
	int loopOffset = 0;

	int n = 0;
	//get values of n and k
	cin >> n; 
	int k = log2(n) + 1; // log 2 + 1 for number of rows. 
	int x[n];

	//use printf because no concurrency is happening
	printf("Number of input data = %d \n", n);
	printf("Input array: \n");
	for (int i = 0; i < n; i++)	{
		cin >> x[i]; //get value of array at index i
		printf("   %d", x[i]);
	}
	printf("\n");

	// setup memory for B array
	B = (int **) malloc(sizeof(int*) * k); //allocate memory for rows

	for (int i = 0; i < k; i++){ //allocate memory for columns
		B[i] = (int*)malloc(sizeof(int) * n); 
	}

	//write the first row of B with x
	for (int i = 0; i < n; i++){
		B[0][i] = x[i];
	}

	 //make n threads
	PrefixSum* summer[n];

	//starts all threads
	for (int run = 1; run < k; run++) {
		//prrint the run number
		snprintf(printBuf, PRINTBUFSIZE, "Run %d:\n", run);
		write(1, printBuf, strlen(printBuf));

		//for each iteration
		for (int j = 0; j < n; j++) {
						//snprintf(printBuf, PRINTBUFSIZE, "created thread %d\n", j);
						//write(1, printBuf, strlen(printBuf));
			summer[j] = new PrefixSum(B, run, j);
			summer[j]->Begin();
		}
		// wait on threads to finish
		for (int j = 0; j < n; j++) {
			summer[j]->Join();
		}

		//print out the row 
		loopOffset = 0;
		bufOffset = snprintf(printBuf, PRINTBUFSIZE, "Result after run %d:\n   ", run);
		for (int j = 0; j < n; j++) {
			loopOffset = snprintf(printBuf + bufOffset, PRINTBUFSIZE, "%d ", B[run][j]);
			bufOffset = bufOffset + loopOffset;
		}
		sprintf(printBuf + strlen(printBuf), "\n");
		write(1, printBuf, strlen(printBuf));
	}

	//final run
	loopOffset = 0;
	bufOffset = snprintf(printBuf, PRINTBUFSIZE, "Final result after run %d:\n   ", k-1);
	for (int j = 0; j < n; j++) {
		loopOffset = snprintf(printBuf + bufOffset, PRINTBUFSIZE, "%d ", B[k-1][j]);
		bufOffset = bufOffset + loopOffset;
	}
	sprintf(printBuf + strlen(printBuf), "\n");
	write(1, printBuf, strlen(printBuf));


	//free memory
	for (int i = 0; i < k; i++)
	{
		free(B[i]);
	}
	free(B); 
	return 0;
}