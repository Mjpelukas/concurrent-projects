//username: mjpeluka@mtu.edu
//days late: 5 days
//slip days using: 5 days
//slip days remaining: 0 days
#include "mandel.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
//-----------------------------------------------------------------
// Computes members of mandelbrot for specified region
// of the real/imaginary plane. Creates PPM image file
// according to one of several color schemes.
//-----------------------------------------------------------------
//
// Compilation: g++ -c mandel.cc
//              g++ -c pmandel.cc
//              g++ -o pmandel pmandel.o mandel.o -lm
//
// There is a makefile in the project directory
//-----------------------------------------------------------------
//
// Invocation: mandelPlot RealCoord ImagCoord RealSideLength ComplexSideLength EscapeIterations HorizontalPixels VerticalPixels ImageFile nprocs
//
// See the project handout for a detailed description of the parameters and
// operation of the program.
//------------------------------------------------------------------

#define PRINTBUFSIZE 241
#define COLORS 15 
unsigned char palette[COLORS][3] = { {255,255,255},{255,0,0},{255,128,0},{255,255,0},{128,255,0},{0,255,0},{0,255,128},{0,255,255},{0,128,255},{0,0,255},{128,0,225},{255,0,255},{255,0,128},{128,128,128},{0,0,0} };

//------------------------------------------------------------------
// Associate a color with one point and add point values
// to the PPM file.  See the project handout for a more complete
// description.
//------------------------------------------------------------------
void addPoint(int scheme, int iterations, int maxiterations, FILE* fp) {

    unsigned char color[3];
    int index;

    //
    //--- Three possible schemes for coloring each point.
    //
    if (scheme == 1) {
        // Scheme 1 - Greyscale uniform 
        color[0] = 255 - ((double)iterations / (double)maxiterations) * 255;
        color[1] = 255 - ((double)iterations / (double)maxiterations) * 255;
        color[2] = 255 - ((double)iterations / (double)maxiterations) * 255;
    }
    if ((scheme == 2) || (scheme == 3)) {
        if (scheme == 2) {
            iterations = sqrt((double)iterations / (double)maxiterations) * (float)(maxiterations);
        }

        if (iterations == maxiterations)
            index = COLORS - 1; //-- Points in the set are always black, assumed last in palette
        else
            index = iterations / (maxiterations / (COLORS - 1));

        color[0] = palette[index][0];
        color[1] = palette[index][1];
        color[2] = palette[index][2];
    }

    //-- Add the point the file
    fputc(color[0], fp);
    fputc(color[1], fp);
    fputc(color[2], fp);
}

//
//------------ Main 
//
int main(int argc, char* argv[]) {
    dblcpoint start;       //-- Top left corner
    double hrange;         //-- Length of a side in real plane   (horizontal length)
    double vrange;         //-- Length of a side in complex plane   (vertical length)
    int hpixels;           //-- Pixels on the real side
    int vpixels;           //-- Pixels on the imaginary side         (vertical pixels)
    double hincrement;     //-- Length between pixels in complex plane
    double vincrement;     //-- Length between pixels in complex plane
    int maxiterations;     //-- Number of iterations use to identify points in the set
    int i;                 //-- Loop index for recurrence relation
    FILE* fp;              //-- Descriptor for file to hold image
    int colorscheme;       //-- Color scheme for the plot
    char printBuf[PRINTBUFSIZE];     //-- Output buffer  
    MandelPointRegion* mandelRegion; //-- Point region -- contains points and methods to generate them
    MandelPoint point;               //-- One point
    int nprocs;  //number of processes defined
    //
    // -- Process the input arguments
    //
    if (argc != 10) {
        printf("mandel RealCoord ImagCoord RealSideLength ImagSideLength EscapeIterations HorizontalPixels VerticalPixels ImageFile nProcs\n");
        exit(1);
    }
    start = { strtod(argv[1],NULL),strtod(argv[2],NULL) };
    printf("Top left coordinate is: %f + %fi\n", start.real(), start.imag());
    hrange = strtod(argv[3], NULL);
    if (hrange <= 0) { printf("Real side must be greater than zero.\n"); exit(1); }
    printf("Length of real side:  %f\n", hrange);
    vrange = strtod(argv[4], NULL);
    if (vrange <= 0) { printf("Imaginary side must be greater than zero.\n"); exit(1); }
    printf("Length of vertical side:  %f\n", vrange);
    maxiterations = atoi(argv[5]);
    if (maxiterations < 2) { printf("Max iterations must be at least 2.\n"); exit(1); }
    hpixels = atoi(argv[6]);
    printf("Pixels on horizontal side:  %d\n", hpixels);
    if (hpixels < 10) { printf("Pixels on horizontal side must be at least 10.\n"); exit(1); }
    vpixels = atoi(argv[7]);
    printf("Pixels on vertical side:  %d\n", vpixels);
    if (vpixels < 10) { printf("Pixels on horizontal side must be at least 10.\n"); exit(1); }

    fp = fopen(argv[8], "wb"); /* b - binary mode */
    if (fp == NULL) { printf("%s cannot be opened for write\n", argv[6]); }
    (void)fprintf(fp, "P6\n%d %d\n255\n", hpixels, vpixels);
    fclose(fp); //close fp
    nprocs = atoi(argv[9]);
    if (nprocs < 1) { printf("nprocs must be at least 1.\n"); exit(1); }

    //
    // MY CODE BLOCK !!!!!!!!!!
    // 
        //set up 
    double pVrange = vrange / nprocs; //partial vrange
    int pVpixels = vpixels / nprocs; //partial pixels     (assumes that 
    int childNo = 0;
    pid_t initialPid = getpid();  //get pid of the parent process. 
    pid_t childPid;
    int status; 
    //creates all of the child processes
    while (childNo < nprocs){
        if (initialPid == getpid()) {   //could use (getppid() == 1) 
            childNo++;
            fork();
        }
        else {
            break; //exists in the case that it is a child
        }
    }
    //this will be used in both
    char fNameBuffer[PRINTBUFSIZE];
    if (initialPid == getpid()) {

        //wait for all child processes
        for (int i = 0; i < nprocs; i++)
        {
            childPid = wait(&status);
        }


        // Parent combine the files
        fp = fopen(argv[8], "a");
        FILE* childFile; //sets up the child file
        char ch;

        for (int j = 1; j < nprocs + 1; j++) { // loop for all children processes
            int buffOffset = snprintf(fNameBuffer, PRINTBUFSIZE, "%c", argv[8][0]);
            for (int i = 1; i < strlen(argv[8]) - 4; i++) { //  minus 4 gets everything before ".ppm" 
                snprintf(fNameBuffer + buffOffset, PRINTBUFSIZE - buffOffset, "%c", argv[8][i]);
                buffOffset++;
            }
            snprintf(fNameBuffer + buffOffset, PRINTBUFSIZE - buffOffset, "%i.ppm", j);
            childFile = fopen(fNameBuffer, "r"); //open child file in read mode
            ch = fgetc(childFile); //gets first char
            while (ch != EOF) { //while not at end of file
                fputc(ch,fp);    //was     fprint(fp, "%c", ch);
                ch = fgetc(childFile); //gets next char
            }
            fclose(childFile);
        }
        fclose(fp);
    }
    else { // WHAT THE CHILDREN RUN
        //childNo++; //adjusts childNo to start at 0
        
      
            //Change start to where the process will start
        start = { start.real(), start.imag() - (pVrange * (childNo - 1)) };
      //
          //-- Output the calculation parameters
          //
        snprintf(printBuf, PRINTBUFSIZE, "Process %d testing rectangle at %.8f + %.8f \n\twidth %.8f and height %.8f \n\tplot area width %d by height %d pixels.\n", getpid(), start.real(), start.imag(), hrange, pVrange, hpixels, pVpixels);
        write(1, printBuf, strlen(printBuf));

            
                //
                //--- Create a new point region and calculate the point values.  The "value" is
                //    the number of iterations before the recurrence value exceeds 2. If maxiterations
                //    is reached without exceeding 2, the point is not in the set.
                //
                mandelRegion = new MandelPointRegion(start, hrange, pVrange, hpixels, pVpixels, maxiterations);
        mandelRegion->ComputePoints();


        // nameBuffer[(strlen(argv[8]) + strlen]             //was trying somethinging more efficient here 


        //GETS FILE NAME
        int buffOffset = snprintf(fNameBuffer, PRINTBUFSIZE, "%c", argv[8][0]);
        for (int i = 1; i < strlen(argv[8]) - 4; i++) { //  minus 4 gets everything before ".ppm" 
            snprintf(fNameBuffer + buffOffset, PRINTBUFSIZE - buffOffset, "%c", argv[8][i]);
            buffOffset++;
        }
        snprintf(fNameBuffer + buffOffset, PRINTBUFSIZE - buffOffset, "%i.ppm", childNo);

        //--- Now create the image file
        //

        fp = fopen(fNameBuffer, "wb");

    #ifndef COLORSCHEME
        colorscheme = 1;
    #else
        colorscheme = COLORSCHEME;
    #endif

        for (i = 0; i < hpixels * pVpixels; i++)
        {
            point = mandelRegion->getPoint(i);
            addPoint(colorscheme, point.iterationsCompleted, maxiterations, fp);
        }
        fclose(fp);
        //
        //-- Done
        //
        snprintf(printBuf, PRINTBUFSIZE, "Process %d done.\n", getpid());
        write(1, printBuf, strlen(printBuf));
    }
}