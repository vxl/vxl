// This is mul/mil/mil_gaussian_pyramid_builder_2d_general.txx
#ifndef mil_gaussian_pyramid_builder_2d_general_txx_
#define mil_gaussian_pyramid_builder_2d_general_txx_
//:
// \file
// \brief Build Gaussian image pyramids at any scale separation
// \author Ian Scott

#include "mil_gaussian_pyramid_builder_2d_general.h"

#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <mil/mil_bilin_interp_2d.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_image_pyramid.h>
#include <mbl/mbl_gamma.h>
#include <vsl/vsl_indent.h>

//=======================================================================

template <class T>
mil_gaussian_pyramid_builder_2d_general<T>::mil_gaussian_pyramid_builder_2d_general()
{
  set_scale_step(2.0);
}


//: Construct with given scale_step
template <class T>
mil_gaussian_pyramid_builder_2d_general<T>::mil_gaussian_pyramid_builder_2d_general(double scale_step)
{
  set_scale_step(scale_step);
}

//=======================================================================

template <class T>
mil_gaussian_pyramid_builder_2d_general<T>::~mil_gaussian_pyramid_builder_2d_general()
{
}

template <class T>
void mil_gaussian_pyramid_builder_2d_general<T>::gauss_reduce(
           mil_image_2d_of<T>& dest_im,
           const mil_image_2d_of<T>& src_im) const
{
  int src_nx = src_im.nx();
  int src_ny = src_im.ny();
  int dest_nx = dest_im.nx();
  int dest_ny = dest_im.ny();
  int ystep = src_im.ystep();
  int n_planes = src_im.n_planes();


  // Reduce plane-by-plane

  // use n-1 because we are trying to align inter-pixel spaces, so that the
  // centre pixel is most accurately registered despite buildup of rounding errors.
  double init_x = 0.5 * (src_nx - 1 - (dest_nx-1)*scale_step());
  double init_y = 0.5 * (src_ny - 1 - (dest_ny-1)*scale_step());

  T* worka_im = worka_.plane(0);
  T* workb_im = workb_.plane(0);
  int work_ystep = worka_.ystep();
  assert (work_ystep == workb_.ystep());


  for (int i=0;i<n_planes;++i)
    gauss_reduce(dest_im.plane(i),dest_im.ystep(),
                 src_im.plane(i),src_nx,src_ny,dest_nx,dest_ny,ystep,
                 worka_im, workb_im, work_ystep,
                 this);
#if 0
  vsl_indent_inc(vcl_cout);
  vcl_cout << vsl_indent() << "Work image B\n";
  workb_.print_all(vcl_cout);
  vsl_indent_dec(vcl_cout);
#endif

    // Sort out world to image transformation for destination image
  mil_transform_2d scaling;
  scaling.set_zoom_only(1/scale_step(),-init_x,-init_y);
  dest_im.setWorld2im(scaling * src_im.world2im());
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
void mil_gaussian_pyramid_builder_2d_general<T>::gauss_reduce(
           T* dest_im, int dest_ystep,
           const T* src_im,
           int src_nx, int src_ny, int dest_nx, int dest_ny, int src_ystep,
           T* worka_im, T* workb_im, int work_ystep,
           const mil_gaussian_pyramid_builder_2d_general<T>* that)
{
  // Convolve src with a 5 x 1 Gaussian filter,
  // placing result in work_


  // First perform horizontal smoothing
  for (int y=0;y<src_ny;y++)
  {
    T* worka_row = worka_im + y*work_ystep;
    const T* src_col3  = src_im + y*src_ystep;
    const T* src_col2  = src_col3 - 1;
    const T* src_col1  = src_col3 - 2;
    const T* src_col4  = src_col3 + 1;
    const T* src_col5  = src_col3 + 2;

    int x;
    int nx2 = src_nx-2;
    for (x=2;x<nx2;x++)
      worka_row[x] = l_round( that->filt2_ * src_col1[x]
                            + that->filt1_ * src_col2[x]
                            + that->filt0_ * src_col3[x]
                            + that->filt1_ * src_col4[x]
                            + that->filt2_ * src_col5[x], (T)0);

    // Now deal with edge effects :
    worka_row[0] = l_round( that->filt_edge0_ * src_col3[0]
                          + that->filt_edge1_ * src_col4[0]
                          + that->filt_edge2_ * src_col5[0], (T)0);

    worka_row[1] = l_round( that->filt_pen_edge_n1_ * src_col2[1]
                          + that->filt_pen_edge0_ * src_col3[1]
                          + that->filt_pen_edge1_ * src_col4[1]
                          + that->filt_pen_edge2_ * src_col5[1], (T)0);

    worka_row[src_nx-2] = l_round( that->filt_pen_edge2_ * src_col1[x]
                                 + that->filt_pen_edge1_ * src_col2[x]
                                 + that->filt_pen_edge0_ * src_col3[x]
                                 + that->filt_pen_edge_n1_ * src_col4[x], (T)0);

    x++;
    worka_row[src_nx-1] = l_round( that->filt_edge2_ * src_col1[x]
                                 + that->filt_edge1_ * src_col2[x]
                                 + that->filt_edge0_ * src_col3[x], (T)0);
  }

//  worka_.print_all(vcl_cout);
  // Now perform vertical smoothing
  for (int y=2;y<src_ny-2;y++)
  {
    T* workb_row = workb_im + y*work_ystep;

    const T* worka_row3  = worka_im + y*work_ystep;
    const T* worka_row2  = worka_row3 - work_ystep;
    const T* worka_row1  = worka_row3 - 2 * work_ystep;
    const T* worka_row4  = worka_row3 + work_ystep;
    const T* worka_row5  = worka_row3 + 2 * work_ystep;

    for (int x=0; x<src_nx; x++)
      workb_row[x] = l_round( that->filt2_ * worka_row1[x]
                            + that->filt1_ * worka_row2[x]
                            + that->filt0_ * worka_row3[x]
                            + that->filt1_ * worka_row4[x]
                            + that->filt2_ * worka_row5[x], (T)0);
  }

  // Now deal with edge effects :
  //
  const T* worka_row_bottom_1 = worka_im;
  const T* worka_row_bottom_2 = worka_row_bottom_1 + work_ystep;
  const T* worka_row_bottom_3 = worka_row_bottom_1 + 2 * work_ystep;
  const T* worka_row_bottom_4 = worka_row_bottom_1 + 3 * work_ystep;

  const T* worka_row_top_5  = worka_im + (src_ny-1) * work_ystep;
  const T* worka_row_top_4  = worka_row_top_5 - work_ystep;
  const T* worka_row_top_3  = worka_row_top_5 - 2 * work_ystep;
  const T* worka_row_top_2  = worka_row_top_5 - 3 * work_ystep;

  T* workb_row_top      = workb_im + (src_ny-1) * work_ystep;
  T* workb_row_next_top  = workb_row_top - work_ystep;
  T* workb_row_bottom    = workb_im;
  T* workb_row_next_bottom  = workb_row_bottom + work_ystep;

  for (int x=0;x<src_nx;x++)
  {
    workb_row_top[x] = l_round( that->filt_edge0_ * worka_row_top_5[x]
                              + that->filt_edge1_ * worka_row_top_4[x]
                              + that->filt_edge2_ * worka_row_top_3[x], (T)0);

    workb_row_next_top[x] = l_round( that->filt_pen_edge2_ * worka_row_top_2[x]
                                   + that->filt_pen_edge1_ * worka_row_top_3[x]
                                   + that->filt_pen_edge0_ * worka_row_top_4[x]
                                   + that->filt_pen_edge_n1_ * worka_row_top_5[x], (T)0);

    workb_row_next_bottom[x] = l_round( that->filt_pen_edge2_ * worka_row_bottom_4[x]
                                      + that->filt_pen_edge1_ * worka_row_bottom_3[x]
                                      + that->filt_pen_edge0_ * worka_row_bottom_2[x]
                                      + that->filt_pen_edge_n1_ * worka_row_bottom_1[x], (T)0);

    workb_row_bottom[x] = l_round( that->filt_edge2_ * worka_row_bottom_3[x]
                                 + that->filt_edge1_ * worka_row_bottom_2[x]
                                 + that->filt_edge0_ * worka_row_bottom_1[x], (T)0);
  }

//  workb_.print_all(vcl_cout);

  T* dest_row = dest_im;

//  assert (dest_nx*scale_step() <= src_nx && dest_ny*that->scale_step() <= src_ny);

  const double init_x = 0.5 * (src_nx-1 - (dest_nx-1)*that->scale_step());
  double dy = 0.5 * (src_ny -1 - (dest_ny-1)*that->scale_step());
  for (int yi=0; yi<dest_ny; yi++)
  {
    double dx=init_x;
    for (int xi=0; xi<dest_nx; xi++)
    {
      dest_row[xi] = l_round (mil_safe_extend_bilin_interp_2d(dx, dy,
                              workb_im,  src_nx, src_ny, work_ystep), (T)0);
      dx += that->scale_step_;
    }
    dy+= that->scale_step_;
    dest_row += dest_ystep;
  }
}


//: Set the Scale step
template <class T>
void mil_gaussian_pyramid_builder_2d_general<T>::set_scale_step(double scaleStep)
{
  assert(scaleStep> 1.0  && scaleStep<=2.0);
  scale_step_ = scaleStep;
// This arrangement gives close to a 1-5-8-5-1 filter for scalestep of 2.0;
// and 0-0-1-0-0 for a scale step close to 1.0;
  double z = 1/vcl_sqrt(2.0*(scaleStep-1.0));
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
};

//=======================================================================

template <class T>
void mil_gaussian_pyramid_builder_2d_general<T>::build(
                  mil_image_pyramid& im_pyr,
                  const mil_image& im) const
{
  //  Require image mil_image_2d_of<T>
  assert(im.is_a()==worka_.is_a());

  const mil_image_2d_of<T>& base_image = (const mil_image_2d_of<T>&) im;

  int nx = base_image.nx();
  int ny = base_image.ny();

  // Compute number of levels to pyramid so that top is no less
  // than minXSize_ x minYSize_
  double s = scale_step();
  int max_levels = 1;
  while (((unsigned int)(nx/s+0.5)>=min_x_size()) &&
         ((unsigned int)(ny/s+0.5)>=min_y_size()))
  {
    max_levels++;
    s *= scale_step();
  }

  if (max_levels>maxLevels())
    max_levels=maxLevels();

  worka_.resize(nx,ny);
  workb_.resize(nx,ny);

  // Set up image pyramid
  checkPyr(im_pyr,max_levels);

  mil_image_2d_of<T>& im0 = (mil_image_2d_of<T>&) im_pyr(0);

  // Shallow copy of part of base_image
  im0.setToWindow(base_image,0,nx-1,0,ny-1);

  s = scale_step();
  for (int i=1;i<max_levels;i++)
  {
    mil_image_2d_of<T>& im_i0 = (mil_image_2d_of<T>&) im_pyr(i);
    mil_image_2d_of<T>& im_i1 = (mil_image_2d_of<T>&) im_pyr(i-1);
    if (im_i0.n_planes()!=im_i1.n_planes())
      im_i0.set_n_planes(im_i1.n_planes());
    im_i0.resize((int) (nx/s+0.5), (int) (ny/s+0.5));

    s*=scale_step();
    gauss_reduce(im_i0,im_i1);
  }

  // Estimate width of pixels in base image
  vgl_point_2d<double>  c0(0.5*(nx-1),0.5*(ny-1));
  vgl_point_2d<double>  c1 = c0 + vgl_vector_2d<double> (1,1);
  mil_transform_2d im2world = base_image.world2im().inverse();
  vgl_vector_2d<double>  dw = im2world(c1) - im2world(c0);

  double base_pixel_width = vcl_sqrt(0.5*(dw.x()*dw.x() + dw.y()*dw.y()));

  im_pyr.setWidths(base_pixel_width,scale_step());
}


//=======================================================================
//: Extend pyramid
// The first layer of the pyramid must already be set.
template<class T>
void mil_gaussian_pyramid_builder_2d_general<T>::extend(mil_image_pyramid& image_pyr) const
{
  //  Require image mil_image_2d_of<T>
  assert(image_pyr(0).is_a() == worka_.is_a());

  assert(image_pyr.scale_step() == scale_step());

  const int nx = image_pyr(0).nx();
  const int ny = image_pyr(0).ny();

  // Compute number of levels to pyramid so that top is no less
  // than 5 x 5
  double s = scale_step();
  int max_levels = 1;
  while (((unsigned int)(nx/s+0.5)>=min_x_size()) &&
         ((unsigned int)(ny/s+0.5)>=min_y_size()))
  {
     max_levels++;
     s*=scale_step();
  }

  if (max_levels>maxLevels())
      max_levels=maxLevels();

  worka_.resize(nx,ny);
  workb_.resize(nx,ny);

  // Set up image pyramid
  int oldsize = image_pyr.n_levels();
  if (oldsize<max_levels) // only extend, if it isn't already tall enough
  {
    image_pyr.data().resize(max_levels);

    s = vcl_pow(scale_step(), oldsize);
    for (int i=oldsize;i<max_levels;i++)
    {
      image_pyr.data()[i] = new mil_image_2d_of<T>;
      mil_image_2d_of<T>& im_i0 = (mil_image_2d_of<T>&) image_pyr(i);
      mil_image_2d_of<T>& im_i1 = (mil_image_2d_of<T>&) image_pyr(i-1);
      if (im_i0.n_planes()!=im_i1.n_planes())
        im_i0.set_n_planes(im_i1.n_planes());
      im_i0.resize((int)(nx/s+0.5),(int)(ny/s+0.5));

      s*=scale_step();
      gauss_reduce(im_i0,im_i1);
    }
  }
}


//=======================================================================

template <class T>
bool mil_gaussian_pyramid_builder_2d_general<T>::is_class(vcl_string const& s) const
{
  return s==mil_gaussian_pyramid_builder_2d_general<T>::is_a() || mil_gaussian_pyramid_builder_2d<T>::is_class(s);
}

//=======================================================================

template <class T>
short mil_gaussian_pyramid_builder_2d_general<T>::version_no() const
{
  return 1;
}

//=======================================================================

template <class T>
mil_image_pyramid_builder* mil_gaussian_pyramid_builder_2d_general<T>::clone() const
{
  return new mil_gaussian_pyramid_builder_2d_general(*this);
}

//=======================================================================

template <class T>
void mil_gaussian_pyramid_builder_2d_general<T>::print_summary(vcl_ostream& os) const
{
  mil_gaussian_pyramid_builder_2d<T>::print_summary(os);
}

//=======================================================================

template <class T>
void mil_gaussian_pyramid_builder_2d_general<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  mil_gaussian_pyramid_builder_2d<T>::b_write(bfs);
  vsl_b_write(bfs,scale_step());
}

//=======================================================================

template <class T>
void mil_gaussian_pyramid_builder_2d_general<T>::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  double scale;

  switch (version)
  {
  case (1):
    mil_gaussian_pyramid_builder_2d<T>::b_read(bfs);

    vsl_b_read(bfs,scale);
    set_scale_step(scale);
    break;
  default:
    vcl_cerr << "I/O ERROR: mil_gaussian_pyramid_builder_2d_general<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#undef MIL_GAUSSIAN_PYRAMID_BUILDER_2D_GENERAL_INSTANTIATE
#define MIL_GAUSSIAN_PYRAMID_BUILDER_2D_GENERAL_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string mil_gaussian_pyramid_builder_2d_general<T >::is_a() const \
{ return vcl_string("mil_gaussian_pyramid_builder_2d_general<" #T ">"); } \
template class mil_gaussian_pyramid_builder_2d_general<T >

#endif // mil_gaussian_pyramid_builder_2d_general_txx_
