// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 11/08/2023
// PROGRAM ASSIGNMENT #4
// FILE NAME : thread-main.cpp
// PROGRAM PURPOSE :
//  This is the file runs the main program of the hungry eagles
//  After receiveing the amount of feeding pots, hungry eagles and feedings
//  Baby Eagles and Mother Eagle will start their threads 
//  Mother eagle will fill the food pots for argv[3] feedings
//  After that many feedings once all the food pots are empty the Mother Eagle retires and the game ends
// -----------------------------------------------------

#pragma once

#include "thread.h"
#include <stdlib.h>
#include <string.h>

Semaphore* WaitEat;
Semaphore* FindPot;
Semaphore* MomSleep;
Semaphore* BabyEating;
Semaphore* WakerBaby;
Semaphore* FirstWaker;
Semaphore* MomAwake;
Mutex * mu;

int potCounter;
int feedingCounter;
//int babeatval;

// -----------------------------------------------------------
// FUNCTION main :
//  Initializes variables for the main driver code
//  Initializes semaphores for the threads
//  Initializes MotherEagle thread.   
//  Initializes a number of BabyEagle threads based on argv[2]
//  Drives the game through to completeion and returns after that
// PARAMETER USAGE :
//  argv[1] - The number of feeding pots
//  argv[2] - The number of Baby Eagles
//  argv[3] - The number of feedings
// FUNCTION CALLED :
//  MotherEagle()
//  BabyEagle()
// -----------------------------------------------------------
int main(int argc, char* argv[])
{
    char printBuf[241];
    //Setting default argement values  be 10
    int feedingPots = 10;
    int numberOfBabys = 10;
    int maxFeedings = 10;
    
    //set variables to arguements if not zero
    if(atoi(argv[1]) != 0){
        feedingPots = atoi(argv[1]);
    }
    if (atoi(argv[2]) != 0){
        numberOfBabys = atoi(argv[2]);
    }
    if (atoi(argv[3]) != 0){
        maxFeedings = atoi(argv[3]);
    }
   
    sprintf(printBuf, "MAIN: There are %d baby eagles, %d feeding pots, and %d feedings.\n",numberOfBabys, feedingPots, maxFeedings);
    write(1, printBuf, strlen(printBuf));
     
    //initialize values
    WaitEat = new Semaphore("", 1);
    FindPot = new Semaphore("", 1);
    MomSleep = new Semaphore("", 0);
    WakerBaby = new Semaphore("", 0);
    BabyEating = new Semaphore("", feedingPots);
    FirstWaker = new Semaphore("", 1);
    MomAwake = new Semaphore("", 0);
    mu = new Mutex("mutexname");

    potCounter = feedingPots;
    feedingCounter = 0;
    //babeatval = feedingPots;

  /*/  //int and pointer to get the baby that woke up mom;
    int wakingBabyNumber = 0; //initialized to zero to catch errors
    int* wakingBabyPointer = &wakingBabyNumber;*/
   
    sprintf(printBuf, "MAIN: Game starts!!!!!\n");
    write(1, printBuf, strlen(printBuf));

    //inmitiallitze threads
    MotherEagle* MotherEagleThread;
    MotherEagleThread = new MotherEagle(feedingPots, maxFeedings);
    MotherEagleThread->Begin();

    BabyEagle* BabyEagleThread[numberOfBabys];
    for (int i = 0; i < numberOfBabys; i++) {
        BabyEagleThread[i] = new BabyEagle(i + 1, feedingPots, maxFeedings, MotherEagleThread); //initalize to number + 1
        BabyEagleThread[i]->Begin();
    }


    //wait and end program
    MotherEagleThread->Join();
    for (int i = 0; i < numberOfBabys; i++)
    {
        BabyEagleThread[i]->Join();
    }

    return 0;
}