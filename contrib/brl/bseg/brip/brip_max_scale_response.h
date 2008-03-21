// This is brl/bseg/brip/brip_max_scale_response.h
#ifndef brip_scale_extrema_h_
#define brip_scale_extrema_h_
//:
// \file
// \brief Compute an image with values of scale according to maximum gradient matrix trace response
// \author J. L. Mundy, March 18, 2008
//
// The trace of the gradient matrix,
//   (Ix)^2   IxIy
//   IxIy     (Iy)^2
// is invariant to feature orientation
// The input can be either an image pyramid or a base image
// In the latter case a pyramid is generated as input to the scale max response
// search. The interval between scales is uniform on according to the 
// logarithm of scale ratios. For example if the scale range is 
// from 1 to 8 the scale intervals are 1, 1.4, 2, 2.8, 4, 5.6, 8, i.e.,
// 8/4 = 2, 2^(0.5) = 1.4, 4*1.4 = 5.6
// 
#include <vil/vil_image_view.h>
#include <vcl_vector.h>

template <class T> 
class brip_max_scale_response
{
 public:
  //:compute pyramid from base
  brip_max_scale_response( vil_image_view<T> const& base_image,
                           float max_scale);
  //: a pre-computed pyramid
  brip_max_scale_response( vcl_vector<vil_image_view<T> > const& pyramid);

  //: a single image with the natual scale at each pixel
  vil_image_view<float> scale_base();

  //: a pyramid of natural scales at each pixel
  vcl_vector<vil_image_view<float> > scale_pyramid();

  //: a mask that is true if the natural scale of pixel at a pyramid level is the same as the scale of the pyramid level
  vcl_vector<vil_image_view<vxl_byte> > mask_pyramid();

  //: for debugging purposes -- not normally used
  vcl_vector<vil_image_view<float> > trace_pyramid(){return trace_;}
 protected:
  //internal functions
  brip_max_scale_response(): trace_valid_(false){}
  void compute_trace_pyramid();
  vcl_vector<float> pyramid_scales_;
  vcl_vector<vil_image_view<float> > pyramid_;
  bool trace_valid_;
  vcl_vector<vil_image_view<float> > trace_;
};

#endif // brip_scale_extrema_h_
