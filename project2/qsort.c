/* ----------------------------------------------------------- */
/* NAME : Matthew Pelukas ID: mjpeluka */
/* DUE DATE : 10/04/2023 */
/* PROGRAM ASSIGNMENT #2 */
/* FILE NAME : qsort.c */
/* PROGRAM PURPOSE : the purpose is to perform a quicksort algorithm. This file recieves the left and right indicies as well as the key for shared memory*/
/* Then, the file partitions the array section a[Left..Right] into two at element a[M]. */
/*  The first child receives Left and M-1, whilethe second receives M+1 and Right */
/*The parent then waits until both child processes complete their job and then the program exits */
/* ----------------------------------------------------------- */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUF 20
#define PRINTBUFSIZE 241

/* ----------------------------------------------------------- */
/* FUNCTION printPartition : printPartition */
/*  prints the contents of the given array in the defined partition */
/* PARAMETER USAGE : */
/*  int* arr is the pointer to the array that will be printed */
/*  int left is the left index of the array being printed */
/*  int right is the right index of the array being printed */
/*  int procid is the process number */
/*  int mestype is if it prints message 1 or 2
/* FUNCTION CALLED : */
/* none */
/* ----------------------------------------------------------- */
void printParition(int* arr, int left, int right, int procid, int mestype) {
    char printBuf[PRINTBUFSIZE];     //-- Output buffer 
    int bufOffset=0;
    char errbuf[PRINTBUFSIZE];
    //initial part of the print
    if (mestype == 1) {
        bufOffset = snprintf(printBuf, PRINTBUFSIZE, "   ### Q-PROC(%d): entering with a[%d .. %d]\n          ", procid, left, right);
    }if (mestype == 2) {
    bufOffset = snprintf(printBuf, PRINTBUFSIZE, "   ### Q-PROC(%d): section a[%d .. %d] sorted\n          ", procid, left, right);
    }
 
    //initial part of the print
    int loopOffset = 0;
    for (int i = left; i <= right; i++) {
        loopOffset = snprintf(printBuf + bufOffset, PRINTBUFSIZE, "%d ", arr[i]);
        bufOffset = bufOffset + loopOffset;
    }
    sprintf(printBuf + strlen(printBuf), "\n");
    write(1, printBuf, strlen(printBuf));
}

/* ----------------------------------------------------------- */
/* FUNCTION swap : swap */
/*  swaps the given parameters in the shared memory */
/* PARAMETER USAGE : */
/*  int* sw1 is the first to be swapped  */
/*  int* sw2 is the first to be swapped  */
/* FUNCTION CALLED : */
/*  none */
/* ----------------------------------------------------------- */
void swap(int* sw1, int* sw2) {
    int temp = *sw1;
    *sw1 = *sw2;
    *sw2 = temp;
}
/* ----------------------------------------------------------- */
/* FUNCTION spawner : spawner */
/*  uses execvp to create child functions */
/* PARAMETER USAGE : */
/*  int* sw1 is the first to be swapped  */
/*  int* sw2 is the first to be swapped  */
/* FUNCTION CALLED : */
/*  execvp() */
/* ----------------------------------------------------------- */
void spawner(int* arr, int left, int right, int amid) {
    int status;
    char printBuf[PRINTBUFSIZE];
    char argBuf[3][BUF];
    char* qsortArgs[5] = { "qsort", "","","",""}; //qsort, Left, Right, k,
    snprintf(argBuf[0], BUF, "%d", left); //writes the initial right value
    qsortArgs[1] = argBuf[0];

    snprintf(argBuf[1], BUF, "%d", right); //writes the initial right value
    qsortArgs[2] = argBuf[1];
    snprintf(argBuf[2], BUF, "%d", amid); //writes length of k down to get shared memory
    qsortArgs[3] = argBuf[2];
    qsortArgs[4] = NULL;
   // if (cpid == 0) { //fork() = 0
     /*   sprintf(printBuf, "   ### Q-PROC(%d): entering with a[%d..%d]\n       ", getpid(), left, right);
        for (int i = left; i <= right; i++) {
            sprintf(printBuf + strlen(printBuf), "%d  ", arr[i]);
        }
        sprintf(printBuf + strlen(printBuf), "\n");
        write(1, printBuf, strlen(printBuf));*/
      //sorter(arr, left, right, amid);
     
        if (execvp("./qsort", qsortArgs) < 0) { //file plus args starts new process
            printf("!!!!!!!!!!!!!!!!!!!!!QSORT(%d) FAILED\n", getpid()); //printf because it's clearly broken so why bother
            exit(1);
        }
  //  }
    return;
}
/* ----------------------------------------------------------- */
/* FUNCTION sorter : sorter */
/*  takes a pivot at the left most index of the partition and sorts through */
/*  it goes until everything left of the pivot is smaller than the pivot and everything to the right is larger   */
/*  two numbers that start from the left and right respectively, with l and r.  */
/*  l goes right in the array until it finds an element larger than thepivot  */
/*  then r goes left from end of the array until it finds a number  */
/*  smaller than the pivot or it reaches the left index.  */
/*  If the value at r is smaller than that at l the two are swapped.  */
/*  And this continues until l is no longer smaller than r  */
/*  Then it swaps the pivot with the final r and returns  */
/*  the value of rlonger smaller than r.  */
/* PARAMETER USAGE : */
/*  int* arr is the array  */
/*  int left is the left most index of the partition  */
/*  int right is the right most index of the partition  */
/* FUNCTIONS CALLED :  */
/*  swap(int* sw1, int* sw2)  */
/*  spawner(int* arr, left, right, k) */
/* ----------------------------------------------------------- */
int sorter(int* arr, int left, int right, int amid) {
    int pivot = arr[left];
    int procid = getpid();
    int l = left;
    int r = right;
    int numchildren = 0;
    while (l < r) {
        //move l right until element at l is less than pivot or reaches 1 before the right
        while ((arr[l] <= pivot) && (l <= right - 1)) {
            l++;
        }

        //move r left until element at l is greater than pivot or reaches 1 before the right
        while ((arr[r] > pivot) && (r >= left + 1)) {
            r--;
        }
        if (l < r) { //only swap if they haven't crossed
            swap(&arr[l], &arr[r]); //swap the values
        }
    }
    //swaps pivot either with itself or highest indexed number less than it 
    swap(&arr[left], &arr[r]); 
   printParition(arr, left, right, procid, 2); //says it enters and prints array
    //spawn children
  
    if (r > left) {
        numchildren++;
        if (fork() == 0) { spawner(arr, left, r - 1, amid); }
    }
    if (r  < right ) {
        numchildren++;
        if (fork() == 0) { spawner(arr, r + 1, right, amid); }
    }
    while (wait(NULL) > 0);
    return numchildren;;
}

/* ----------------------------------------------------------- */
/* FUNCTION main : main */
/*  seperates arrays for quicksort and spawns two more child qsort processes to do the same */
/*  takes a pivot in the range of the left and right, and then keeps swapping numbers until the pivot is in the correct spot* /
/*  pivot is in right spot when all numbers left are smaller and all numbers right are larger to shared memory in the correct location*/
/*  It then creates children, one if the range left of the pivot exists and one if the range right of the pivot exists */
/*  the parent waits until all things are sorted and all children are terminated and then exits and this cascades until the*/
/*  very first parent terminates*/
/* PARAMETER USAGE : */
/*  argc and argv are parsed to get the passed parameters that are then assigned to variables in the function*/
/*  argc is just the number of arguements*/
/*  argv contains: qsort, Left, Right, k*/
/*  argv[1] - Left is the minimum index of subarray  */
/*  argv[2] - Right is the maximum index of the subarray */
/*  argv[3] - k is the legnth of the subarray being sorted*/
/* FUNCTIONS CALLED : */

/*  sorter(int* arr, int l, int r)      */
/*  printParition(int* arr, int left, int right, int procid) */
/* ----------------------------------------------------------- */
int main(int argc, char* argv[]) {
    char printBuf[PRINTBUFSIZE];     //-- Output buffer 
    int* a; //array for the shared memory sector to attatch
    int amid; //future id of memory
    //variables to be filled in from argv[]

    int k, left, right; //length of the array and then the index of the left and right bound of the array
    //gets args
    left = atoi(argv[1]);
    right = atoi(argv[2]);
    //k = atoi(argv[3]);
    amid = atoi(argv[3]);
    int procid = getpid(); //get process id

    //shared memory stuff
   // key_t akey = ftok("./", 'a'); //key for the shmget()
    //amid = shmget(akey, k * sizeof(int), 0666);
    shmdt(a);
    a = (int*)shmat(amid, NULL, 0);

    //snprintf(printBuf, PRINTBUFSIZE, "error check: %d , %d, %d, length: %d\n ", left, right, procid, k);
    //write(1, printBuf, strlen(printBuf));
   printParition( a , left, right,procid, 1); //says it enters and prints array

    //when it is only one element it prints stuff and exits immediately 
    if (left >= right) {
        snprintf(printBuf, PRINTBUFSIZE, "   ### Q-PROC(%d): section a[%d..%d] sorted\n", procid, left, right);
        write(1, printBuf, strlen(printBuf));
        snprintf(printBuf, PRINTBUFSIZE, "   ### Q-PROC(%d): exits\n", procid);
        write(1, printBuf, strlen(printBuf));
        exit(0);
    }
   

    snprintf(printBuf, PRINTBUFSIZE, "   ### Q-PROC(%d): pivot element is a[%d] = %d\n", procid, left, a[left]);
    write(1, printBuf, strlen(printBuf));
    //int pivotIndex = sorter(a, left, right, k); // returns the final pivot index
    
    int numChildren = sorter(a, left, right, amid); 
    int status;
    

    ///!!!!!!!!!!!!!!!!!!!!!! UNUSED!!!!!!!!!!!!!!!!!!!!!!!!!
    /*int numChildren = 0;
    int fakepid = 1;
    int cpid = 1;
    if (pivotIndex >= left){ //only create left child if have elements to the left of pivot
        numChildren++;
        cpid = fork(); //creates left child
    }
    if (cpid != 0 ){ //only parent
        if (pivotIndex <= right) { //only create right child if have elements to the right of pivot
            numChildren++;
            fakepid = 2; // designates right side of pivot
            cpid = fork(); //creates child 2
        }
    }
   
     //section for children
    if (cpid == 0){
        snprintf(printBuf, PRINTBUFSIZE, " ###########Q-PROC(%d): numchildren: %d, CPID = %d , fakepid = %d \n", procid, numChildren, cpid, fakepid);
        write(1, printBuf, strlen(printBuf));
        if (fakepid == 1) {
            char leftargBuf[3][BUF];
            char* leftqsortArgs[5] = {"./qsort", "","","","\0"}; //qsort, Left, Right, k,
            //snprintf(leftargBuf[0], BUF, "%d", left); //writes the initial right value
            leftqsortArgs[1] = argv[1];

            snprintf(leftargBuf[1], BUF, "%d", (pivotIndex - 1)); //writes the initial right value
            leftqsortArgs[2] = leftargBuf[1];
            //snprintf(leftargBuf[2], BUF, "%d", k); //writes length of k down because future calls will not have opportunity to do k+1
            leftqsortArgs[3] = argv[3];
            if (execvp("qsort", leftqsortArgs) < 0) { //file plus args starts new process
                printf("!!!!!!!!!!!!!!!!!!!!!QSORT LEFT FAILED\n"); //printf because it's clearly broken so why bother
                exit(1);
            }
        }else{ //child 2 process fakepid must be 2
            char* rightqsortArgs[5] = { "./qsort", "","","","\0"}; //qsort, Left, Right, k,
            char rightargBuf[3][BUF];
            snprintf(rightargBuf[0], BUF, "%d", (pivotIndex + 1)); //writes the initial right value
            rightqsortArgs[1] = rightargBuf[0];
            snprintf(rightargBuf[1], BUF, "%d", right); //writes the initial right value
            rightqsortArgs[2] = rightargBuf[1];
            //snprintf(rightargBuf[3], BUF, "%d", k); //writes length of k down because future calls will not have opportunity to do k+1
            rightqsortArgs[3] = argv[3];//rightargBuf[2];
            int qsortstatus = execvp("./qsort", rightqsortArgs); //file plus args starts new process
                if (qsortstatus == -1) {
                    printf("!!!!!!!!!!!!!!!!!!!!!QSORT RIGHT FAILED\n"); //printf because it's clearly broken so why bother
                    exit(1);
                }
        }*/
    //Section for the original process
   // }else{
   //!!!!!!!!!!!!!!!!!!!!!!! UNUSED ENDS!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
        //Waits for children to complete
        for (int i = 0; i < numChildren; i++) {
            wait(&status);
        }
   // }
    shmdt(a);
    //prints that it is about to exit and exits
    snprintf(printBuf, PRINTBUFSIZE, "   ### Q-PROC(%d): exits\n", procid);
    write(1, printBuf, strlen(printBuf));
    return 0;
}