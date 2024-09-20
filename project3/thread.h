// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 1/12/2024
// PROGRAM ASSIGNMENT #3
// FILE NAME : thread.h
// PROGRAM PURPOSE :
// This is the file that contains all of the class definitions 
// -----------------------------------------------------\

#include "ThreadClass.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

class PrefixSum : public Thread
{
public:
	PrefixSum(int** arrarg, int rownum, int colnum);

private:
	int** B;
	int i;
	int j;
	void ThreadFunc();
};

		