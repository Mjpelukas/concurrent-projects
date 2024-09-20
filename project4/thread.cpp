// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 11/08/2023
// PROGRAM ASSIGNMENT #4
// FILE NAME : thread.cpp
// PROGRAM PURPOSE :
//  This is the file that contains the class implementations for baby and mother eagle threads
//  It also contains the implementaion of Threadfunc
// -----------------------------------------------------

#include "thread.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// -----------------------------------------------------------
// FUNCTION MotherEagle::MotherEagle :
//  Constructor implementation for the MotherEagle class
// PARAMETER USAGE :
//  p - total number of pots
//  f - number of feedings
//  w - pointer to waking baby
// FUNCTION CALLED :
//  none
// -----------------------------------------------------------
MotherEagle::MotherEagle(int p, int f)
    :totalPots(p), maxfeedings(f) /*  wakingBabyPointer(w),*/
{
    currentFeeding = 0;
    wakingBaby = 0;
}

// -----------------------------------------------------------
// FUNCTION BabyEagle::BabyEagle :
//  Constructor implementation for the BabyEagle class
// PARAMETER USAGE :
//  n - the identifying number of the baby eagle
//  p - total number of pots
//  f - number of feedings
//  w - pointer to waking baby
//  Mother - is the mother thread. 
// FUNCTION CALLED :
//  none
// -----------------------------------------------------------
BabyEagle::BabyEagle(int n, int p, int f, MotherEagle* Mother)
    :babyNumber(n), totalPots(p), maxfeedings(f), mom(Mother)
{   
    indent = ' ';
    srand(time(NULL));
}


// -----------------------------------------------------------
// FUNCTION MotherEagle::ThreadFunc :
//  Function called by Begin() and is the driver code of the thread
//  of the mother eagle
// PARAMETER USAGE :
//    N/A
// FUNCTION CALLED :
//    Delay()
//    goto_sleep()
//    food_ready()
// -----------------------------------------------------------
void MotherEagle::ThreadFunc() {
    //char printBuf[241];
    sprintf(printBuf, "Mother eagle started.\n");
    write(1, printBuf, strlen(printBuf));

    int randel = rand() % 5; //random for momdelay
    while (currentFeeding != maxfeedings) {
        goto_sleep();
        randel = rand() % 5;
        for (int delno = 0; delno < randel; delno++) { Delay(); } //delays
        currentFeeding++;
        feedingCounter = currentFeeding;
        sprintf(printBuf, "Mother eagle says \"Feeding (%d)\"\n", currentFeeding);
        write(1, printBuf, strlen(printBuf));


        food_ready(); //gets all of the pots ready
        randel = rand() % 5;
        for (int delno = 0; delno < randel; delno++) { Delay(); } //delays
    }
    
    for (int i = 0; i < totalPots; i++) { //waits mom if all babys are not done eating
        BabyEating->Wait();
    }

    sprintf(printBuf, "Mother eagle retires after serving %d feedings. Game is over!!!\n", maxfeedings);
    write(1, printBuf, strlen(printBuf));
    

    Exit();//end thread
}




// -----------------------------------------------------------
// FUNCTION MotherEagle::ThreadFunc :
//  Function called by Begin() and is the driver code of the thread
//  of the Baby Eagle
// PARAMETER USAGE :
//    N/A
// FUNCTION CALLED :
//    Delay()
//    ready_to_eat()
//    finish_eating() 
//    wakeMom()
// -----------------------------------------------------------
void BabyEagle::ThreadFunc(){
    //char printBuf[241];
    sprintf(printBuf, "%*cBaby eagle %d started.\n", babyNumber, indent, babyNumber);
    write(1, printBuf, strlen(printBuf));

    int randel = rand() % 5; //random for baby
    
    while (1) {
        randel = rand() % 5;
        for (int delno = 0; delno < randel; delno++) { Delay(); } //delays
        sprintf(printBuf, "%*cBaby eagle %d ready to eat.\n", babyNumber, indent, babyNumber);
        write(1, printBuf, strlen(printBuf));

        ready_to_eat();//gets ready to eat

        randel = rand() % 5;
        for (int delno = 0; delno < randel; delno++) { Delay(); } //delay for eating time
        finish_eating(); //done eating

      // mu->Lock();
        if ((potCounter == totalPots) && (feedingCounter == maxfeedings)) {
         //   mu->Unlock();
            Exit();
        }
    //    mu->Unlock();
    }

    Exit();//end thread mainly just a catch
}