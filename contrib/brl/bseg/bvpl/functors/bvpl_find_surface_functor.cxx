//:
// \file
#include <iostream>
#include <cmath>
#include <iomanip>
#include "bvpl_find_surface_functor.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
    P1_ += std::log(val);
    n1_++;
  }
}

float bvpl_find_surface_functor::result()
{
  P1_/=(float)n1_;

  float result =0.0f;

  if (std::exp(P1_) > 0.005f)
    result = 1.0f;


  //reset all variables
  init();

  return result;
}
