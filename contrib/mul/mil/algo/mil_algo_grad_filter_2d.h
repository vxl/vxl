// This is mul/mil/algo/mil_algo_grad_filter_2d.h
#ifndef mil_algo_grad_filter_2d_h_
#define mil_algo_grad_filter_2d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
//  \brief Apply various gradient filters to a 2D image
//  \author Tim Cootes

#include <mil/mil_image_2d_of.h>

class vsl_b_ostream;
class vsl_b_istream;

//: Apply exponential filter to a 2D image
template<class srcT, class destT>
class mil_algo_grad_filter_2d
{
 public:
  //: Apply Sobel 3x3 gradient filter to 2D image.
  //  dest has twice as many planes as src, with dest plane (2i) being the x-gradient
  //  of source plane i and dest plane (2i+1) being the y-gradient.
  static void filter_xy_3x3(mil_image_2d_of<destT>& dest, mil_image_2d_of<srcT>& src);

  //: Apply Sobel 3x3 gradient filter to 2D image
  static void filter_xy_3x3(mil_image_2d_of<destT>& grad_x,
                     mil_image_2d_of<destT>& grad_y,
                     mil_image_2d_of<srcT>& src);
  //: Apply simple 1x3 gradient filter to 2D image.
  //  dest has twice as many planes as src, with dest plane (2i) being the x-gradient
  //  of source plane i and dest plane (2i+1) being the y-gradient.
  static void filter_xy_1x3(mil_image_2d_of<destT>& dest, mil_image_2d_of<srcT>& src);

  //: Apply simple 1x3 gradient filter to 2D image
  static void filter_xy_1x3(mil_image_2d_of<destT>& grad_x,
                     mil_image_2d_of<destT>& grad_y,
                     mil_image_2d_of<srcT>& src);
};

#endif // mil_algo_grad_filter_2d_h_
