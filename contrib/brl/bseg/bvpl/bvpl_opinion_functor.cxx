//:
// \file
#include "bvpl_opinion_functor.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>


// Default constructor
bvpl_opinion_functor::bvpl_opinion_functor()
{
  this->init();
}

//:Initializes all local variables
void bvpl_opinion_functor::init()
{
  b_=0;
  u_=0;
  n_=0;
}

void bvpl_opinion_functor::apply(bvxm_opinion& val, bvpl_kernel_dispatch& d)
{
  if (d.c_ == 1) {
    b_=vcl_pow(b_, n_/n_+1)*vcl_pow(val.b(), 1/n_+1);
    u_=vcl_pow(b_, n_/n_+1)*vcl_pow(val.u(), 1/n_+1)+
      vcl_pow(u_, n_/n_+1)*vcl_pow(val.b(), 1/n_+1)+
      vcl_pow(u_, n_/n_+1)*vcl_pow(1, n_/n_+1);
  } else if (d.c_ == -1) {
    float x=1-val.u()-val.b(); 
    b_=vcl_pow(b_, n_/n_+1)*vcl_pow(x, 1/n_+1);
    u_=vcl_pow(b_, n_/n_+1)*vcl_pow(val.u(), 1/n_+1)+
      vcl_pow(u_, n_/n_+1)*vcl_pow(x, 1/n_+1)+
      vcl_pow(u_, n_/n_+1)*vcl_pow(1, n_/n_+1);
  }
  n_++;
}

bvxm_opinion bvpl_opinion_functor::result()
{
  bvxm_opinion result(b_,u_);
  
  //reset all variables
  init();
  
  return result;
}
