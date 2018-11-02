// This is core/vil/algo/vil_gauss_filter.h
#ifndef vil_gauss_filter_h_
#define vil_gauss_filter_h_
//:
// \file
// \brief Smooths images.
// \author Ian Scott

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/algo/vil_convolve_1d.h>
#include <vil/vil_transpose.h>

class vil_gauss_filter_5tap_params
{
  double sigma_;
  double filt2_, filt1_, filt0_;
  double filt_edge2_, filt_edge1_, filt_edge0_;
  double filt_pen_edge2_, filt_pen_edge1_,
         filt_pen_edge0_, filt_pen_edge_n1_;
 public:
  //: Set the
  explicit vil_gauss_filter_5tap_params(double sigma_);
  //: The width of the Gaussian
  double sigma() const {return sigma_;}

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
void vil_gauss_filter_5tap(const srcT* src_im, std::ptrdiff_t src_ystep,
                           unsigned ni, unsigned nj,
                           destT* dest_im, std::ptrdiff_t dest_ystep,
                           const vil_gauss_filter_5tap_params& params,
                           destT* work);

//: Smooth a src_im to produce dest_im
//  Applies 5 element FIR filter in x and y.
template <class srcT, class destT>
void vil_gauss_filter_5tap(const vil_image_view<srcT>& src_im,
                           vil_image_view<destT>& dest_im,
                           const vil_gauss_filter_5tap_params &params,
                           vil_image_view<destT> &work);


//: Smooth a src_im to produce dest_im
//  Applies 5 element FIR filter in x and y.
template <class srcT, class destT>
inline void vil_gauss_filter_5tap(const vil_image_view<srcT>& src_im,
                                  vil_image_view<destT>& dest_im,
                                  const vil_gauss_filter_5tap_params &params)
{
  vil_image_view<destT> work;
  vil_gauss_filter_5tap(src_im, dest_im, params, work);
}


//: Generate an n-tap FIR filter from a Gaussian function.
// The filter uses the equation $k D^d \exp -\frac{x^2}{2\sigma^2} $,
// where D is the differential operator, and k is a normalising constant.
// \param diff The number of differential operators to apply to the filter.
// If you want just a normal gaussian, set diff to 0.
// \param sd The width of the gaussian.
//
// The taps will be calculated using the integral of the above equation over
// the pixel width. However, aliasing will reduce the meaningfulness of
// your filter when sd << (diff+1). In most applications you will
// want filter.size() ~= sd*7, which will avoid significant truncation,
// without wasting the outer taps on near-zero values.
void vil_gauss_filter_gen_ntap(double sd, unsigned diff,
                               std::vector<double> &filter_dest);

//: Smooth a src_im to produce dest_im with gaussian of width sd
//  Generates gaussian filter of width sd, using (2*half_width+1)
//  values in the filter.  Typically half_width>3sd.
//  Convolves this with src_im to generate dest_im.
template <class srcT, class destT>
inline void vil_gauss_filter_1d(const vil_image_view<srcT>& src_im,
                                vil_image_view<destT>& dest_im,
                                double sd, unsigned half_width)
{
  std::vector<double> filter(2*half_width+1);
  vil_gauss_filter_gen_ntap(sd,0,filter);
  vil_convolve_1d(src_im,dest_im,&filter[half_width],-int(half_width),half_width,
                  float(),vil_convolve_zero_extend,vil_convolve_zero_extend);
}

//: Smooth a src_im to produce dest_im with gaussian of width sd
//  Generates gaussian filter of width sd, using (2*half_width+1)
//  values in the filter.  Typically half_width>3sd.
//  Convolves this with src_im to generate work_im, then applies filter
//  vertically to generate dest_im.
template <class srcT, class destT>
inline void vil_gauss_filter_2d(const vil_image_view<srcT>& src_im,
                                vil_image_view<destT>& dest_im,
                                double sd, unsigned half_width,
                                vil_convolve_boundary_option boundary = vil_convolve_zero_extend)
{
  // Generate filter
  std::vector<double> filter(2*half_width+1);
  vil_gauss_filter_gen_ntap(sd,0,filter);

  // Apply 1D convolution along i direction
  vil_image_view<destT> work_im;
  vil_convolve_1d(src_im,work_im,&filter[half_width],-int(half_width),half_width,
                  float(), boundary, boundary);

  // Apply 1D convolution along j direction by applying filter to transpose
  dest_im.set_size(src_im.ni(),src_im.nj(),src_im.nplanes());
  vil_image_view<destT> work_im_t = vil_transpose(work_im);
  vil_image_view<destT> dest_im_t = vil_transpose(dest_im);

  vil_convolve_1d(work_im_t,dest_im_t,
                  &filter[half_width],-int(half_width),half_width,
                  float(), boundary, boundary);
}


//: Smooth a src_im to produce dest_im with gaussian of width sd
//  Generates two gaussian filters of width sd_i,sd_j, using (2*half_width_i+1)
//  values in the filter.  Typically half_width>3sd.
//  Convolves this with src_im to generate work_im, then applies filter
//  vertically to generate dest_im.
template <class srcT, class destT>
inline void vil_gauss_filter_2d(const vil_image_view<srcT>& src_im,
                                vil_image_view<destT>& dest_im,
                                double sd_i, unsigned half_width_i,
                                double sd_j, unsigned half_width_j,
                                vil_convolve_boundary_option boundary = vil_convolve_zero_extend)
{
  // Generate filter for i
  std::vector<double> filter_i(2*half_width_i+1);
  vil_gauss_filter_gen_ntap(sd_i,0,filter_i);

  // Apply 1D convolution along i direction
  vil_image_view<destT> work_im;
  vil_convolve_1d(src_im,work_im,&filter_i[half_width_i],-int(half_width_i),half_width_i,
                  float(), boundary, boundary);

  // Apply 1D convolution along j direction by applying filter to transpose
  dest_im.set_size(src_im.ni(),src_im.nj(),src_im.nplanes());
  vil_image_view<destT> work_im_t = vil_transpose(work_im);
  vil_image_view<destT> dest_im_t = vil_transpose(dest_im);

  // Generate filter for j
  std::vector<double> filter_j(2*half_width_j+1);
  vil_gauss_filter_gen_ntap(sd_j,0,filter_j);

  vil_convolve_1d(work_im_t,dest_im_t,
                  &filter_j[half_width_j],-int(half_width_j),half_width_j,
                  float(), boundary, boundary);
}


#endif // vil_gauss_filter_h_
