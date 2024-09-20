// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 11/29/2023
// PROGRAM ASSIGNMENT #5
// FILE NAME : thread-main.cpp
// PROGRAM PURPOSE :
//  This is the file runs the main program of the River Crossing problem
//  After receiveing the amount of cannibals, missionaries and boat loads by 
//  Cannibals and Missionaries will start their threads based on the argv values 
//  Boat takes 3 people across the river at a time and nobody else can be taken until boat crosses
//  after a given number of crosses the program terminates
// -----------------------------------------------------
#pragma once
#include "thread.h"
#include "boat-monitor.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
// -----------------------------------------------------------
// FUNCTION main :
//  Initializes variables for the main driver code
//  Initializes the boat threads
//  Initializes the cannibals and missionaries threads based on argv[1] and argv[2] respectively
//  Drives the game through to completeion and returns after that by waiting until boatTj joins then exits
// PARAMETER USAGE :
//  argv[1] - The number of cannibals
//  argv[2] - The number of missionaries
//  argv[3] - The number of boat loads
// FUNCTION CALLED :
//  Missionary()
//  Cannibal()
//  BoatThread()
//  Begin()
//	Join()
// -----------------------------------------------------------

int main(int argc, char* argv[])
{
    //Setting default argement values
    int numCans = 8;
    int numMis = 8;
    int safeLoads = 5;
    //set variables to arguements if not zero
    if (atoi(argv[1]) != 0) {
        numCans = atoi(argv[1]);
    }
    if (atoi(argv[2]) != 0) {
        numMis = atoi(argv[2]);
    }
    if (atoi(argv[3]) != 0) {
        safeLoads = atoi(argv[3]);
    }

    
    
    
    
    //start all the threads

    BoatThread* boatT;
    boatT = new BoatThread(safeLoads);		//start boat
    boatT->Begin();

    //start Cannibals
    Cannibal* cannibalT[numCans]; //initialize
    for (int i = 1; i <= numCans; i++) {
        cannibalT[i] = new Cannibal(i);
        cannibalT[i]->Begin();
    }

    //start missionaries
    Missionary* missionaryT[numMis]; //initialize
    for (int i = 1; i <= numMis; i++) {
        missionaryT[i] = new Missionary(i);
        missionaryT[i]-> Begin();
    }

    boatT->Join();			//we only care about boat's exit

    return 0;
};
