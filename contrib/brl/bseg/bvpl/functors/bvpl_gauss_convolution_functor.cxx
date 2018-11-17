#include "bvpl_gauss_convolution_functor.h"
//:
// \file


//: Default Constructor
bvpl_gauss_convolution_functor::bvpl_gauss_convolution_functor()
{
  max_ =0.0;
  init();
}

//: Constructor
bvpl_gauss_convolution_functor::bvpl_gauss_convolution_functor(const bvpl_kernel_iterator& kernel)
{
  //compute max
  set_max(kernel);
  init();
}

//: Initialize class variables
void bvpl_gauss_convolution_functor::init()
{
  mean_= 0.0f;
  var_ = 0.0f;
}

//: Multiply the dispatch and the input gaussians together
void bvpl_gauss_convolution_functor::apply(bsta_gauss_sf1& gauss, bvpl_kernel_dispatch& d)
{
  mean_+= d.c_*gauss.mean();
  var_+= d.c_*d.c_*gauss.var();
}

//: Return the final result
bsta_gauss_sf1 bvpl_gauss_convolution_functor::result()
{
   bsta_gauss_sf1 final_gauss;
   //if (max_ > 1.0e-10){
   // final_gauss.set_mean(mean_/max_);
   // final_gauss.set_var(var_);
   //}
   //else{ // this case is happens when max_ is not initialized
     final_gauss.set_mean(mean_);
     final_gauss.set_var(var_);
   //}
  init();
  return final_gauss;
}

void bvpl_gauss_convolution_functor::set_max(bvpl_kernel_iterator kernel_iter)
{
  float max_val = 0.0f;
  kernel_iter.begin();
  while (!kernel_iter.isDone()) {
    bvpl_kernel_dispatch d = *kernel_iter;
    if ( d.c_>0.0f)
      max_val += d.c_ * 0.99f;
    else
      max_val += d.c_ *0.01f;

    ++kernel_iter;
  }

  std::cout << "max response : " << max_val << std::endl;
  max_ = max_val;
}
