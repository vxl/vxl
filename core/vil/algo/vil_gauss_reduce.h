// This is core/vil/algo/vil_gauss_reduce.h
#ifndef vil_gauss_reduce_h_
#define vil_gauss_reduce_h_
//:
// \file
// \brief Functions to smooth and sub-sample image in one direction
// \author Tim Cootes
// Some of these are not templated because
// - Each type tends to need a slightly different implementation
// - Let's not have too many templates.

#include <vil/vil_image_view.h>
#include <vxl_config.h> // for vxl_byte

//: Smooth and subsample src_im to produce dest_im
//  Applies 1-5-8-5-1 smoothing filter in x and y, then samples every other pixel.
//  work_im provides workspace
// \relates vil_image_view
template<class T>
void vil_gauss_reduce(const vil_image_view<T>& src,
                      vil_image_view<T>& dest,
                      vil_image_view<T>& work_im);

//: Smooth and subsample src_im to produce dest_im (2/3 size)
//  Applies filter in x and y, then samples every other pixel.
//  work_im provides workspace
// \relates vil_image_view
template<class T>
void vil_gauss_reduce_2_3(const vil_image_view<T>& src_im,
                          vil_image_view<T>& dest_im,
                          vil_image_view<T>& work_im);

//: Smooth and subsample src_im to produce dest_im
//  Applies 1-2-1 smoothing filter in x and y, then samples every other pixel.
// \relates vil_image_view
template<class T>
void vil_gauss_reduce_121(const vil_image_view<T>& src,
                          vil_image_view<T>& dest);


class vil_gauss_reduce_params
{
  double scale_step_;
  double filt2_, filt1_, filt0_;
  double filt_edge2_, filt_edge1_, filt_edge0_;
  double filt_pen_edge2_, filt_pen_edge1_,
         filt_pen_edge0_, filt_pen_edge_n1_;
 public:
  explicit vil_gauss_reduce_params(double scale_step);
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

//: Smooth and subsample src_im by an arbitrary factor to produce dest_im
// \param worka provide workspace to avoid repetitive memory alloc and free
// \param workb provide workspace to avoid repetitive memory alloc and free
template <class T>
void vil_gauss_reduce_general(const vil_image_view<T>& src_im,
                              vil_image_view<T>& dest_im,
                              vil_image_view<T>& worka,
                              vil_image_view<T>& workb,
                              const vil_gauss_reduce_params& params);


//: Smooth and subsample src_im by an arbitrary factor to produce dest_im
// \relates vil_image_view
template <class T>
inline void vil_gauss_reduce_general(const vil_image_view<T>& src_im,
                                     vil_image_view<T>& dest_im,
                                     const vil_gauss_reduce_params& params)
{
  vil_image_view<T> tempA, tempB;
  vil_gauss_reduce_general(src_im, dest_im, tempA, tempB, params);
}


//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
//  Assumes dest_im has sufficient data allocated.
//
//  This is essentially a utility function, used by mil_gauss_pyramid_builder
//
//  By applying twice we can obtain a full gaussian smoothed and
//  sub-sampled 2D image
void vil_gauss_reduce(const vxl_byte* src_im,
                      unsigned src_nx, unsigned src_ny,
                      vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                      vxl_byte* dest_im,
                      vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);


//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
//  Assumes dest_im has sufficient data allocated.
//
//  This is essentially a utility function, used by mil_gauss_pyramid_builder
//
//  By applying twice we can obtain a full gaussian smoothed and
//  sub-sampled 2D image.
void vil_gauss_reduce(const float* src_im,
                      unsigned src_nx, unsigned src_ny,
                      vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                      float* dest_im,
                      vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);


//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
//  Assumes dest_im has sufficient data allocated.
//
//  This is essentially a utility function, used by mil_gauss_pyramid_builder
//
//  By applying twice we can obtain a full gaussian smoothed and
//  sub-sampled 2D image.
void vil_gauss_reduce(const int* src_im,
                      unsigned src_nx, unsigned src_ny,
                      vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                      int* dest_im,
                      vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);

//: Smooth and subsample single plane src_im in x to produce dest_im
//  Applies 1-5-8-5-1 filter in x, then samples
//  every other pixel.  Fills [0,(nx+1)/2-1][0,ny-1] elements of dest
//  Assumes dest_im has sufficient data allocated.
//
//  This is essentially a utility function, used by mil_gauss_pyramid_builder
//
//  By applying twice we can obtain a full gaussian smoothed and
//  sub-sampled 2D image.
void vil_gauss_reduce(const vxl_int_16* src_im,
                      unsigned src_nx, unsigned src_ny,
                      vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                      vxl_int_16* dest_im,
                      vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);

//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smoothes with a 3x3 filter and subsamples
void vil_gauss_reduce_121(const vxl_byte* src_im,
                          unsigned src_nx, unsigned src_ny,
                          vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                          vxl_byte* dest_im,
                          vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);

//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smoothes with a 3x3 filter and subsamples
void vil_gauss_reduce_121(const float* src_im,
                          unsigned src_nx, unsigned src_ny,
                          vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                          float* dest_im,
                          vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);

//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smoothes with a 3x3 filter and subsamples
void vil_gauss_reduce_121(const int* src_im,
                          unsigned src_nx, unsigned src_ny,
                          vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                          int* dest_im,
                          vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);

//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smoothes with a 3x3 filter and subsamples
void vil_gauss_reduce_121(const vxl_int_16* src_im,
                          unsigned src_nx, unsigned src_ny,
                          vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                          vxl_int_16* dest_im,
                          vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);

//: Smooth and subsample single plane src_im in x, result is 2/3rd size
//  Applies alternate 1-3-1, 1-1 filter in x, then samples
//  every other pixel.  Fills [0,(2*ni+1)/3-1][0,nj-1] elements of dest
//
//  Note, 131 filter only an approximation
void vil_gauss_reduce_2_3(const vxl_byte* src_im,
                          unsigned src_ni, unsigned src_nj,
                          vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                          vxl_byte* dest_im, vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);

//: Smooth and subsample single plane src_im in x, result is 2/3rd size
//  Applies alternate 1-3-1, 1-1 filter in x, then samples
//  every other pixel.  Fills [0,(2*ni+1)/3-1][0,nj-1] elements of dest
//
//  Note, 131 filter only an approximation
void vil_gauss_reduce_2_3(const int* src_im,
                          unsigned src_ni, unsigned src_nj,
                          vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                          int* dest_im, vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);

//: Smooth and subsample single plane src_im in x, result is 2/3rd size
//  Applies alternate 1-3-1, 1-1 filter in x, then samples
//  every other pixel.  Fills [0,(2*ni+1)/3-1][0,nj-1] elements of dest
//
//  Note, 131 filter only an approximation
void vil_gauss_reduce_2_3(const vxl_int_16* src_im,
                          unsigned src_ni, unsigned src_nj,
                          vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                          vxl_int_16* dest_im, vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);

//: Smooth and subsample single plane src_im in x, result is 2/3rd size
//  Applies alternate 1-3-1, 1-1 filter in x, then samples
//  every other pixel.  Fills [0,(2*ni+1)/3-1][0,nj-1] elements of dest
//
//  Note, 131 filter only an approximation
void vil_gauss_reduce_2_3(const float* src_im,
                          unsigned src_ni, unsigned src_nj,
                          vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                          float* dest_im, vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step);

#endif // vil_gauss_reduce_h_
