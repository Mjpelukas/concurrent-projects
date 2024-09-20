#include <complex>
#include <cmath>
#include "mandel.h"
using namespace std;

MandelPoint MandelPointRegion::getPoint(int index)
{
  return points[index];
}

void MandelPointRegion::ComputePoints() 
{
  
//-----------------------------------------------------------------
// Computes members of mandelbrot for specified square region
// of the real/imaginary plane.
//-----------------------------------------------------------------
  
  dblcpoint z;      //-- Temporary used in mandelbrot recurrence computation
  dblcpoint ci;     //-- Temporary used in mandelbrot recurrence computation
  dblcpoint c;      //-- Temporary used in mandelbrot recurrence computation
  double size;      //-- Temporary used in mandelbrot recurrence computation
  int iterations;   //-- Recurrence iteration counters
  int pheight;      //-- Counter for loop over pixels down imaginary axis
  int pwidth;       //-- Counter for loop over pixels across real axis
  int i;            //-- Loop index for recurrence relation
  int pointIndex;   //-- Points in a 1D array by row; this is the index
  //
  // -- Start the mandelbrot set identification calculation 
  //    Start at the left corner and work down the imaginary
  //    axis by row. 
  //
  z={0,0};
  size=0;
  ci=topLeftPoint;

  //--- Outer loop works down the imaginary plane (vertical)
  for (pheight=0;pheight<vpoints;pheight++){
      c=ci;
      //-- Inner loop works across the real plane (horizontal)
      for (pwidth=0;pwidth<hpoints;pwidth++){
       
#ifdef DEBUGCALC	
	       printf("\nc = %f + %f\n", c.real(), c.imag());
#endif
           //-- Calculation to identify whether this point is in the set.
           //   If the recurrence value exceeds two before maxiteration
	   //   is hit, it is not in the set.  Otherwise, the value
           //   has remained bounded and it is in the set.
	   //
           z=0;
	   iterations=0;
           for (i=1;i<=maxIterations;i++){
	        z=pow(z,2)+c; //-- The recurence
		size=abs(z);  //--- Size.  Bounded if in the set.
  	        if (size>2.0){  
	          iterations=i; //-- We know now its not bounded. No need
		                //   to continue.
	          break;
	        }
           }
	  if (i>maxIterations)iterations=maxIterations;
	  //
	  //-- Fill in the points array.
	  //   Points are in a 1D array in row major order.
	  //
	  pointIndex=pheight*hpoints+pwidth;
	  points[pointIndex].coord=c;  //-- Coordinate in real, imag plane
	  points[pointIndex].inSet=(maxIterations==iterations); //-- In set?
	  points[pointIndex].iterationsCompleted=iterations;    //-- Can be used to assign a color
          c=c+hstep;
      } //-- On to next point in the row
      ci=ci-vstep;
  } //-- Done with this row
}
