// This is mul/vil2/algo/vil2_gauss_reduce.txx
#ifndef vil2_gauss_reduce_txx_
#define vil2_gauss_reduce_txx_
//: \file
//  \brief Functions to smooth and sub-sample image in one direction
//  \author Tim Cootes

#include "vil2_gauss_reduce.h"
#include <vil2/vil2_bilin_interp.h>
//: Smooth and subsample src_im to produce dest_im
//  Applies filter in x and y, then samples every other pixel.
//  work_im provides workspace
template<class T>
void vil2_gauss_reduce(const vil2_image_view<T>& src_im,
                            vil2_image_view<T>& dest_im,
                            vil2_image_view<T>& work_im)
{
  unsigned int ni = src_im.ni();
  unsigned int nj = src_im.nj();
  unsigned int n_planes = src_im.nplanes();

  // Output image size
  unsigned int ni2 = (ni+1)/2;
  unsigned int nj2 = (nj+1)/2;

  dest_im.resize(ni2,nj2,n_planes);

  if (work_im.ni()<ni2 || work_im.nj()<nj)
    work_im.resize(ni2,nj);

  // Reduce plane-by-plane
  for (unsigned int i=0;i<n_planes;++i)
  {
    // Smooth and subsample in x, result in work_im
    vil2_gauss_reduce(src_im.top_left_ptr()+i*src_im.planestep(),ni,nj,
                           src_im.istep(),src_im.jstep(),
                           work_im.top_left_ptr(),
                           work_im.istep(),work_im.jstep());

    // Smooth and subsample in y (by implicitly transposing work_im)
    vil2_gauss_reduce(work_im.top_left_ptr(),nj,ni2,
                           work_im.jstep(),work_im.istep(),
                           dest_im.top_left_ptr()+i*dest_im.planestep(),
                           dest_im.jstep(),dest_im.istep());
  }
}
//: Smooth and subsample src_im to produce dest_im
//  Applies filter in x and y, then samples every other pixel.
template<class T>
void vil2_gauss_reduce_121(const vil2_image_view<T>& src_im,
                                vil2_image_view<T>& dest_im)
{
  unsigned int ni = src_im.ni();
  unsigned int nj = src_im.nj();
  unsigned int n_planes = src_im.nplanes();

  // Output image size
  unsigned int ni2 = (ni+1)/2;
  unsigned int nj2 = (nj+1)/2;

  dest_im.resize(ni2,nj2,n_planes);

  // Reduce plane-by-plane
  for (unsigned int i=0;i<n_planes;++i)
  {
    vil2_gauss_reduce_121(src_im.top_left_ptr()+i*src_im.planestep(),ni,nj,
                               src_im.istep(),src_im.jstep(),
                               dest_im.top_left_ptr(),
                               dest_im.istep(),dest_im.jstep());
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
//  Assumes dest_im has suffient data allocated

template <class T>
void gauss_reduce_general_plane(const vil2_image_view<T>& src,
                          vil2_image_view<T>& dest,
                          vil2_image_view<T>& worka,
                          vil2_image_view<T>& workb,
                          const vil2_gauss_reduce_params &params)
{
  // Convolve src with a 5 x 1 gaussian filter,
  // placing result in worka

  // First perform horizontal smoothing
  for (int y=0;y<src.nj();y++)
  {
/*    T* worka_row = worka_im + y*work_jstep;
    const T* src_col3  = src_im + y*src_jstep;
    const T* src_col2  = src_col3 - 1;
    const T* src_col1  = src_col3 - 2;
    const T* src_col4  = src_col3 + 1;
    const T* src_col5  = src_col3 + 2;
*/
    int x;
    int ni2 = src.ni()-2;
    for (x=2;x<ni2;x++)
      worka(x,y) = l_round(  params.filt2() * src(x-2,y)
                           + params.filt1() * src(x-1,y)
                           + params.filt0() * src(x  ,y)
                           + params.filt1() * src(x+1,y)
                           + params.filt2() * src(x+2,y), (T)0);

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

    x++;
    worka(src.ni()-1,y) = l_round( params.filt_edge2() * src(src.ni()-3,y)
                                 + params.filt_edge1() * src(src.ni()-2,y)
                                 + params.filt_edge0() * src(src.ni()-1,y), (T)0);
  }

//  worka_.print_all(vcl_cout);
  // Now perform vertical smoothing
  for (int y=2;y<src.nj()-2;y++)
  {
/*    T* workb_row = workb_im + y*work_jstep;

    const T* worka_row3  = worka_im + y*work_jstep;
    const T* worka_row2  = worka_row3 - work_jstep;
    const T* worka_row1  = worka_row3 - 2 * work_jstep;
    const T* worka_row4  = worka_row3 + work_jstep;
    const T* worka_row5  = worka_row3 + 2 * work_jstep;
*/
    for (int x=0; x<src.ni(); x++)
      workb(x,y) = l_round(  params.filt2() * worka(x,y-2)
                           + params.filt1() * worka(x,y-1)
                           + params.filt0() * worka(x,  y)
                           + params.filt1() * worka(x,y+1)
                           + params.filt2() * worka(x,y+2), (T)0);
  }

  // Now deal with edge effects :
  //
/*  const T* worka_row_bottom_1 = worka_im;
  const T* worka_row_bottom_2 = worka_row_bottom_1 + work_jstep;
  const T* worka_row_bottom_3 = worka_row_bottom_1 + 2 * work_jstep;
  const T* worka_row_bottom_4 = worka_row_bottom_1 + 3 * work_jstep;

  const T* worka_row_top_5  = worka_im + (src.nj()-1) * work_jstep;
  const T* worka_row_top_4  = worka_row_top_5 - work_jstep;
  const T* worka_row_top_3  = worka_row_top_5 - 2 * work_jstep;
  const T* worka_row_top_2  = worka_row_top_5 - 3 * work_jstep;

  T* workb_row_top      = workb_im + (src.nj()-1) * work_jstep;
  T* workb_row_next_top  = workb_row_top - work_jstep;
  T* workb_row_bottom    = workb_im;
  T* workb_row_next_bottom  = workb_row_bottom + work_jstep;
*/
  for (int x=0;x<src.ni();x++)
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

 // T* dest_row = dest_im;

//  assert (dest_ni*scale_step() <= src.ni() && dest_nj*scale_step() <= src.nj());

  const double init_x = 0.5 * (src.ni()-1 - (dest.ni()-1)*params.scale_step());
  double y = 0.5 * (src.nj() -1 - (dest.nj()-1)*params.scale_step());
  for (int yi=0; yi<dest.nj(); yi++)
  {
    double x=init_x;
    for (int xi=0; xi<dest.ni(); xi++)
    {
      dest(xi,yi) = l_round (vil2_bilin_interp_safe_extend(workb, x, y), (T)0);
      x += params.scale_step();
    }
    y+= params.scale_step();
  }
}


template <class T>
void gauss_reduce_general(const vil2_image_view<T>& src_im,
                          vil2_image_view<T>& dest_im,
                          vil2_image_view<T>& worka,
                          vil2_image_view<T>& workb,
                          const vil2_gauss_reduce_params &params)
{
  int src_ni = src_im.ni();
  int src_nj = src_im.nj();
  int dest_ni = dest_im.ni();
  int dest_nj = dest_im.nj();
  int jstep = src_im.jstep();
  int n_planes = src_im.nplanes();


  // Reduce plane-by-plane



  for (int p=0;p<n_planes;++p)
    gauss_reduce_general_plane(src_im, dest_im, worka, workb, params);
#if 0
  vsl_indent_inc(vcl_cout);
  vcl_cout << vsl_indent() << "Work image B\n";
  workb_.print_all(vcl_cout);
  vsl_indent_dec(vcl_cout);
#endif

}


#undef VIL2_GAUSS_REDUCE_INSTANTIATE
#define VIL2_GAUSS_REDUCE_INSTANTIATE(T) \
template void vil2_gauss_reduce(const vil2_image_view<T >& src, \
                                     vil2_image_view<T >& dest, \
                                     vil2_image_view<T >& work_im); \
template void vil2_gauss_reduce_121(const vil2_image_view<T >& src, \
                                         vil2_image_view<T >& dest); \
template void gauss_reduce_general(const vil2_image_view<T >& src_im, \
                                   vil2_image_view<T >& dest_im, \
                                   vil2_image_view<T >& worka, \
                                   vil2_image_view<T >& workb, \
                                   const vil2_gauss_reduce_params& params);

#endif // vil2_gauss_reduce_txx_
