// This is mul/mil/algo/mil_algo_exp_filter_2d.h
#ifndef mil_algo_exp_filter_2d_h_
#define mil_algo_exp_filter_2d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//: \file
//  \brief Apply exponential filter to a 2D image
//  \author Tim Cootes

#include <mil/mil_image_2d_of.h>
class vsl_b_ostream;
class vsl_b_istream;

//: Apply exponential filter to a 2D image
template<class srcT, class destT>
class mil_algo_exp_filter_2d
{
  //: Filtered image workspace
  mil_image_2d_of<destT> x_filtered_;
 public:
  //: Apply exponential filter along x to 2D image
  //  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
  //  Uses fast recursive implementation.
  void filter_x(mil_image_2d_of<destT>& dest, mil_image_2d_of<srcT>& src, double k);

  //: Apply exponential filter along y to 2D image
  //  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
  //  Uses fast recursive implementation.
  void filter_y(mil_image_2d_of<destT>& dest, mil_image_2d_of<srcT>& src, double k);

  //: Apply exponential filter along x and y to 2D image
  //  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
  //  Uses fast recursive implementation.
  //
  //  Applies filter in x to generate image which can be obtained by x_filtered()
  void filter_xy(mil_image_2d_of<destT>& dest, mil_image_2d_of<srcT>& src, double k);

  //: Result of filtering source along x in last call to filter_xy(dest,src,k)
  const mil_image_2d_of<destT>& x_filtered()  const { return x_filtered_; }

  //: Apply exponential filter along x to single plane
  void filter_x(destT* dest, const srcT* src, int nx, int ny,
                int d_x_step, int d_y_step,
                int s_x_step, int s_y_step, double k);
};

#endif // mil_algo_exp_filter_2d_h_
