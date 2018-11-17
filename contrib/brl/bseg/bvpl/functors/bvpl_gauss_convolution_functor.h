// This is brl/bseg/bvpl/functors/bvpl_gauss_convolution_functor.h
#ifndef bvpl_gauss_convolution_functor_h
#define bvpl_gauss_convolution_functor_h
//:
// \file
// \brief A functor that convolves a kernel with gaussian distributions
// \author Isabel Restrepo mir@lems.brown.edu
// \date  August 10, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/kernels/bvpl_kernel_iterator.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_attributes.h>

//: This class convolves a kernel with Gaussian distributions.
// Therefore, the response of the kernel is a Gaussian distribution obtained
// as a linear combination of input Gaussians weighted by the kernel value.
class bvpl_gauss_convolution_functor
{
 public:
  //: Default constructor
  bvpl_gauss_convolution_functor();

  //: constructor that passes kernel to find normalization constant
  bvpl_gauss_convolution_functor(const bvpl_kernel_iterator& kernel);

  //: Destructor
  ~bvpl_gauss_convolution_functor() = default;

  //: Multiply the dispatch and the input gaussians together
  void apply(bsta_gauss_sf1& val, bvpl_kernel_dispatch& d);

  //: Returns the final operation of this functor
  bsta_gauss_sf1 result();

 private:
  float mean_;
  float var_;
  float max_;

  //: Initializes class variables
  void init();
  //: Sets max response (member \p max_) from convolution kernel
  void set_max(bvpl_kernel_iterator kernel);
};

#endif
