/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vnl_sample.h"

#include <vcl/vcl_cmath.h>
#include <vcl/vcl_cstdlib.h>

// yes, I know that rand() is not necessarily a good random number generator.

double vnl_sample_uniform(double a, double b) {
  return a + (b-a)*rand()/double(RAND_MAX);
}

double vnl_sample_normal(double mean, double sigma) {
  double x1 = vnl_sample_uniform(0, 1);
  double x2 = vnl_sample_uniform(0, 1);
  
  double r = sqrt(-2*log(x1));
  double y1 = r * cos(2*3.1415926*x2);
  //double y2 = r * sin(2*3.1415926*x2);
  
  return mean + sigma * y1;
}
