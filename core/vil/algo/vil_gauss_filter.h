// This is mul/vil2/algo/vil2_gauss_filter.h
#ifndef vil2_gauss_filter_h_
#define vil2_gauss_filter_h_
//:
//  \file
//  \brief Smoothes images.
//  \author Ian Scott

//=======================================================================
// inclusions

#include <vil2/vil2_image_view.h>

class vil2_gauss_filter_params
{
  double scale_step_;
  double filt2_, filt1_, filt0_;
  double filt_edge2_, filt_edge1_, filt_edge0_;
  double filt_pen_edge2_, filt_pen_edge1_,
         filt_pen_edge0_, filt_pen_edge_n1_;
public:
  explicit vil2_gauss_filter_params(double scale_step);
  //: the scale step between pyramid levels
  double scale_step() const {return scale_step_;}

  //: Filter tap value
  // The value of the two outside elements of the 5-tap 1D FIR filter
  double filt2() const { return filt2_;}
  //: Filter tap value
  // The value of elements 2 and 4 of the 5-tap 1D FIR filter
  double filt1() const { return filt1_;}
  //: Filter tap value
  // The value of the central element of the 5-tap 1D FIR filter
  double filt0() const { return filt0_;}

  //: Filter tap value
  // The value of the first element of the 3 tap 1D FIR filter for use at the edge of the window
  // Corresponds to the filt2_ elements in a symmetrical filter
  double filt_edge2() const { return filt_edge2_;}
  //: Filter tap value
  // The value of the second element of the 3 tap 1D FIR filter for use at the edge of the window
  // Corresponds to the filt1_ elements in a symmetrical filter
  double filt_edge1() const { return filt_edge1_;}
  //: Filter tap value
  // The value of the third element of the 3 tap 1D FIR filter for use at the edge of the window
  // Corresponds to the filt0_ element in a symmetrical filter
  double filt_edge0() const { return filt_edge0_;}

  //: Filter tap value
  // The value of the first element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
  // Corresponds to the filt2_ elements in a symmetrical filter
  double filt_pen_edge2() const { return filt_pen_edge2_;}
  //: Filter tap value
  // The value of the second element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
  // Corresponds to the filt1_ elements in a symmetrical filter
  double filt_pen_edge1() const { return filt_pen_edge1_;}
  //: Filter tap value
  // The value of the third element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
  // Corresponds to the filt0_ elements in a symmetrical filter
  double filt_pen_edge0() const { return filt_pen_edge0_;}
  //: Filter tap value
  // The value of the fourth element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
  // Corresponds to the filt1_ elements in a symmetrical filter
  double filt_pen_edge_n1() const { return filt_pen_edge_n1_;}
};


    //: Smooth a single plane src_im to produce dest_im
    //  Applies 5 element FIR filter in x and y.
    //  Assumes dest_im has sufficient data allocated.
template <class srcT, class destT>
void vil2_gauss_filter_5tap(const srcT* src_im, vcl_ptrdiff_t src_ystep,
                            unsigned ni, unsigned nj,
                            destT* dest_im, vcl_ptrdiff_t dest_ystep,
                            const vil2_gauss_filter_params& params,
                            destT* work);

//: Smooth a src_im to produce dest_im
//  Applies 5 element FIR filter in x and y.
template <class srcT, class destT>
void vil2_gauss_filter_5tap(const vil2_image_view<srcT>& src_im,
                            vil2_image_view<destT>& dest_im,
                            const vil2_gauss_filter_params &params,
                            vil2_image_view<destT> &work);



//: Smooth a src_im to produce dest_im
//  Applies 5 element FIR filter in x and y.
template <class srcT, class destT>
inline void vil2_gauss_filter_5tap(const vil2_image_view<srcT>& src_im,
                                   vil2_image_view<destT>& dest_im,
                                   const vil2_gauss_filter_params &params)
{
  vil2_image_view<destT> work;
  vil2_gauss_filter_5tap(src_im, dest_im, params, work);
}

#endif // vil2_gauss_filter_h_
