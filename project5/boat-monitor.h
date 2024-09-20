// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 11/29/2023
// PROGRAM ASSIGNMENT #5
// FILE NAME : boat-monitor.h
// PROGRAM PURPOSE :
//	This is the file that contains all of the class definitions for the boat monitor
// -----------------------------------------------------


#include "ThreadClass.h"

class BoatMonitor : public Monitor {
public:
	BoatMonitor(char* name);
	void CannibalArrives(int id);
	void MissionaryArrives(int id);
	int* BoatReady(int* passengers);
	int BoatDone(int rideLimit);


private:
	int FillSeats(); //funciton to fill the seats
	Condition* CannibalWait;	//blocks cannibals
	Condition* MissionaryWait;	//blocks cannibals
	Condition* BoatLoading;		//blocks boat to load passengers
	Condition* BoatCrossing;
	int tripNumber;			//the number of trips being taken
	int passengerCount;		//number of passengers on boat
	int cannibalCount;		//number of cannibals on boat
	int canWaiting;			//number of cannibals waiting waiting
	int misWaiting;			//number of cannibals waiting waiting
	int seats[3];			//id storage for passengers

};



