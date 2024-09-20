// --------------------------------------------------------
// NAME : Matthew Pelukas					User ID: mjpeluka
// DUE DATE : 11/08/2023
// PROGRAM ASSIGNMENT #4
// FILE NAME : thread.cpp
// PROGRAM PURPOSE :
//  This is the file that contains supporting functions for baby and mother eagle threads
// -----------------------------------------------------
#include "thread.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>



// -----------------------------------------------------------
// FUNCTION MotherEagle::get_woke :
//  Sets wakingBaby for Mom and signals 
// PARAMETER USAGE :
//  none
// FUNCTION CALLED :
//  Signal()
// -----------------------------------------------------------
void MotherEagle::get_woke(int w) {
	wakingBaby = w;
}

// -----------------------------------------------------------
// FUNCTION MotherEagle::goto_sleep :
//  Sets the mom to sleep by using the sempahor MomSleep
// PARAMETER USAGE :
//  none
// FUNCTION CALLED :
//  Wait()
// -----------------------------------------------------------
void MotherEagle::goto_sleep(void) {
	sprintf(printBuf, "Mother eagle takes a nap.\n");
	write(1, printBuf, strlen(printBuf));
	MomAwake->Signal();
	MomSleep->Wait();
}


// -----------------------------------------------------------
// FUNCTION MotherEagle::food_ready :
//	Resets the potCOunter to 0, mother eagle has added food to all feeding pots
//  Allows babys to get food by signaling WaitEat
// PARAMETER USAGE :
//  none
// FUNCTION CALLED :
//  Signal()
//	Lock()
//	Unlock()
// -----------------------------------------------------------
void MotherEagle::food_ready(void) {
	mu->Lock(); //locks mutex
	potCounter = 0; //reset pot counter
	mu->Unlock(); //unlock mutex
	//WaitEat->Signal();
	WakerBaby->Signal(); //allows the waking bird that were ready to eat to go find a pot
}


// -----------------------------------------------------------
// FUNCTION BabyEagle::Wake_Mom :
//  Sets wakingBabyPointer for Mom to the calling baby and signals her to wake up;
// PARAMETER USAGE :
//  none
// FUNCTION CALLED :
//  Signal()
// -----------------------------------------------------------
void BabyEagle::wake_Mom(void) {
	mom->get_woke(babyNumber);
	MomSleep->Signal();
}

// -----------------------------------------------------------
// FUNCTION BabyEagle::finish_eating :
//  Signals that the bay is done eating. and prints
// PARAMETER USAGE :
//  none
// FUNCTION CALLED :
//  Signal()
// -----------------------------------------------------------
void BabyEagle::finish_eating(void) {
	sprintf(printBuf, "%*cBaby eagle %d finishes eating.\n", babyNumber, indent, babyNumber);
	write(1, printBuf, strlen(printBuf));
	//babeatval++;
	BabyEating->Signal();
	
}

// -----------------------------------------------------------
// FUNCTION BabyEagle::ready_to_eat :
//  Blocks the baby if there is no pot for it to eat from. But before that the baby signals to mother eagle that there is no food
//	Only returns once the baby can eat. 
// PARAMETER USAGE :
//  none
// FUNCTION CALLED :
//  Signal()
//	Wait()
//	Lock()
//	Unlock()
//	wake_Mom()
// -----------------------------------------------------------

void BabyEagle::ready_to_eat(void) {
	
	WaitEat->Wait(); // only allow first to enter

	FindPot->Wait(); //is redundant but will help catch errors in testing
	mu->Lock(); //get mutex
	
	if (potCounter < totalPots) { //if not all pots used
		potCounter++;
		currentPot = potCounter; //current pot they are using
		mu->Unlock();
						
		sprintf(printBuf, "%*cBaby eagle %d is eating using feeding pot %d.\n", babyNumber, indent, babyNumber, potCounter);
		write(1, printBuf, strlen(printBuf));
		WaitEat->Signal();
		FindPot->Signal();
		return;
	}
	if (potCounter == totalPots) {
		if (feedingCounter == maxfeedings) { //in the case of last feeding
			WaitEat->Signal();
			FindPot->Signal();
			mu->Unlock();
			Exit();
		}
		mu->Unlock(); //give up mutex
		FirstWaker->Wait(); //semaphore


		mu->Lock(); //gain mutex
		if (potCounter == totalPots) {//if they are still waiting on food
			if (feedingCounter == maxfeedings) {//Exits threads after all feeding is done
				mu->Unlock(); //give up mutex
				WaitEat->Signal();
				FindPot->Signal();
				Exit();
			}
			
			for (int i = 0; i < totalPots; i++) { //waits baby if all others are not done eating
				//babeatval--;
				BabyEating->Wait();
			}
			 
			MomAwake->Wait(); //waits on mom to be napping
			mu->Unlock(); //give up mutex
			sprintf(printBuf, "%*cBaby eagle %d sees all feeding pots are empty and wakes up the mother.\n", babyNumber, indent, babyNumber, potCounter);
			write(1, printBuf, strlen(printBuf));
			wake_Mom(); //wake up mom
			WakerBaby->Wait(); //wait for food to be ready
		}
		//was no longer available to get a pot
		mu->Unlock(); //give up mutex
		FirstWaker->Signal();
		WaitEat->Signal();
		FindPot->Signal();
		ready_to_eat();
	}
	return;
}
	
	/*sprintf(printBuf, "%*cBaby eagle %d is eating using feeding pot %d.\n", babyNumber - 1, indent, babyNumber, potCounter);
	write(1, printBuf, strlen(printBuf));*/




