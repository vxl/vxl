#include "bvpl_negative_gauss_conv_functor.h"
//:
// \file


//: Default Constructor
bvpl_negative_gauss_conv_functor::bvpl_negative_gauss_conv_functor()
{
  init();
}

//: Initialize class variables
void bvpl_negative_gauss_conv_functor::init()
{
  mean_= 0.0f;
  var_ = 0.0f;
}

//: Multiply the dispatch and the input gaussians together
void bvpl_negative_gauss_conv_functor::apply(bsta_gauss_sf1& gauss, bvpl_kernel_dispatch& d)
{
  mean_+= d.c_*gauss.mean();
  var_+= d.c_*d.c_*gauss.var();
}

//: Return the final result
bsta_gauss_sf1 bvpl_negative_gauss_conv_functor::result()
{
  bsta_gauss_sf1 final_gauss;


  final_gauss.set_mean((mean_<0.0f)?std::abs(mean_):0.0f);
  final_gauss.set_var(var_);

  init();

  return final_gauss;
}
