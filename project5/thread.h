// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 11/29/2023
// PROGRAM ASSIGNMENT #5
// FILE NAME : thread.h
// PROGRAM PURPOSE :
// This is the file that contains all of the class definitions for
//  cannibals, missionaries, and the boatthread
// -----------------------------------------------------
#pragma once
#include "ThreadClass.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

class Cannibal :public Thread {
public:
	Cannibal(int c);

private:
	void ThreadFunc();

	int canNum; //id of cannibal
};

class Missionary :public Thread {
public:
	Missionary(int m);

private:
	void ThreadFunc();

	int misNum; //id of missionary
};



class BoatThread :public Thread {
	public:
		BoatThread(int b);

	private:
		void ThreadFunc();

		int maxLoads; //maximum loads
		int passengers[4];
};