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
// from 1 to 8 and the scale ratio is sqrt(2), the scale intervals are:
// 1, 1.4, 2, 2.8, 4, 5.6, 8, i.e.,
// 8/4 = 2, 2^(0.5) = 1.4, 4*1.4 = 5.6
//
#include <iostream>
#include <vector>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class brip_max_scale_response
{
 public:
  //:compute pyramid from base
  brip_max_scale_response( vil_image_view<T> const& base_image,
                           double scale_ratio,
                           double max_scale);
  //: a pre-computed pyramid
  brip_max_scale_response( std::vector<vil_image_view<T> > const& pyramid);

  //: The vector of image scales
  std::vector<float> scales() const {return pyramid_scales_;}

  //: A pyramid from a base image with scales according to this->scales()
  std::vector<vil_image_view<T> > image_pyramid(vil_image_view<T> const& base);

  //: a single image with the natural scale at each pixel
  vil_image_view<float> scale_base();

  //: a pyramid of natural scales at each pixel
  std::vector<vil_image_view<float> > scale_pyramid();

  //: a mask that is true if the natural scale of pixel at a pyramid level is the same as the scale of the pyramid level
  std::vector<vil_image_view<vxl_byte> > mask_pyramid();

  //: for debugging purposes -- not normally used
  std::vector<vil_image_view<float> > trace_pyramid() const {return trace_;}
 protected:
  //internal functions
  brip_max_scale_response(): trace_valid_(false){}
  void compute_trace_pyramid();
  std::vector<float> pyramid_scales_;
  std::vector<vil_image_view<float> > grey_pyramid_;
  bool trace_valid_;
  std::vector<vil_image_view<float> > trace_;
};

#endif // brip_scale_extrema_h_
