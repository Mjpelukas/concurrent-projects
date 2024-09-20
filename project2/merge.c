/* ----------------------------------------------------------- */
/* NAME : Matthew Pelukas ID: mjpeluka */
/* DUE DATE : 10/04/2023 */
/* PROGRAM ASSIGNMENT #2 */
/* FILE NAME : merge.c */
/* PROGRAM PURPOSE : the purpose is to perform a merge algorithm.*/
/* This file recieves command line arguements of keys for three shared memory arrays x[], y[], and z[], and their lengths* /
/* Then, it creates m(lenght of x)+n(length of y) child processes, each of which is assigned to an element of x[ ] or an element of y [ ]. */
/* Each child process carries out a modified binary search as discussed earlier to determine the final location of the assigned element in the sorted array z[]. */
/* When the final location is found, the process writes its assigned value into that location of the output array z[] */
/*this pprogram then waits until all of its child processes exit, and terminates itself.*/
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
/* FUNCTION binarySearch : binarySearch */
/*  finds the index where the given element should go in the output array in shared memory*/
/* PARAMETER USAGE : */
/*  int* arr -is the array to be searched through  */
/*  int l -is the left bound index of the array  */
/*  int r -is the left bound index of the array  */
/*  int element -value that is searching against the array  */
/* FUNCTION CALLED : */
/*  binarySearch(int* arr, int l, int r, int element) */

/* ----------------------------------------------------------- */
int binarySearch(int* arr, int l, int r, int element){
   // char printBuf[PRINTBUFSIZE]; //not used
    if (l <= r) { //should maintain this while searching
        int mid = l + (r - l) / 2; // gets an index between left and right

        // If element is smaller than mid, then search subarray left of mid
        if (arr[mid] > element) {
            return binarySearch(arr, l, mid - 1, element);
        }
        // If element is larger than mid, then search subarray right of mid
        if (arr[mid] < element) {
            return binarySearch(arr, mid + 1, r, element);
        }
    }
    //binary search should have a result
    else { //effectively if (l > r)
        //checks if correct location was found works for both x[] and y[]  
        if (r < 0) { return 0; } //element is smaller than arr[0] scenario (doesn't matter if x or y element)
        if (element > arr[r]) { return r + 1; } // x[i] is larger than y[n-1] && x[i] is between y[k-1] and y[k] scenarios (doesn't matter if x or y element)

    }
    //catches failure code should never get here
    printf("!!!!!BINARY SEARCH FAILURE l:%d  r:%d  element:%d",l,r,element); //uses print f because there's an error to report
    exit(1);
}

/* ----------------------------------------------------------- */
/* FUNCTION main : main */
/*  creates many child processes that sort x[] and y[] from shared memory sector into the output z[] in shared memory */
/*  creates a child process for each element of x[] and y[] and performs binary search to find where they go and writes to z in shared memory*/
/*  writes in shared memory completely sorted for the element assigned*/ 
/*  parent waits until all things are sorted and all children are terminated and then exits*/
/* PARAMETER USAGE : */
/*  argc and argv are parsed to get the passed parameters  */
/*  argv[1] - the x length */
/*  argv[2] - the y length */
/* FUNCTION CALLED : */
/*  binarySearch(int* arr, int l, int r, int element) */
/* ----------------------------------------------------------- */
int main(int argc, char* argv[]) {
    char printBuf[PRINTBUFSIZE];
    int cpid = 0;
    int status;
    int elementType = 0;
    int index = -1;

    key_t xkey = ftok("./", 'x');
    key_t ykey = ftok("./", 'y');
    key_t zkey = ftok("./", 'z');
    int m, n, zlen; // lengths of a[],x[], y[], and z[] respectively
    int* x;
    int* y;
    int* z; // pointer for the arrays
    
    //assign lenghths
    m = atoi(argv[1]); //x[] length
    n = atoi(argv[2]); //y[] length
    zlen = m + n; //z[] length

    //attach to memory segments 
    int xmid = shmget(xkey, m * sizeof(int), 0666);
    x = (int*)shmat(xmid, NULL, 0);
    int ymid = shmget(ykey, n * sizeof(int), 0666);
    y = (int*)shmat(ymid, NULL, 0);
    int zmid = shmget(zkey, zlen * sizeof(int), 0666);
    z = (int*)shmat(zmid, NULL, 0);

    int procid;
    cpid = 1;
    for (int i = 0; i < m; i++) { // create children that have x[] elements
        elementType = 1; // element of x[]
        cpid = fork();
        if (cpid == 0) {
            procid = getpid();
            index = i;
            snprintf(printBuf, PRINTBUFSIZE, "      $$$ M-PROC(%d): handling x[%d] = %d\n", procid, index, x[index]); //output
            write(1, printBuf, strlen(printBuf)); //prints the output)
            break; //breaks out of for loop if not the parent
        }

    }
    if (cpid != 0) { //only the original parent can enter.
        elementType = 2; //element of y[]
        index = -1;
        for (int i = 0; i < n; i++) {//create children that have y[] elements
            cpid = fork();
            if (cpid == 0) {
                procid = getpid();
                index = i;
                snprintf(printBuf, PRINTBUFSIZE, "      $$$ M-PROC(%d): handling y[%d] = %d\n", procid, index, y[index]); //output
                write(1, printBuf, strlen(printBuf)); //prints the output)
                break; // breaks out of for loop if not the parent
            }
            
        }
    }
    if (cpid == 0){ //children
        int searchIndex;
        int placement;
        if (elementType == 1) { //for x[] elements
            searchIndex = binarySearch(y, 0, n - 1, x[index]);
            placement = index + searchIndex;
            //Print where it is based on search
            if (searchIndex == 0) { //smaller than all
                snprintf(printBuf, PRINTBUFSIZE, "      $$$ M-PROC(%d): x[%d] = %d is found to be smaller than y[0] = %d\n",
                    procid, index, x[index], y[0]); //output
            }
            else if (searchIndex > n-1){ //larger than all
                snprintf(printBuf, PRINTBUFSIZE, "      $$$ M-PROC(%d): x[%d] = %d is found to be larger than y[%d] = %d\n",
                    procid, index, x[index], n-1, y[n-1]); //output
            }
            else {
                snprintf(printBuf, PRINTBUFSIZE, "      $$$ M-PROC(%d): x[%d] = %d is found between y[%d] = %d and y[%d] = %d \n",
                    procid, index, x[index], searchIndex - 1, y[searchIndex - 1], searchIndex + 1, y[searchIndex + 1]); //output
            }
            write(1, printBuf, strlen(printBuf)); //prints the output)
            
            snprintf(printBuf, PRINTBUFSIZE, "      $$$ M-PROC(%d): about to write x[%d] = %d into position %d of the output array\n", procid, index, x[index], placement); //output
            write(1, printBuf, strlen(printBuf)); //prints the output)
            z[index + searchIndex] = x[index]; //
        }
        if (elementType == 2) { //for y[] elements
            searchIndex = binarySearch(x, 0, m - 1, y[index]);
            placement = index + searchIndex;
            
            //Print where it is based on search
            if (searchIndex == 0) { //smaller than all
                snprintf(printBuf, PRINTBUFSIZE, "      $$$ M-PROC(%d): y[%d] = %d is found to be smaller than x[0] = %d\n",
                    procid, index, y[index], x[0]); //output
            }
            else if (searchIndex > m - 1) { //larger than all
                snprintf(printBuf, PRINTBUFSIZE, "      $$$ M-PROC(%d): y[%d] = %d is found to be larger than x[%d] = %d\n",
                    procid, index, y[index], m - 1, y[m - 1]); //output
            }
            else {
                snprintf(printBuf, PRINTBUFSIZE, "      $$$ M-PROC(%d): y[%d] = %d is found between x[%d] = %d and x[%d] = %d \n",
                    procid, index, y[index], searchIndex - 1, x[searchIndex - 1], searchIndex + 1, x[searchIndex + 1]); //output
            }
            write(1, printBuf, strlen(printBuf)); //prints the output)

            snprintf(printBuf, PRINTBUFSIZE, "      $$$ M-PROC(%d): about to write y[%d] = %d into position %d of the output array\n", procid, index, y[index], placement); //output
            write(1, printBuf, strlen(printBuf)); //prints the output
            z[placement] = y[index];
        }
        exit(0); //exits child
    }
    for (int i = 0; i < zlen; i++){
        wait(&status);
    }
    
    exit(0);
}