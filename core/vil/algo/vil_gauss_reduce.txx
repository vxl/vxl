// This is core/vil/algo/vil_gauss_reduce.txx
#ifndef vil_gauss_reduce_txx_
#define vil_gauss_reduce_txx_
//:
// \file
// \brief Functions to smooth and sub-sample image in one direction
// \author Tim Cootes

#include "vil_gauss_reduce.h"
#include <vcl_cassert.h>
#include <vil/vil_bilin_interp.h>
#include <vil/vil_plane.h>
#include <vil/vil_convert.h>

//: Smooth and subsample src_im to produce dest_im
//  Applies filter in x and y, then samples every other pixel.
//  work_im provides workspace
template<class T>
void vil_gauss_reduce(const vil_image_view<T>& src_im,
                      vil_image_view<T>& dest_im,
                      vil_image_view<T>& work_im)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned n_planes = src_im.nplanes();

  // Output image size
  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;

  dest_im.set_size(ni2,nj2,n_planes);

  if (work_im.ni()<ni2 || work_im.nj()<nj)
    work_im.set_size(ni2,nj);

  // Reduce plane-by-plane
  for (unsigned int i=0;i<n_planes;++i)
  {
    // Smooth and subsample in x, result in work_im
    vil_gauss_reduce_1plane(src_im.top_left_ptr()+i*src_im.planestep(),ni,nj,
                            src_im.istep(),src_im.jstep(),
                            work_im.top_left_ptr(),
                            work_im.istep(),work_im.jstep());

    // Smooth and subsample in y (by implicitly transposing work_im)
    vil_gauss_reduce_1plane(work_im.top_left_ptr(),nj,ni2,
                            work_im.jstep(),work_im.istep(),
                            dest_im.top_left_ptr()+i*dest_im.planestep(),
                            dest_im.jstep(),dest_im.istep());
  }
}

//: Smooth and subsample src_im to produce dest_im (2/3 size)
//  Applies filter in x and y, then samples every other pixel.
//  work_im provides workspace
template<class T>
void vil_gauss_reduce_2_3(const vil_image_view<T>& src_im,
                          vil_image_view<T>& dest_im,
                          vil_image_view<T>& work_im)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned n_planes = src_im.nplanes();

  // Output image size
  unsigned ni2 = (2*ni+1)/3;
  unsigned nj2 = (2*nj+1)/3;

  dest_im.set_size(ni2,nj2,n_planes);

  if (work_im.ni()<ni2 || work_im.nj()<nj)
    work_im.set_size(ni2,nj);

  // Reduce plane-by-plane
  for (unsigned int i=0;i<n_planes;++i)
  {
    // Smooth and subsample in x, result in work_im
    vil_gauss_reduce_2_3_1plane(src_im.top_left_ptr()+i*src_im.planestep(),ni,nj,
                                src_im.istep(),src_im.jstep(),
                                work_im.top_left_ptr(),
                                work_im.istep(),work_im.jstep());

    // Smooth and subsample in y (by implicitly transposing work_im)
    vil_gauss_reduce_2_3_1plane(work_im.top_left_ptr(),nj,ni2,
                                work_im.jstep(),work_im.istep(),
                                dest_im.top_left_ptr()+i*dest_im.planestep(),
                                dest_im.jstep(),dest_im.istep());
  }
}

//: Smooth and subsample src_im to produce dest_im
//  Applies filter in x and y, then samples every other pixel.
template<class T>
void vil_gauss_reduce_121(const vil_image_view<T>& src_im,
                          vil_image_view<T>& dest_im)
{
  unsigned int ni = src_im.ni();
  unsigned int nj = src_im.nj();
  unsigned int n_planes = src_im.nplanes();

  // Output image size
  unsigned int ni2 = (ni+1)/2;
  unsigned int nj2 = (nj+1)/2;

  dest_im.set_size(ni2,nj2,n_planes);

  // Reduce plane-by-plane
  for (unsigned int i=0;i<n_planes;++i)
  {
    vil_gauss_reduce_121_1plane(src_im.top_left_ptr() + i*src_im.planestep(),
                                ni, nj,
                                src_im.istep(),src_im.jstep(),
                                dest_im.top_left_ptr() + i*dst_im.planestep(),
                                dest_im.istep(), dest_im.jstep());
  }
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

template <class T>
void vil_gauss_reduce_general_plane(const vil_image_view<T>& src,
                                    vil_image_view<T>& dest,
                                    vil_image_view<T>& worka,
                                    vil_image_view<T>& workb,
                                    const vil_gauss_reduce_params &params)
{
  assert(src.ni() >= 5 && src.nj() >= 5);
  // Convolve src with a 5 x 1 gaussian filter,
  // placing result in worka

  // First perform horizontal smoothing
  for (unsigned y=0;y<src.nj();y++)
  {
    unsigned ni2 = src.ni()-2;
    for (unsigned x=2;x<ni2;++x)
      worka(x,y) = l_round(  params.filt2() * (src(x-2,y) + src(x+2,y))
                           + params.filt1() * (src(x-1,y) + src(x+1,y))
                           + params.filt0() *  src(x  ,y),
                           T(0));

    // Now deal with edge effects :
    worka(0,y) = l_round( params.filt_edge0() * src(0,y)
                        + params.filt_edge1() * src(1,y)
                        + params.filt_edge2() * src(2,y), (T)0);

    worka(1,y) = l_round( params.filt_pen_edge_n1() * src(0,y)
                        + params.filt_pen_edge0() * src(1,y)
                        + params.filt_pen_edge1() * src(2,y)
                        + params.filt_pen_edge2() * src(3,y), (T)0);

    worka(src.ni()-2,y) = l_round( params.filt_pen_edge2() * src(src.ni()-4,y)
                                 + params.filt_pen_edge1() * src(src.ni()-3,y)
                                 + params.filt_pen_edge0() * src(src.ni()-2,y)
                                 + params.filt_pen_edge_n1() * src(src.ni()-1,y), (T)0);

    worka(src.ni()-1,y) = l_round( params.filt_edge2() * src(src.ni()-3,y)
                                 + params.filt_edge1() * src(src.ni()-2,y)
                                 + params.filt_edge0() * src(src.ni()-1,y), (T)0);
  }

//  worka_.print_all(vcl_cout);
  // Now perform vertical smoothing
  for (unsigned int y=2;y+2<src.nj();++y)
  {
    for (unsigned int x=0; x<src.ni(); x++)
      workb(x,y) = l_round(  params.filt2() *(worka(x,y-2) + worka(x,y+2))
                           + params.filt1() *(worka(x,y-1) + worka(x,y+1))
                           + params.filt0() * worka(x,  y),
                           (T)0);
  }

  // Now deal with edge effects :
  //
  for (unsigned x=0;x<src.ni();x++)
  {
    workb(x,src.nj()-1) = l_round( params.filt_edge0() * worka(x,src.nj()-1)
                                 + params.filt_edge1() * worka(x,src.nj()-2)
                                 + params.filt_edge2() * worka(x,src.nj()-3), (T)0);

    workb(x,src.nj()-2) = l_round( params.filt_pen_edge2() * worka(x,src.nj()-4)
                                 + params.filt_pen_edge1() * worka(x,src.nj()-3)
                                 + params.filt_pen_edge0() * worka(x,src.nj()-2)
                                 + params.filt_pen_edge_n1() * worka(x,src.nj()-1), (T)0);

    workb(x,1) = l_round( params.filt_pen_edge2() * worka(x,3)
                        + params.filt_pen_edge1() * worka(x,2)
                        + params.filt_pen_edge0() * worka(x,1)
                        + params.filt_pen_edge_n1() * worka(x,0), (T)0);

    workb(x,0) = l_round( params.filt_edge2() * worka(x,2)
                        + params.filt_edge1() * worka(x,1)
                        + params.filt_edge0() * worka(x,0), (T)0);
  }

//  workb_.print_all(vcl_cout);


//  assert (dest_ni*scale_step() <= src.ni() && dest_nj*scale_step() <= src.nj());

  const double init_x = 0.5 * (src.ni()-1 - (dest.ni()-1)*params.scale_step());
  double yd = 0.5 * (src.nj() -1 - (dest.nj()-1)*params.scale_step());
  for (unsigned int yi=0; yi<dest.nj(); yi++)
  {
    double xd=init_x;
    for (unsigned int xi=0; xi<dest.ni(); xi++)
    {
      dest(xi,yi) = l_round (vil_bilin_interp_safe_extend(workb, xd, yd), T(0));
      xd += params.scale_step();
    }
    yd+= params.scale_step();
  }
}


template <class T>
void vil_gauss_reduce_general(const vil_image_view<T>& src,
                              vil_image_view<T>& dest,
                              vil_image_view<T>& worka,
                              vil_image_view<T>& workb,
                              const vil_gauss_reduce_params &params)
{
  if (worka.ni() < src.ni() || worka.nj() < src.nj())
    worka.set_size(src.ni(), src.nj());
  if (workb.ni() < src.ni() || workb.nj() < src.nj())
    workb.set_size(src.ni(), src.nj());
  dest.set_size((unsigned) (src.ni()/params.scale_step()+0.5),
    (unsigned) (src.nj()/params.scale_step()+0.5), src.nplanes());

  // Reduce plane-by-plane
  for (unsigned p=0;p<src.nplanes();++p) {
    vil_image_view<T> src_plane  = vil_plane(src,p);
    vil_image_view<T> dest_plane = vil_plane(dest,p);
    vil_gauss_reduce_general_plane(src_plane, dest_plane, worka, workb, params);
  }
#if 0
  vsl_indent_inc(vcl_cout);
  vcl_cout << vsl_indent() << "Work image B\n";
  workb_.print_all(vcl_cout);
  vsl_indent_dec(vcl_cout);
#endif
}


template <class T>
void vil_gauss_reduce_1plane(const T* src_im,
                             unsigned src_ni, unsigned src_nj,
                             vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                             T* dest_im,
                             vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step)
{
  T* d_row = dest_im;
  const T* s_row = src_im;
  vcl_ptrdiff_t sxs2 = s_x_step*2;
  unsigned ni2 = (src_ni-3)/2;
  vil_convert_round_pixel<double,T> rounder;

  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first element of row
    double dsum = 0.071 * static_cast<double>(s_row[sxs2]) +
                  0.357 * static_cast<double>(s_row[s_x_step]) +
                  0.572 * static_cast<double>(s_row[0]);
    rounder(dsum, *d_row);

    T* d = d_row + d_x_step;
    const T* s = s_row + sxs2;
    for (unsigned x=0;x<ni2;++x)
    {
      dsum = 0.05*static_cast<double>(s[-sxs2]) + 0.25*static_cast<double>(s[-s_x_step]) +
             0.05*static_cast<double>(s[ sxs2]) + 0.25*static_cast<double>(s[ s_x_step]) +
             0.4 *static_cast<double>(s[0]);
      rounder(dsum, *d);

      d += d_x_step;
      s += sxs2;
    }
    // Set last elements of row
    dsum = 0.071 * static_cast<double>(s[-sxs2]) +
           0.357 * static_cast<double>(s[-s_x_step]) +
           0.572 * static_cast<double>(s[0]);
    rounder(dsum, *d);

    d_row += d_y_step;
    s_row += s_y_step;
  }
}


//: Smooth and subsample single plane src_im in x to produce dest_im using 121 filter in x and y
//  Smoothes with a 3x3 filter and subsamples
template <class T>
void vil_gauss_reduce_121_1plane(const T* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                                 T* dest_im,
                                 vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step)
{
  vcl_ptrdiff_t sxs2 = s_x_step*2;
  vcl_ptrdiff_t sys2 = s_y_step*2;
  T* d_row = dest_im+d_y_step;
  const T* s_row1 = src_im + s_y_step;
  const T* s_row2 = s_row1 + s_y_step;
  const T* s_row3 = s_row2 + s_y_step;
  unsigned ni2 = (src_ni-2)/2;
  unsigned nj2 = (src_nj-2)/2;
  vil_convert_round_pixel<double,T> rounder;

  for (unsigned y=0;y<nj2;++y)
  {
      // Set first element of row
      *d_row = *s_row2;
      T * d = d_row + d_x_step;
      const T* s1 = s_row1 + sxs2;
      const T* s2 = s_row2 + sxs2;
      const T* s3 = s_row3 + sxs2;
      for (unsigned x=0;x<ni2;++x)
      {
          // The following is a little inefficient - could group terms to reduce arithmetic
          double ds1 = 0.0625 * static_cast<double>(s1[-s_x_step])
                     + 0.125  * static_cast<double>(s1[0])
                     + 0.0625 * static_cast<double>(s1[s_x_step]),
                 ds2 = 0.1250 * static_cast<double>(s2[-s_x_step])
                     + 0.250  * static_cast<double>(s2[0])
                     + 0.1250 * static_cast<double>(s2[s_x_step]),
                 ds3 = 0.0625 * static_cast<double>(s3[-s_x_step])
                     + 0.125  * static_cast<double>(s3[0])
                     + 0.0625 * static_cast<double>(s3[s_x_step]);
          rounder(ds1 + ds2 + ds3, *d);

          d += d_x_step;
          s1 += sxs2;
          s2 += sxs2;
          s3 += sxs2;
      }
      // Set last elements of row
      if (src_ni&1)
        *d = *s2;

      d_row += d_y_step;
      s_row1 += sys2;
      s_row2 += sys2;
      s_row3 += sys2;
  }

  // Need to set first and last rows as well

  // Dest image should be (src_ni+1)/2 x (src_nj+1)/2
  const T* s0 = src_im;
  unsigned ni=(src_ni+1)/2;
  for (unsigned i=0;i<ni;++i)
  {
    dest_im[i]= *s0;
    s0+=sxs2;
  }

  if (src_nj&1)
  {
    unsigned yhi = (src_nj-1)/2;
    T* dest_last_row = dest_im + yhi*d_y_step;
    const T* s_last = src_im + yhi*sys2;
    for (unsigned i=0;i<ni;++i)
    {
      dest_last_row[i]= *s_last;
      s_last+=sxs2;
    }
  }
}


template <class T>
void vil_gauss_reduce_2_3_1plane(const T* src_im,
                                 unsigned src_ni, unsigned src_nj,
                                 vcl_ptrdiff_t s_x_step, vcl_ptrdiff_t s_y_step,
                                 T* dest_im, vcl_ptrdiff_t d_x_step, vcl_ptrdiff_t d_y_step)
{
  T* d_row = dest_im;
  const T* s_row = src_im;
  vcl_ptrdiff_t sxs2 = s_x_step*2,sxs3 = s_x_step*3;
  unsigned d_ni = (2*src_ni+1)/3;
  unsigned d_ni2 = d_ni/2;
  vil_convert_round_pixel<double,T> rounder;

  for (unsigned y=0;y<src_nj;++y)
  {
    // Set first elements of row
    // The 0.5 offset in the following ensures rounding
    double drow0=0.75*static_cast<double>(s_row[0]) + 0.25*static_cast<double>(s_row[s_x_step]);
    double drow_xs=0.5*static_cast<double>(s_row[s_x_step]) + 0.5*static_cast<double>(s_row[sxs2]);
    rounder(drow0,d_row[0]);
    rounder(drow_xs,d_row[d_x_step]);

    T* d = d_row + 2*d_x_step;
    const T* s = s_row + sxs3;
    for (unsigned x=1;x<d_ni2;++x)
    {
      double df= 0.2 * ( static_cast<double>(s[-s_x_step]) + static_cast<double>(s[s_x_step]) )
               + 0.6 * static_cast<double>(s[0]) ;
      rounder(df,*d);

      d += d_x_step;

      df = 0.5*(static_cast<double>(s[s_x_step])  + static_cast<double>(s[sxs2]) );
      rounder(df,*d);

      d += d_x_step;
      s += sxs3;
    }
    // Set last elements of row
    if (src_ni%3==1)
    {
      double df=0.75*static_cast<double>(s[-s_x_step]) + 0.25*static_cast<double>(s[0]);
      rounder(df,*d);
    }
    else if (src_ni%3==2)
    {
      double df = 0.2 * (static_cast<double>(s[-s_x_step]) + static_cast<double>(s[s_x_step]) )
                + 0.6 * static_cast<double>(s[0]);
      rounder(df,*d);
    }
    d_row += d_y_step;
    s_row += s_y_step;
  }
}


#undef VIL_GAUSS_REDUCE_INSTANTIATE
#define VIL_GAUSS_REDUCE_INSTANTIATE(T) \
template void vil_gauss_reduce(const vil_image_view<T >& src, \
                               vil_image_view<T >& dest, \
                               vil_image_view<T >& work_im); \
template void vil_gauss_reduce_2_3(const vil_image_view<T >& src, \
                                   vil_image_view<T >& dest, \
                                   vil_image_view<T >& work_im); \
template void vil_gauss_reduce_121(const vil_image_view<T >& src, \
                                   vil_image_view<T >& dest); \
template void vil_gauss_reduce_general(const vil_image_view<T >& src_im, \
                                       vil_image_view<T >& dest_im, \
                                       vil_image_view<T >& worka, \
                                       vil_image_view<T >& workb, \
                                       const vil_gauss_reduce_params& params)

#endif // vil_gauss_reduce_txx_
