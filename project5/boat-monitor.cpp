// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 11/29/2023
// PROGRAM ASSIGNMENT #5
// FILE NAME : boat-monitor.cpp
// PROGRAM PURPOSE :
//	This is the file that contains all of the class and funciton implemenaions for the boat monitor
// -----------------------------------------------------

#include "boat-monitor.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>


// -----------------------------------------------------------
// FUNCTION BoatMonitor::BoatMonitor(char* name):
//  Constructor implementation for the BoatMonitor
// PARAMETER USAGE :
//  char* name - the name for the monitor
// FUNCTION CALLED :
//	Monitor(name, HOARE)
//  Condition()
//	srand()
//	time()
// -----------------------------------------------------------
BoatMonitor::BoatMonitor(char* name)
	: Monitor(name, HOARE) {
	tripNumber = 0;
	passengerCount = 0;		//no passengers on boat
	cannibalCount = 0;		//no cannibals on boat
	seats[0] = seats[1] = seats[2] = 0;	//holds ids of passenger in each seats
	misWaiting = 0;			//no passengers waiting
	canWaiting = 0;

	//condition variables
	CannibalWait = new Condition("CannibalWait");	
	MissionaryWait = new Condition("MissionaryWait");

	BoatLoading = new Condition("Loading");   // loading the boat with passengers
	BoatCrossing = new Condition("Crossing"); //Crossing the river

	srand(time(NULL));
		//gets random seed because only one monitor exists
}



// -----------------------------------------------------------
// FUNCTION: BoatMonitor::CannibalArrives(int id) :
//  Allows a Cannibal to get in line to get on the boat 
//	They arraive an enter the monitor and then add to the waiting counter 
//	After it is realeased it will increment the number of passengers and cannibals on the boat
//	then it will take a seats on the boat
//	if it is the third passenger it will signal that boatloading is done and start crosssing river
//	it then waits until boat has crossed the river
//	reduces passenger count by 1, and cannibalCount by 1.
//	after that it gives up the monitor 
// PARAMETER USAGE :
//  int id - is the id of the cannibal that arrives
// FUNCTION CALLED :
//  MonitorBegin()
//	Wait()
//	Signal()
//	MonitorEnd()
// -----------------------------------------------------------

void BoatMonitor::CannibalArrives(int id) {
	MonitorBegin();
	char printBuf[50];
	//starts waiting
	canWaiting++;
	CannibalWait->Wait();			//blocks thread until next boat load is ready
	
	//stops waiting and gets on boat
	canWaiting--;
	cannibalCount++;
	passengerCount++;		

	seats[passengerCount-1] = id * (-1);	//takes their seats

	if (passengerCount == 3) {	//if last passenger signals boat to depart
		BoatLoading->Signal();		
	}

	BoatCrossing->Wait();		//blocks until ride is over to get off
	//gets off boat
	passengerCount--; //no longer on boat
	cannibalCount--;
	MonitorEnd();
}

// -----------------------------------------------------------
// FUNCTION: BoatMonitor::MissionaryArrives(int id) :
//  Allows a Missionary to get in line to get on the boat 
//	They arraive an enter the monitor and then add to the waiting counter 
//	After it is realeased it will increment the number of passengers
//	then it will take a seats on the boat
//	if it is the third passenger it will signal that boatloading is done and start crosssing river
//	it then waits until boat has crossed the river
//	reduces passenger count by one.
//	after that it  gives up the monitor 
// PARAMETER USAGE :
//  int id - is the id of the Missionary that arrives
// FUNCTION CALLED :
//  MonitorBegin()
//	Wait()
//	Signal()
//	MonitorEnd()
// -----------------------------------------------------------

void BoatMonitor::MissionaryArrives(int id) {
	MonitorBegin();
	char printBuf[50];
	misWaiting++;
	MissionaryWait->Wait();			//block until next boat load is ready

	//stops waiting and gets on
	misWaiting--;
	passengerCount++;		

	seats[passengerCount-1] = id;	//let captain know who boarded

	if (passengerCount == 3) {	//if last passenger signals boat to depart
		BoatLoading->Signal();		
	}

	BoatCrossing->Wait();		//blocks until ride is over to get off
	passengerCount--;
	MonitorEnd();		
}


// -----------------------------------------------------------
// FUNCTION: BoatMonitor::BoatReady(int * passengers) :
//  Goes in a loop going in and out of the monitor until all seatss are filled
//	It holds onto the monitor where it waits if the passengers are actually in the boat
//	it then checks the number of passangers and waits if there is less than 3
//	Next it checks the seatss and makes sure a passenger is in the first seats by moving them
//	there in the passenger array they aren't already there
//	Next it prints a statement based on the compostion fo the passengers
//	then it gives up monitor and returns the passenger array
// PARAMETER USAGE :
//  int id - is the id of the cannibal that arrives
// FUNCTION CALLED :
//  FillSeats()
// 
//	abs()
//	MonitorBegin()
//	MonitorEnd()
// -----------------------------------------------------------
int* BoatMonitor::BoatReady(int* passengers) {
	char printBuf[100];
	int filled = 0;
	MonitorBegin();

	//loop until seatss are filled
	do {
		MonitorEnd();
		MonitorBegin();
		filled = FillSeats(); //fills seatss	
	} while (!filled);

	if (passengerCount < 3) {//boat waits if it isnt full
		BoatLoading->Wait();	
	}

	//handle cannibal identifying and put seatss ids in passenger array
	//makes sure that cannibal is always in front of missionaries
	if (seats[0] < 0) {
		for (int i = 0; i < 3; i++) {
			passengers[i] = abs(seats[i]);
		}
	}else if (seats[1] < 0) {
		passengers[0] = abs(seats[1]);
		passengers[1] = abs(seats[0]);
		passengers[2] = abs(seats[2]);
	}
	else { // (seats[2] < 0)
		passengers[0] = abs(seats[2]);
		passengers[1] = abs(seats[0]);
		passengers[2] = abs(seats[1]);
	}

	tripNumber++;	//increase the trip nuber
	//figure out what combination
	switch (cannibalCount) {
	case 1: //one cannibal on the boat
		sprintf(printBuf, "MONITOR(%d): one cannibal (%d) and two missionaries (%d, %d) are selected\n", tripNumber, passengers[0], passengers[1], passengers[2]);
		passengers[3] = 1;
		break;
	case 3: //all cannibals for the boat
		sprintf(printBuf, "MONITOR(%d): three cannibals (%d, %d, %d) are selected\n", tripNumber, passengers[0], passengers[1], passengers[2]);
		passengers[3] = 2;
		break;
	case 0://all missionaries
		sprintf(printBuf, "MONITOR(%d): three missionaries (%d, %d, %d) are selected\n", tripNumber, passengers[0], passengers[1], passengers[2]);
		passengers[3] = 3;
		break;
	}
	write(1, printBuf, strlen(printBuf));

	MonitorEnd();
	return passengers;

}

// --------------------------------------------------------------------- 
// FUNCTION : BoatMonitor::BoatDone()                                   
//	This is called when the boat finishes crossing the river it starts by obtaining the monitor
//	Instead if the trip number is greater than or equal to the cax trips then it prints some information
//	and terminates the whole program, also returns 1 after that
//	if the trip number is less than the max trips 
//	thien it callls alls the BoatCrossing signal() so all  of the passengers can get off the boat
//  by resuming their threads then gives up monitor and returns 0.         
// PARAMETER USAGE :                                                     
//	int maxTrips - the number of rides that can be given before the program terminates                                     
// FUNCTION CALLED :                                                     
//	MonitorBegin()
//	MonitorEnd()                                                         
// ---------------------------------------------------------------------

int BoatMonitor::BoatDone(int maxTrips) {
	MonitorBegin();
	char printBuf[100];

	if (tripNumber >= maxTrips) { // if trip number is ride limit or more then closes river
		int offset;
		offset = sprintf(printBuf, "MONITOR: %d crosses have been made.\n", tripNumber);
		sprintf(printBuf + offset, "MONITOR: This river cross is closed indefinitely for renovation.\n");
		write(1, printBuf, strlen(printBuf));
		//exit the program

		exit(0); //exit the program
		MonitorEnd();
		return 1;
	}
	
	
		// when program doesn't terminate
	for (int i = 0; i < 3; i++) { //releases the threads of all of the passengers
		BoatCrossing->Signal();
	}

	
	MonitorEnd();
	return 0;
}




// -----------------------------------------------------------
// FUNCTION: int BoatMonitor::FillSeats() :
//  picks what types of passangers will make up the boat
//	it can either be all cannibals, all missionaries, or two missionary, one cannibal
//	it takes a random number and then picks a case that represents on of those compostions
//	in each case if there are enough cannibals or missionaries waiting needed to make that compostion 
//	it realeases the appropriate aomunt of them with from their waiting, and then returns 1
//  if there aren't enough, it tries again on the next case, if all three 
//	cases failed then it will return 0
// PARAMETER USAGE :
//  N/A
// FUNCTION CALLED :
//  rand()
//	Signal()
// -----------------------------------------------------------

int BoatMonitor::FillSeats() {

	int lottery = rand() % 3;//random number

	
	if (passengerCount > 2) { //catches this being called while boat is full
		return 1;
	}
	for (int i = 0; i < 3; i++) { //tries all 3 cases in case they didn't work

		lottery = (lottery + 1) % 3; //allows it to cycle to all three

		switch (lottery) {
		case 1:	//all cannibals
			if (canWaiting >= 3) {
				for (int j = 0; j < 3; j++) {
					CannibalWait->Signal();
				}

				return 1;
			}
			break;
			

		case 2:	//2 missionaries and a cannibal
			if ((misWaiting >= 2) && (canWaiting >= 1)) {
				for (int j = 0; j < 2; j++) {
					MissionaryWait->Signal();
				}
				CannibalWait->Signal();

				return 1;
			}
			break;

		default: 
			//all missionaries
			if (misWaiting >= 3) {
				for (int j = 0; j < 3; j++) {
					MissionaryWait->Signal();
				}
				return 1;
			}
			break;
		}
	}

	return 0;

}