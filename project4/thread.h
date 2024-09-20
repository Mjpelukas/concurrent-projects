
// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 11/08/2023
// PROGRAM ASSIGNMENT #4
// FILE NAME : thread.h
// PROGRAM PURPOSE :
// This is the file that contains all of the class definitions for baby and mother eagle threads
// -----------------------------------------------------

#include "ThreadClass.h"
#include <stdio.h>

extern Semaphore* WaitEat;
extern Semaphore* FindPot;
extern Semaphore* MomSleep;
extern Semaphore* BabyEating;
extern Semaphore* WakerBaby;
extern Semaphore* FirstWaker;
extern Semaphore* MomAwake;
extern Mutex* mu;

extern int potCounter;
extern int feedingCounter;
//extern int babeatval;

class MotherEagle : public Thread
{
public:
    MotherEagle(int p, int f); // p is number of pots, f is number of feedings
    void get_woke(int w);

private:
    void ThreadFunc();
    void goto_sleep(void);
    void food_ready(void);
 
    //void bufferPrint(char*); //redundant to just writing the buffer
    char printBuf[241];

    int maxfeedings;
    int totalPots;
    int wakingBaby;
    int currentFeeding;
   // int* wakingBabyPointer;
 
};

class BabyEagle : public Thread
{
public:
    BabyEagle(int n, int p, int f, MotherEagle * Mother); //baby number, total pots, mother thread


private:
    void ThreadFunc();
    void ready_to_eat(void);
    void finish_eating(void);
    void wake_Mom(void);
    int babyNumber;
    int maxfeedings;
    int currentPot;

    int totalPots;
   // int* wakingBabyPointer;
    char indent;
    MotherEagle * mom;
    char printBuf[241];
};