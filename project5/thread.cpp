// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 11/29/2023
// PROGRAM ASSIGNMENT #4
// FILE NAME : thread.cpp
// PROGRAM PURPOSE :
//  This is the file that contains the class implementations for the
//  cannibal, missionary and boatthread threads. 
//  It also contains the implementaion of Threadfunc for all three
// -----------------------------------------------------

#pragma once
#include "thread.h"
#include "boat-monitor.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cmath>


// make the boat thread first
static BoatMonitor* boat = new BoatMonitor("canoe");


// -----------------------------------------------------------
// FUNCTION Cannibal::Cannibal :
//  Constructor implementation for the Cannibal class
// PARAMETER USAGE :
//  c - the identifying number of the Cannibal
// FUNCTION CALLED :
//  none
// -----------------------------------------------------------
Cannibal::Cannibal(int c)
	:canNum(c){
}

// -----------------------------------------------------------
// FUNCTION: Missionary::Missionary :
//  Constructor implementation for the Missionary class
// PARAMETER USAGE :
//  m - the identifying number of the Missionary
// FUNCTION CALLED :
//  none
// -----------------------------------------------------------
Missionary::Missionary(int m)
	:misNum(m) {
}


// -----------------------------------------------------------
// FUNCTION: BoatThread::BoatThread:
//  Constructor implementation for the Boat Thread
// PARAMETER USAGE :
//  int b - the number of safe boat loads before the river becomes uncrossable
// FUNCTION CALLED :
//  none
// -----------------------------------------------------------
BoatThread::BoatThread(int b)
	:maxLoads(b) {
}

// -----------------------------------------------------------
// FUNCTION: Cannibal::ThreadFunc() 
//  Announces that it starts and then starts to loop infinitely
//	it prints off that it arrives on the side of the river starting on the 
//	column that matches canNum annd then it calls cannibalArrives the restarts the loop
//	This loops infinitely until it is culled by a join in thread-main
// PARAMETER USAGE :
//  N/A
// FUNCTION CALLED :
//	CannibalArrives()
// 
//	Delay()
// -----------------------------------------------------------
void Cannibal::ThreadFunc() {
	Thread::ThreadFunc();
	char spacer = ' ';
	char printBuf[100];

	//prints like this so Cannibal 1 will be in column 1
	if (canNum == 1) {
		sprintf(printBuf, "Cannibal %d starts\n", canNum);
	}
	else {
		sprintf(printBuf, "%*cCannibal %d starts\n", canNum - 1, spacer, canNum);
	}
	write(1, printBuf, strlen(printBuf));

	while (1) {			//loop until river closes
		Delay();	//Delay for realism

		if (canNum == 1) {
			sprintf(printBuf, "Cannibal %d arrives\n", canNum);
		}
		else {
			sprintf(printBuf, "%*cCannibal %d arrives\n", canNum - 1, spacer, canNum);
		}
		write(1, printBuf, strlen(printBuf));

		boat->CannibalArrives(canNum);	//enters monitor and can get blocked
	}

	Exit();
};





// -----------------------------------------------------------
// FUNCTION: Missionary::ThreadFunc() 
//  Announces that it starts and then starts to loop infinitely
//	it prints off that it arrives on the side of the river starting on the 
//	column that matches canNum annd then it calls missionaryArrives the restarts the loop
//	This loops infinitely until it is culled by a join in thread-main
// PARAMETER USAGE :
//  N/A
// FUNCTION CALLED :
//	missionaryArrives()
// 
//	Delay()
// -----------------------------------------------------------
void Missionary::ThreadFunc() {
	Thread::ThreadFunc();
	char spacer = ' ';
	char printBuf[100];

	//prints like this so Cannibal 1 will be in column 1
	if (misNum == 1) {
		sprintf(printBuf, "Missionary %d starts\n", misNum);
	}
	else {
		sprintf(printBuf, "%*cMissionary %d starts\n", misNum - 1, spacer, misNum);
	}
	write(1, printBuf, strlen(printBuf));

	while (1) {			//loop until river closes
		Delay();	//Delay for realism

		if (misNum == 1) {
			sprintf(printBuf, "Missionary %d arrives\n", misNum);
		}
		else {
			sprintf(printBuf, "%*cMissionary %d arrives\n", misNum - 1, spacer, misNum);
		}
		write(1, printBuf, strlen(printBuf));

		boat->MissionaryArrives(misNum);	//enters monitor and can get blocked
	}
	Exit();
};





// -----------------------------------------------------------
// FUNCTION: boatThread::ThreadFunc() 
//  Controls the main functionality for the boatThread
//	Loops thorugh infinitely until it has done the required number of boat trips in maxLoads
//	starts by calling BoatReady and recieves the array of passengers and what type they 
//  are with missionary or cannibal and prints based on this, then it waits to cross and 
//	calls BoatDone(). if River closed is true, then it breaks the loop, otherwise it continues looping.
// PARAMETER USAGE :
//  N/A
// FUNCTION CALLED :
//  BoatReady()
//	BoatDone()
// 
//	Delay()
// -----------------------------------------------------------
void BoatThread::ThreadFunc() {
	Thread::ThreadFunc();

	char printBuf[60];

	sprintf(printBuf, "***** BOAT thread starts\n");
	write(1, printBuf, strlen(printBuf));

	int k, riverClosed = 0;
	int tripNumber = 0;
	int passengersArray[4];

	while (1) {		//while the river is not closed
		Delay();

		int* passengers = boat->BoatReady(passengersArray);
		tripNumber++; //increase trip number
		int offset = sprintf(printBuf, "***** Boat load(%d): Passenger list (", tripNumber);
		switch (passengers[3]) {
		case 1:	//one cannibal two missionaries
			sprintf(printBuf + offset, "c%d, m%d, m%d)\n", passengers[0], passengers[1], passengers[2]);
			break;
		case 2:	//all cannibals 
			sprintf(printBuf + offset, "c%d, c%d, c%d)\n", passengers[0], passengers[1], passengers[2]);
			break;
		default://all missionaries
			sprintf(printBuf + offset, "m%d, m%d, m%d)\n", passengers[0], passengers[1], passengers[2]);
			break;
		}
		write(1, printBuf, strlen(printBuf));

		//make it so it takes time to cross
		Delay();
		Delay();

		sprintf(printBuf, "***** Boat load(%d): Completed\n", tripNumber);
		write(1, printBuf, strlen(printBuf));

		riverClosed = boat->BoatDone(maxLoads); //tell the monitor that the boat has crossed

		if (riverClosed == 1) { //checks if river is closed or not
			Exit();
		}
	}
	Exit(); //emergency thread exit
};
