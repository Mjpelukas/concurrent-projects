#include <complex>
#include <cmath>
using namespace std;

#ifndef _MANDEL
#define _MANDEL
typedef std::complex<double> dblcpoint;
class MandelPoint
{
public:
  dblcpoint coord;
  int inSet;
  int iterationsCompleted;
};

class MandelPointRegion
{
  dblcpoint topLeftPoint;
  double    horizontalLength;
  double    verticalLength;
  int       hpoints;
  int       vpoints;
  int       maxIterations;
  MandelPoint *points;
  dblcpoint   vstep;
  dblcpoint   hstep;
  
 public:
  MandelPoint getPoint(int index);
  void ComputePoints();

  MandelPointRegion() //-- Default parameters 
    {
      topLeftPoint={-2.0,1.25};
      horizontalLength=2.5;
      verticalLength=2.5;
      hpoints=500;
      vpoints=500;
      maxIterations=50;
      hstep={horizontalLength/(double)(hpoints-1),0};
      vstep={0,verticalLength/(double)(vpoints-1)};
      points=new MandelPoint[hpoints*vpoints];
    }
  MandelPointRegion(dblcpoint topLeft, double hlength, double vlength, int hpts, int vpts, int maxiter) 
    {
      topLeftPoint=topLeft;
      horizontalLength=hlength;
      verticalLength=vlength;
      hpoints=hpts;
      vpoints=vpts;
      maxIterations=maxiter;
      hstep={horizontalLength/(hpoints-1),0};
      vstep={0,verticalLength/(vpoints-1)};      
      points=new MandelPoint[hpoints*vpoints];
    }
};
#endif
