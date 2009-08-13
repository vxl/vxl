#include "bvpl_gauss_convolution_functor.h"

//: Constructor
bvpl_gauss_convolution_functor::bvpl_gauss_convolution_functor()
{
  init();
}

//: Initialize class variables
void bvpl_gauss_convolution_functor::init()
{
  mean_= 0.0f;
  var_ = 0.0f;
}

//: Multiply the dispatch and the input gaussians together
void bvpl_gauss_convolution_functor::apply(bsta_gauss_f1& gauss, bvpl_kernel_dispatch& d)
{
  mean_+= d.c_*gauss.mean();
  var_+= d.c_*d.c_*gauss.var();
  
}

//: Return the final result
bsta_gauss_f1 bvpl_gauss_convolution_functor::result()
{
  return bsta_gauss_f1(mean_, var_);
  init();
}