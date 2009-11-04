//:
// \file
#include "bvpl_find_surface_functor.h"

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
// Default constructor
bvpl_find_surface_functor::bvpl_find_surface_functor()
{
  this->init();
}

//:Initializes all local variables
void bvpl_find_surface_functor::init()
{
  P1_ = float(0);
  n1_=0;
}

void bvpl_find_surface_functor::apply(float& val, bvpl_kernel_dispatch& d)
{
  if (d.c_ > 0) {
    P1_ += vcl_log(val);
    n1_++;
  }
}

float bvpl_find_surface_functor::result()
{
  P1_/=(float)n1_;

  float result =0.0f;

  if (vcl_exp(P1_) > 0.005f)
    result = 1.0f;


  //reset all variables
  init();

  return result;
}
