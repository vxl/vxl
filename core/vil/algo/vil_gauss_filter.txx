// This is core/vil/algo/vil_gauss_filter.txx
#ifndef vil_gauss_filter_txx_
#define vil_gauss_filter_txx_
//:
// \file
// \brief smooth images
// \author Ian Scott

#include "vil_gauss_filter.h"
#include <vil/vil_transpose.h>
#include <vil/algo/vil_convolve_1d.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

//=======================================================================

// An optimisable rounding function
inline unsigned char  l_round(double x, unsigned char  ) { return (unsigned char )(x<0?x-0.5:x+0.5);}
inline   signed char  l_round(double x,   signed char  ) { return (  signed char )(x<0?x-0.5:x+0.5);}
inline unsigned short l_round(double x, unsigned short ) { return (unsigned short)(x<0?x-0.5:x+0.5);}
inline   signed short l_round(double x,   signed short ) { return (  signed short)(x<0?x-0.5:x+0.5);}
inline unsigned int   l_round(double x, unsigned int   ) { return (unsigned int  )(x<0?x-0.5:x+0.5);}
inline   signed int   l_round(double x,   signed int   ) { return (  signed int  )(x<0?x-0.5:x+0.5);}
inline unsigned long  l_round(double x, unsigned long  ) { return (unsigned long )(x<0?x-0.5:x+0.5);}
inline   signed long  l_round(double x,   signed long  ) { return (  signed long )(x<0?x-0.5:x+0.5);}
inline   double       l_round(double x,   double       ) { return x; }
inline    float       l_round(double x,    float       ) { return (float)x; }

//=======================================================================
//: Smooth and subsample src_im to produce dest_im
//  Applies 5 pin filter in x and y, then samples
//  every other pixel.
//  Assumes dest_im has sufficient data allocated

template <class srcT, class destT>
void vil_gauss_filter_5tap(const srcT* src_im, vcl_ptrdiff_t src_istep, vcl_ptrdiff_t src_jstep,
                           destT* dest_im, vcl_ptrdiff_t dest_istep, vcl_ptrdiff_t dest_jstep,
                           unsigned nx, unsigned ny, const vil_gauss_filter_5tap_params& params,
                           destT* work, vcl_ptrdiff_t work_jstep)
{
  assert (nx > 3 && ny > 3);
  // Convolve src with a 5 x 1 Gaussian filter,
  // placing result in work_
  // First perform horizontal smoothing
  for (unsigned int y=0;y<ny;y++)
  {
    destT* work_row = work + y*work_jstep;
    const srcT* src_col3  = src_im + y*src_jstep;
    const srcT* src_col2  = src_col3 - src_istep;
    const srcT* src_col1  = src_col3 - 2 * src_istep;
    const srcT* src_col4  = src_col3 + src_istep;
    const srcT* src_col5  = src_col3 + 2 * src_istep;

    int x;
    int nx2 = nx-2;
    for (x=2;x<nx2;x++)
      work_row[x] = l_round(  params.filt2() * src_col1[x*src_istep]
                            + params.filt1() * src_col2[x*src_istep]
                            + params.filt0() * src_col3[x*src_istep]
                            + params.filt1() * src_col4[x*src_istep]
                            + params.filt2() * src_col5[x*src_istep], (destT)0);

    // Now deal with edge effects :
    work_row[0] = l_round(  params.filt_edge0() * src_col3[0]
                          + params.filt_edge1() * src_col4[0]
                          + params.filt_edge2() * src_col5[0], (destT)0);

    work_row[1] = l_round(  params.filt_pen_edge_n1() * src_col2[src_istep]
                          + params.filt_pen_edge0() * src_col3[src_istep]
                          + params.filt_pen_edge1() * src_col4[src_istep]
                          + params.filt_pen_edge2() * src_col5[src_istep], (destT)0);

    work_row[nx-2] = l_round(  params.filt_pen_edge2() * src_col1[(nx-2)*src_istep]
                             + params.filt_pen_edge1() * src_col2[(nx-2)*src_istep]
                             + params.filt_pen_edge0() * src_col3[(nx-2)*src_istep]
                             + params.filt_pen_edge_n1() * src_col4[(nx-2)*src_istep], (destT)0);

    work_row[nx-1] = l_round(  params.filt_edge2() * src_col1[(nx-1)*src_istep]
                             + params.filt_edge1() * src_col2[(nx-1)*src_istep]
                             + params.filt_edge0() * src_col3[(nx-1)*src_istep], (destT)0);
  }

//  work_.print_all(vcl_cout);
  // Now perform vertical smoothing
  for (unsigned int y=2;y<ny-2;y++)
  {
    destT* dest_row = dest_im + y*dest_jstep;

    const destT* work_row3  = work + y*work_jstep;
    const destT* work_row2  = work_row3 - work_jstep;
    const destT* work_row1  = work_row3 - 2 * work_jstep;
    const destT* work_row4  = work_row3 + work_jstep;
    const destT* work_row5  = work_row3 + 2 * work_jstep;

    for (unsigned int x=0; x<nx; x++)
      dest_row[x*dest_istep] = l_round(  params.filt2() * work_row1[x]
                                       + params.filt1() * work_row2[x]
                                       + params.filt0() * work_row3[x]
                                       + params.filt1() * work_row4[x]
                                       + params.filt2() * work_row5[x], (destT)0);
  }

  // Now deal with edge effects :
  //
  const destT* work_row_bottom_1 = work;
  const destT* work_row_bottom_2 = work_row_bottom_1 + work_jstep;
  const destT* work_row_bottom_3 = work_row_bottom_1 + 2 * work_jstep;
  const destT* work_row_bottom_4 = work_row_bottom_1 + 3 * work_jstep;

  const destT* work_row_top_5  = work + (ny-1) * work_jstep;
  const destT* work_row_top_4  = work_row_top_5 - work_jstep;
  const destT* work_row_top_3  = work_row_top_5 - 2 * work_jstep;
  const destT* work_row_top_2  = work_row_top_5 - 3 * work_jstep;

  destT* dest_row_top      = dest_im + (ny-1) * dest_jstep;
  destT* dest_row_next_top  = dest_row_top - dest_jstep;
  destT* dest_row_bottom    = dest_im;
  destT* dest_row_next_bottom  = dest_row_bottom + dest_jstep;

  for (unsigned int x=0;x<nx;x++)
  {
    dest_row_top[x*dest_istep] = l_round(  params.filt_edge0() * work_row_top_5[x]
                                         + params.filt_edge1() * work_row_top_4[x]
                                         + params.filt_edge2() * work_row_top_3[x], (destT)0);

    dest_row_next_top[x*dest_istep] = l_round(  params.filt_pen_edge2() * work_row_top_2[x]
                                              + params.filt_pen_edge1() * work_row_top_3[x]
                                              + params.filt_pen_edge0() * work_row_top_4[x]
                                              + params.filt_pen_edge_n1()*work_row_top_5[x], (destT)0);

    dest_row_next_bottom[x*dest_istep] = l_round(  params.filt_pen_edge2() * work_row_bottom_4[x]
                                                 + params.filt_pen_edge1() * work_row_bottom_3[x]
                                                 + params.filt_pen_edge0() * work_row_bottom_2[x]
                                                 + params.filt_pen_edge_n1()*work_row_bottom_1[x], (destT)0);

    dest_row_bottom[x*dest_istep] = l_round(  params.filt_edge2() * work_row_bottom_3[x]
                                            + params.filt_edge1() * work_row_bottom_2[x]
                                            + params.filt_edge0() * work_row_bottom_1[x], (destT)0);
  }
//  dest_im.print_all(vcl_cout);
}

template <class srcT, class destT>
void vil_gauss_filter_5tap(const vil_image_view<srcT>& src_im,
                           vil_image_view<destT>& dest_im,
                           const vil_gauss_filter_5tap_params& params,
                           vil_image_view<destT>& work)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned n_planes = src_im.nplanes();
  dest_im.set_size(ni, nj, n_planes);
  work.set_size(ni,nj,1);
  assert (work.jstep() == (vcl_ptrdiff_t)ni);

  if (ni > 3 && nj > 3)
  // Reduce plane-by-plane
    for (unsigned p=0;p<n_planes;++p)
      vil_gauss_filter_5tap(&src_im(0,0,p), src_im.istep(), src_im.jstep(),
                            &dest_im(0,0,p), dest_im.istep(), dest_im.jstep(), ni,nj,
                            params, work.top_left_ptr(), work.jstep());
  else
  {
    if (ni==0 || nj==0) return;
    if (ni==1)
    {
      for (unsigned p=0;p<n_planes;++p)
        for (unsigned j=0;j<nj;++j)
          work(0,j,p) = l_round(src_im(0,j,p), destT());
    }
    else if (ni==2)
    {
      const double k0 = params.filt0()/(params.filt0() + params.filt1());
      const double k1 = params.filt1()/(params.filt0() + params.filt1());
      for (unsigned p=0;p<n_planes;++p)
        for (unsigned j=0;j<nj;++j)
        {
          work(0,j,p) = l_round(k0*src_im(0,j,p) + k1*src_im(1,j,p), destT());
          work(1,j,p) = l_round(k1*src_im(0,j,p) + k0*src_im(1,j,p), destT());
        }
    }
    else if (ni==3)
    {
      const double ke0 = params.filt0()/(params.filt0() + params.filt1());
      const double ke1 = params.filt1()/(params.filt0() + params.filt1());
      const double k0 = params.filt0()/(params.filt0() + 2.0*params.filt1());
      const double k1 = params.filt1()/(params.filt0() + 2.0*params.filt1());
      for (unsigned p=0;p<n_planes;++p)
        for (unsigned j=0;j<nj;++j)
        {
          work(0,j,p) = l_round(ke0*src_im(0,j,p) + ke1*src_im(1,j,p)                    , destT());
          work(1,j,p) = l_round(k1 *src_im(0,j,p) + k0 *src_im(1,j,p) + k1 *src_im(2,j,p), destT());
          work(2,j,p) = l_round(                    ke1*src_im(1,j,p) + ke0*src_im(2,j,p), destT());
        }
    }
    else if (ni==4)
    {
      const double ke0 = params.filt_edge0();
      const double ke1 = params.filt_edge1();
      const double ke2 = params.filt_edge2();
      const double kpn1= params.filt_pen_edge_n1();
      const double kp0 = params.filt_pen_edge0();
      const double kp1 = params.filt_pen_edge1();
      const double kp2 = params.filt_pen_edge2();
      for (unsigned p=0;p<n_planes;++p)
        for (unsigned j=0;j<nj;++j)
        {
          work(0,j,p) = l_round(ke0 *src_im(0,j,p) + ke1*src_im(1,j,p) + ke2*src_im(2,j,p)                     , destT());
          work(1,j,p) = l_round(kpn1*src_im(0,j,p) + kp0*src_im(1,j,p) + kp1*src_im(2,j,p) + kp2 *src_im(3,j,p), destT());
          work(2,j,p) = l_round(kp2 *src_im(0,j,p) + kp1*src_im(1,j,p) + kp0*src_im(2,j,p) + kpn1*src_im(3,j,p), destT());
          work(3,j,p) = l_round(                     ke2*src_im(1,j,p) + ke1*src_im(2,j,p) + ke0 *src_im(3,j,p), destT());
        }
    }
    else if (ni>4)
    {
      double kernel[5];
      kernel[0] = kernel[4] = params.filt2();
      kernel[1] = kernel[3] = params.filt1();
      kernel[2] = params.filt0();
      vil_convolve_1d(src_im,work, kernel+2, -2, +2,
                      double(), vil_convolve_trim, vil_convolve_trim);
    }
    if (nj==1)
      dest_im=work;
    else if (nj==2)
    {
      const double k0 = params.filt0()/(params.filt0() + params.filt1());
      const double k1 = params.filt1()/(params.filt0() + params.filt1());
      for (unsigned p=0;p<n_planes;++p)
        for (unsigned i=0;i<ni;++i)
        {
          dest_im(i,0,p) = l_round(k0 * work(i,0,p) + k1 * work(i,1,p), destT());
          dest_im(i,1,p) = l_round(k1 * work(i,0,p) + k0 * work(i,1,p), destT());
        }
    }
    else if (nj==3)
    {
      const double ke0 = params.filt0()/(params.filt0() + params.filt1());
      const double ke1 = params.filt1()/(params.filt0() + params.filt1());
      const double k0 = params.filt0()/(params.filt0() + 2.0*params.filt1());
      const double k1 = params.filt1()/(params.filt0() + 2.0*params.filt1());
      for (unsigned p=0;p<n_planes;++p)
        for (unsigned i=0;i<ni;++i)
        {
          dest_im(i,0,p) = l_round(ke0*work(i,0,p) + ke1*work(i,1,p)                  , destT());
          dest_im(i,1,p) = l_round(k1 *work(i,0,p) + k0 *work(i,1,p) + k1 *work(i,2,p), destT());
          dest_im(i,2,p) = l_round(                  ke1*work(i,1,p) + ke0*work(i,2,p), destT());
        }
    }
    else if (nj==4)
    {
      const double ke0 = params.filt_edge0();
      const double ke1 = params.filt_edge1();
      const double ke2 = params.filt_edge2();
      const double kpn1= params.filt_pen_edge_n1();
      const double kp0 = params.filt_pen_edge0();
      const double kp1 = params.filt_pen_edge1();
      const double kp2 = params.filt_pen_edge2();
      for (unsigned p=0;p<n_planes;++p)
        for (unsigned i=0;i<ni;++i)
        {
          dest_im(i,0,p) = l_round(ke0 *work(i,0,p) + ke1*work(i,1,p) + ke2*work(i,2,p)                   , destT());
          dest_im(i,1,p) = l_round(kpn1*work(i,0,p) + kp0*work(i,1,p) + kp1*work(i,2,p) + kp2 *work(i,3,p), destT());
          dest_im(i,2,p) = l_round(kp2 *work(i,0,p) + kp1*work(i,1,p) + kp0*work(i,2,p) + kpn1*work(i,3,p), destT());
          dest_im(i,3,p) = l_round(                   ke2*work(i,1,p) + ke1*work(i,2,p) + ke0 *work(i,3,p), destT());
        }
    }
    else if (nj>4)
    {
      double kernel[5];
      kernel[0] = kernel[4] = params.filt2();
      kernel[1] = kernel[3] = params.filt1();
      kernel[2] = params.filt0();
      vil_image_view<destT> rdest = vil_transpose(dest_im);
      vil_convolve_1d(vil_transpose(work), rdest, kernel+2, -2, +2,
                      double(), vil_convolve_trim, vil_convolve_trim);
    }
  }

#if 0
  vsl_indent_inc(vcl_cout);
  vcl_cout << vsl_indent() << "Work image B\n";
  workb_.print_all(vcl_cout);
  vsl_indent_dec(vcl_cout);
#endif
}

#undef VIL_GAUSS_FILTER_INSTANTIATE
#define VIL_GAUSS_FILTER_INSTANTIATE(srcT, destT) \
template void vil_gauss_filter_5tap(const vil_image_view<srcT >& src_im, \
                                    vil_image_view<destT >& dest_im, \
                                    const vil_gauss_filter_5tap_params& params, \
                                    vil_image_view<destT >& work)

#endif // vil_gauss_filter_txx_
