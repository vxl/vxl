// This is mul/mil/algo/mil_algo_gaussian_filter.txx
#ifndef mil_algo_gaussian_filter_txx_
#define mil_algo_gaussian_filter_txx_
//:
//  \file
//  \brief smooth images
//  \author Ian Scott

#include "mil_algo_gaussian_filter.h"
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vsl/vsl_indent.h>
#include <mbl/mbl_gamma.h>
#include <mil/mil_image_2d_of.h>

//=======================================================================

template <class srcT, class destT>
mil_algo_gaussian_filter<srcT, destT>::mil_algo_gaussian_filter()
{
  set_width(2.0);
}


//: Construct with given scale_step
template <class srcT, class destT>
mil_algo_gaussian_filter<srcT, destT>::mil_algo_gaussian_filter(double scale_step)
{
  set_width(scale_step);
}

//=======================================================================

template <class srcT, class destT>
mil_algo_gaussian_filter<srcT, destT>::~mil_algo_gaussian_filter()
{
}

template <class srcT, class destT>
void mil_algo_gaussian_filter<srcT, destT>::filter(
           mil_image_2d_of<destT>& dest_im,
           const mil_image_2d_of<srcT>& src_im) const
{
  int nx = src_im.nx();
  int ny = src_im.ny();
  int n_planes = src_im.n_planes();
  dest_im.resize(nx, ny, n_planes);
  dest_im.setWorld2im(src_im.world2im());
  worka_.resize(nx,ny,1);

  int ystep = src_im.ystep();


  // Reduce plane-by-plane


  for (int i=0;i<n_planes;++i)
    filter(dest_im.plane(i),dest_im.ystep(),
                 src_im.plane(i),nx,ny,ystep);
#if 0
  vsl_indent_inc(vcl_cout);
  vcl_cout << vsl_indent() << "Work image B\n";
  workb_.print_all(vcl_cout);
  vsl_indent_dec(vcl_cout);
#endif
}


//: An optimisable rounding function
inline unsigned char l_round(double x, unsigned char )
{  return (unsigned char) (x+0.5);}

inline signed char l_round(double x, signed char )
{  return (signed char) (x+0.5);}

inline unsigned short l_round(double x, unsigned short )
{  return (unsigned short) (x+0.5);}

inline signed short l_round(double x, signed short )
{  return (signed short) (x+0.5);}

inline unsigned int l_round(double x, unsigned int )
{  return (unsigned int) (x+0.5);}

inline signed int l_round(double x, signed int )
{  return (signed int) (x+0.5);}

inline unsigned long l_round(double x, unsigned long )
{  return (unsigned long) (x+0.5);}

inline signed long l_round(double x, signed long )
{  return (signed long) (x+0.5);}

inline double l_round (double x, double )
{  return x; }

inline float l_round (double x, float )
{  return (float) x; }

//=======================================================================
//: Smooth and subsample src_im to produce dest_im
//  Applies 5 pin filter in x and y, then samples
//  every other pixel.
//  Assumes dest_im has sufficient data allocated

template <class srcT, class destT>
void mil_algo_gaussian_filter<srcT, destT>::filter(
           destT* dest_im, int dest_ystep,
           const srcT* src_im,
           unsigned nx, unsigned ny, int src_ystep) const
{
  // Convolve src with a 5 x 1 Gaussian filter,
  // placing result in work_

  destT* worka_im = worka_.plane(0);

  int work_ystep = worka_.ystep();

  // First perform horizontal smoothing
  for (unsigned int y=0;y<ny;y++)
  {
    destT* worka_row = worka_im + y*work_ystep;
    const srcT* src_col3  = src_im + y*src_ystep;
    const srcT* src_col2  = src_col3 - 1;
    const srcT* src_col1  = src_col3 - 2;
    const srcT* src_col4  = src_col3 + 1;
    const srcT* src_col5  = src_col3 + 2;

    int x;
    int nx2 = nx-2;
    for (x=2;x<nx2;x++)
      worka_row[x] = l_round(  filt2_ * src_col1[x]
                             + filt1_ * src_col2[x]
                             + filt0_ * src_col3[x]
                             + filt1_ * src_col4[x]
                             + filt2_ * src_col5[x], (destT)0);

    // Now deal with edge effects :
    worka_row[0] = l_round( filt_edge0_ * src_col3[0]
                          + filt_edge1_ * src_col4[0]
                          + filt_edge2_ * src_col5[0], (destT)0);

    worka_row[1] = l_round( filt_pen_edge_n1_ * src_col2[1]
                          + filt_pen_edge0_ * src_col3[1]
                          + filt_pen_edge1_ * src_col4[1]
                          + filt_pen_edge2_ * src_col5[1], (destT)0);

    worka_row[nx-2] = l_round( filt_pen_edge2_ * src_col1[x]
                             + filt_pen_edge1_ * src_col2[x]
                             + filt_pen_edge0_ * src_col3[x]
                             + filt_pen_edge_n1_ * src_col4[x], (destT)0);

    x++;
    worka_row[nx-1] = l_round( filt_edge2_ * src_col1[x]
                             + filt_edge1_ * src_col2[x]
                             + filt_edge0_ * src_col3[x], (destT)0);
  }

//  worka_.print_all(vcl_cout);
  // Now perform vertical smoothing
  for (unsigned int y=2;y<ny-2;y++)
  {
    destT* dest_row = dest_im + y*dest_ystep;

    const destT* worka_row3  = worka_im + y*work_ystep;
    const destT* worka_row2  = worka_row3 - work_ystep;
    const destT* worka_row1  = worka_row3 - 2 * work_ystep;
    const destT* worka_row4  = worka_row3 + work_ystep;
    const destT* worka_row5  = worka_row3 + 2 * work_ystep;

    for (unsigned int x=0; x<nx; x++)
      dest_row[x] = l_round(  filt2_ * worka_row1[x]
                            + filt1_ * worka_row2[x]
                            + filt0_ * worka_row3[x]
                            + filt1_ * worka_row4[x]
                            + filt2_ * worka_row5[x], (destT)0);
  }

  // Now deal with edge effects :
  //
  const destT* worka_row_bottom_1 = worka_im;
  const destT* worka_row_bottom_2 = worka_row_bottom_1 + work_ystep;
  const destT* worka_row_bottom_3 = worka_row_bottom_1 + 2 * work_ystep;
  const destT* worka_row_bottom_4 = worka_row_bottom_1 + 3 * work_ystep;

  const destT* worka_row_top_5  = worka_im + (ny-1) * work_ystep;
  const destT* worka_row_top_4  = worka_row_top_5 - work_ystep;
  const destT* worka_row_top_3  = worka_row_top_5 - 2 * work_ystep;
  const destT* worka_row_top_2  = worka_row_top_5 - 3 * work_ystep;

  destT* dest_row_top      = dest_im + (ny-1) * work_ystep;
  destT* dest_row_next_top  = dest_row_top - work_ystep;
  destT* dest_row_bottom    = dest_im;
  destT* dest_row_next_bottom  = dest_row_bottom + work_ystep;

  for (unsigned int x=0;x<nx;x++)
  {
    dest_row_top[x] = l_round(  filt_edge0_ * worka_row_top_5[x]
                              + filt_edge1_ * worka_row_top_4[x]
                              + filt_edge2_ * worka_row_top_3[x], (destT)0);

    dest_row_next_top[x] = l_round( filt_pen_edge2_ * worka_row_top_2[x]
                                  + filt_pen_edge1_ * worka_row_top_3[x]
                                  + filt_pen_edge0_ * worka_row_top_4[x]
                                  + filt_pen_edge_n1_ * worka_row_top_5[x], (destT)0);

    dest_row_next_bottom[x] = l_round(  filt_pen_edge2_ * worka_row_bottom_4[x]
                                      + filt_pen_edge1_ * worka_row_bottom_3[x]
                                      + filt_pen_edge0_ * worka_row_bottom_2[x]
                                      + filt_pen_edge_n1_ * worka_row_bottom_1[x], (destT)0); 

    dest_row_bottom[x] = l_round(   filt_edge2_ * worka_row_bottom_3[x]
                                  + filt_edge1_ * worka_row_bottom_2[x]
                                  + filt_edge0_ * worka_row_bottom_1[x], (destT)0);
  }

//  dest_im.print_all(vcl_cout);
}


//: Set the Scale step
template <class srcT, class destT>
void mil_algo_gaussian_filter<srcT, destT>::set_width(double sigma)
{
  sigma_ = sigma;
  const double z = 1/(vcl_sqrt(2.0)*sigma);
  filt0_ = mbl_erf(0.5 * z) - mbl_erf(-0.5 * z);
  filt1_ = mbl_erf(1.5 * z) - mbl_erf(0.5 * z);
  filt2_ = mbl_erf(2.5 * z) - mbl_erf(1.5 * z);

  double five_tap_total = 2*(filt2_ + filt1_) + filt0_;
//  double four_tap_total = filt2_ + 2*(filt1_) + filt0_;
//  double three_tap_total = filt2_ + filt1_ + filt0_;

//  Calculate 3 tap half Gaussian filter assuming constant edge extension
  filt_edge0_ = (filt0_ + filt1_ + filt2_) / five_tap_total;
  filt_edge1_ = filt1_ / five_tap_total;
  filt_edge2_ = filt2_ / five_tap_total;
#if 0
  filt_edge0_ = 1.0;
  filt_edge1_ = 0.0;
  filt_edge2_ = 0.0;
#endif
//  Calculate 4 tap skewed Gaussian filter assuming constant edge extension
  filt_pen_edge_n1_ = (filt1_+filt2_) / five_tap_total;
  filt_pen_edge0_ = filt0_ / five_tap_total;
  filt_pen_edge1_ = filt1_ / five_tap_total;
  filt_pen_edge2_ = filt2_ / five_tap_total;

//  Calculate 5 tap Gaussian filter
  filt0_ = filt0_ / five_tap_total;
  filt1_ = filt1_ / five_tap_total;
  filt2_ = filt2_ / five_tap_total;
}


#endif // mil_algo_gaussian_filter_txx_
