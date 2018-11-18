//:
// \file
// \author Isabel Restrepo
// \date 11-Oct-2010

#include <iostream>
#include <cmath>
#include "bvpl_algebraic_functor.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


// Default constructor

bvpl_algebraic_functor::bvpl_algebraic_functor()
{
  this->init();
}

//:Initializes all local variables
void bvpl_algebraic_functor::init()
{
  result_=0.0f;
}


void bvpl_algebraic_functor::apply(const float& val, const bvpl_kernel_dispatch& d)
{
  result_+=(d.c_)*val;
}

float bvpl_algebraic_functor::result()
{
  float result = result_;

  //reset all variables
  init();

  return result;
}
