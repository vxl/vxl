// This is core/vnl/algo/vnl_bracket_minimum.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Function to bracket a minimum
// \author Tim Cootes
// \date   Feb 2007

#include "vnl_bracket_minimum.h"
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>

static const double GOLDEN_RATIO = 1.618033988749894848; // = 0.5*(vcl_sqrt(5)-1);

inline void swap(double& a, double& b)
{
  double x=a;
  a=b;
  b=x;
}

class vnl_bm_func
{
  vnl_vector<double> v;
  vnl_cost_function* f;
public:
  vnl_bm_func(vnl_cost_function& fn)
    { f=&fn; v.set_size(1); }

  double operator()(double x) { v[0]=x; return f->f(v); }
};

//: Given initial values a and b, find bracket a<b<c s.t. f(a)>f(b)<f(c)
//  Final function values at a,b,c stored in fa,fb,fc
void vnl_bracket_minimum(vnl_cost_function& fn,
                        double& a, double& b, double& c,
                        double& fa, double& fb, double& fc)
{
  // Set up object to evaluate function
  // Note that fn takes a vector input - f converts a scalar to a vector
  vnl_bm_func f(fn);

  if (b==a) b=a+1.0;
  fa = f(a);
  fb = f(b);

  // Arrange that fb<=fa
  if (fb>fa)
  {
    swap(a,b); swap(fa,fb);
  }

  // Initial guess at c
  c = b+ GOLDEN_RATIO*(b-a);
  fc = f(c);

  while (fc<fb)  // Keep stepping until we go uphill again
  {
    // Next guess is at u
    double u = c+GOLDEN_RATIO*(c-b);

    // Move bracket
    a=b;   b=c;    c=u;
    fa=fb; fb=fc; fc=f(c);
  }

  // Ensure a<b<c
  if (a>c)
  {
    swap(a,c); swap(fa,fc);
  }
}

