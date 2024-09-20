/* ----------------------------------------------------------- */
/* NAME : Matthew Pelukas ID: mjpeluka */
/* DUE DATE : 10/04/2023 */
/* PROGRAM ASSIGNMENT #2 */
/* FILE NAME : main.c */
/* PROGRAM PURPOSE : the purpose is help to perform quicksort and merge algorithms, it reads in and prints out three arrays.
/* this file then creates a child process to run the program qsort.c using execvp() system call*/
/* and passes the assigned Left, Right and other needed information to the program qsort.c, Initally , Left and Right are 0 and k-1 respectively */
/* It then reates a second child process to run program merge.c on x[] and y[] using the execvp() system call and passes the needed command line arguments to merge.c. */
/* Finally it waits for both child processes to complete, prints out the results, and terminates itself
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
/* FUNCTION makeArray : makeArray */
/*  allocates shared memory segment and writes given array it */
/*  returns the memory segment id with ShmID and also sends the length through another pointer*/
/* PARAMETER USAGE : */
/*  int* arr is the pointer to the array that will be filled in*/
/*  int* ptrlength is a pointer that captures the length of the array and gets it to main */
/*  key_t key is the key that is used to allocate the shared memory segment */
/* FUNCTION CALLED : */
/*  none */
/* ----------------------------------------------------------- */
int makeArray(int* arr, int *ptrlength, key_t key){
    int offset, length, ShmID;
    scanf("%d", &length);
    *ptrlength = length; //passes out the value for length
    ShmID = shmget(key, length * sizeof(int),IPC_CREAT | 0666);
    arr = (int *) shmat(ShmID, NULL, 0);
    if ( arr < 0) { printf("shmat() failed\n"); exit(1);}
    //arr = (int *) malloc(length * sizeof(int)); no longer using this have to do shared memory
    for(offset = 0; offset < length; offset++){
        scanf("%d", arr + offset);
    }
    shmdt(arr);
    return ShmID;
}

/* ----------------------------------------------------------- */
/* FUNCTION printArray : printArray */
/*  prints the contents of the given array */
/* PARAMETER USAGE : */
/*  int* arr is the pointer to the array that will be printed */
/*  int length of the array being printed */
/* FUNCTION CALLED : */
/* none */
/* ----------------------------------------------------------- */
void printArray(int* arr, int length) {
    printf("      ");
    for (int i = 0; i < length; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n\n");
}
/* ----------------------------------------------------------- */
/* FUNCTION main : main */
/*  the driver of the program, reads in the arrays, allocates the memory, prints the arrays*/
/*  then makes the processes for merge and qsort using execvp() */
/*  finally it prints the results after all children are terminated*/
/* PARAMETER USAGE : */
/*  No Parameters  */
/* FUNCTION CALLED : */
/*  printArray(int *arr, int length)                 */
/*   makeArray(int *arr, int *ptrlength, key_t key)         */    
/* ----------------------------------------------------------- */

int main(void) {
    char printBuf[PRINTBUFSIZE];     //-- Output buffer  
    char errors[PRINTBUFSIZE];     //-- error priting buffer
    int k, m, n, zlen; // lengths of a[],x[], y[], and z[] respectively
    int *a;
    int *x;
    int *y;
    int *z; // pointer for the arrays
    int amid,xmid, ymid, zmid; //memory id to clear the the shared memory later
    //all the keys
    key_t akey = ftok("./", 'a');
    key_t xkey = ftok("./", 'x');
    key_t ykey = ftok("./", 'y');
    key_t zkey = ftok("./", 'z');
    //Makes the arrays and prints success messages
    amid = makeArray(a,&k, akey); //reads length and stores values for a[]
    printf("*** MAIN: qsort shared memory key = %d\n", akey);
    printf("*** MAIN: qsort shared memory created\n");
    a = (int*)shmat(amid, NULL, 0);
    printf("*** MAIN: qsort shared memory attached and ready to use\n\n");
    xmid = makeArray(x,&m, xkey);    //reads length and stores values for x[] prints success stories.
    printf("*** MAIN: merge x[] shared memory key = %d\n", xkey);
    printf("*** MAIN: merge x[] shared memory created\n");
    x = (int*)shmat(xmid, NULL, 0);
    printf("*** MAIN: merge x[] shared memory attached and ready to use\n\n");
    ymid = makeArray(y,&n, ykey);     //reads length and stores values for y[] 
    printf("*** MAIN: merge y[] shared memory key = %d\n", ykey);
    printf("*** MAIN: merge y[] shared memory created\n");
    y = (int*)shmat(ymid, NULL, 0);
    printf("*** MAIN: merge y[] shared memory attached and ready to use\n\n");

    //create the merge output memory
    zlen = m+n;
    printf("*** MAIN: merge output shared memory key = %d\n", zkey);
    zmid = shmget(zkey, zlen * sizeof(int),IPC_CREAT | 0666);
    printf("*** MAIN: merge z[] shared memory created\n");
    z = (int *) shmat(zmid, NULL, 0);
    //prints whether or not shmat was successful
    if ( z < 0) { printf("*** MAIN: merge output shmat() failed\n"); exit(1);}
    else{printf("*** MAIN: merge output shared memory attached and ready to use\n"); }

    //Prints out the values of the arrays
    printf("Input array for qsort has %d elements \n", k);
    printArray(a, k);
    printf("Input array x[] for merge has %d elements \n", m);
    printArray(x, m); //prints array
    printf("Input array y[] for merge has %d elements \n", n);
    printArray(y, n); //prints array
    
    
    int status;
    int fakepid = 1;
    int cpid = fork(); //creates child one
    if (cpid != 0 ){ //only parent
        fakepid = 2; //2
        cpid = fork(); //creates child 2
    }
     //section for children
    if (cpid == 0) {
        char argBuf[4][BUF];
        //child one process (qsort)
        if(fakepid== 1){ 
            char * qsortArgs[5]= { "qsort","0","","", "\0" }; //qsort, Left, Right, k
            snprintf(argBuf[0], BUF, "%d", k - 1); //writes the initial right value
            qsortArgs[2] = argBuf[0];
            snprintf(argBuf[1], BUF, "%d", amid); //writes length of k down because future calls will not have opportunity to do k+1
            qsortArgs[3] = argBuf[1];
            snprintf(printBuf,PRINTBUFSIZE,"*** MAIN: about to spawn the process for qsort\n"); //output
            write(1, printBuf, strlen(printBuf)); //prints the output
            int qsortstatus = execvp("./qsort", qsortArgs); //file plus args starts new process
            if (qsortstatus == -1){
                printf("!!!!!!!!!!!!!!!!!!!!!QSORT DIDNT INITIALIZE \n"); //printf because it's clearly broken so why bother
                exit(1);
            }
            printf("fakepid1 got here \n");
            //detatch all memory
            shmdt(x); //detatch x
            shmdt(y); //detatch y
            shmdt(z); //detatch z
            shmdt(a); //detatch a
            exit(0); //exits the children
        }
        if(fakepid == 2){
            //child 2 process fakepid must be 2
            char * mergeArgs[4] = { "merge","","","\0"}; //merge, xlength, ylength
            snprintf(argBuf[3], BUF, "%d", m); //writes the x length
            mergeArgs[1] = argBuf[3];
            snprintf(argBuf[4], BUF, "%d", n); //writes the y length
            mergeArgs[2] = argBuf[4];
            snprintf(printBuf,PRINTBUFSIZE,"*** MAIN: about to spawn the process for merge\n"); //output
            write(1, printBuf, strlen(printBuf)); //prints the output
            int mergeStatus = execvp("./merge", mergeArgs); //file plus args starts new process
            if (mergeStatus == -1){
                printf("!!!!!MERGE DIDNT INITIALIZE/ Incorrect Termination!!!!!!!!!!!!!!\n"); //printf because it's clearly broken so why bother
                exit(1);
            }
            //detatch all memory
            printf("fakepid1 got here \n");
            shmdt(x); //detatch x
            shmdt(y); //detatch y
            shmdt(z); //detatch z
            shmdt(a); //detatch a
            exit(0); //exits the child
        }
        printf("something went wrong and a child did not get caught"); //printf because it's clearly broken so why bother
        exit(1); //exits the children

    //Section for the original process
    }else{ //ends if   (cpid != 0)
        //Waits for children to complete
        for(int i = 0; i < 2; i++){
            wait(&status);
        }
        //!!!!! Child processes are all closed, printing and cleaing up   (can use printf because all other processes should be dead)
        //detatch and clear memory  
        //do these before others to prevent shared memory from remaining in case of segfault on printing the a and z arrays.
        shmdt(x); //detatch x
        printf("*** MAIN: merge x[] shared memory successfully detached\n");
        shmctl(xmid, IPC_RMID, NULL); //get rid of x[] memory as no longer needed
        printf("*** MAIN: merge x[] shared memory successfully removed\n\n");
        shmdt(y); //detatch y
        printf("*** MAIN: merge y[] shared memory successfully detached\n");
        shmctl(ymid, IPC_RMID, NULL); //get rid of y[] memory as no longer needed
        printf("*** MAIN: merge y[] shared memory successfully removed\n\n");

        //print out results
        //for qsort
        printf("*** MAIN: sorted array by qsort:\n");
        printArray(a,k);
        //for merge
        printf("*** MAIN: merged array:\n");
        printArray(z,zlen);



        //detatch and clear the rest of the memory
        shmdt(a); //detatch a
        printf("*** MAIN: qsort shared memory successfully detached\n");
        shmctl(amid, IPC_RMID, NULL); //get rid of x[] memory as no longer needed
        printf("*** MAIN: qsort shared memory successfully removed\n\n");
        shmdt(z); //detatch z
        printf("*** MAIN: merge output shared memory successfully detached\n");
        shmctl(zmid, IPC_RMID, NULL); //get rid of y[] memory as no longer needed
        printf("*** MAIN: merge output shared memory successfully removed\n");
    }

    exit(0);
}