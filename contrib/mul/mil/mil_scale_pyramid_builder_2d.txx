// This is mul/mil/mil_scale_pyramid_builder_2d.txx
#ifndef mil_scale_pyramid_builder_2d_txx_
#define mil_scale_pyramid_builder_2d_txx_
//:
// \file
// \brief Build Gaussian image pyramids at any scale separation
// \author Ian Scott

#include "mil_scale_pyramid_builder_2d.h"

#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <mil/mil_bilin_interp_2d.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_image_pyramid.h>
#include <vsl/vsl_indent.h>
#include <vcl_cmath.h>

//=======================================================================

template <class T>
mil_scale_pyramid_builder_2d<T>::mil_scale_pyramid_builder_2d()
: max_levels_(99)
{
  set_scale_step(2.0);
  set_min_size(5, 5);
}

//: Construct with given scale_step
template <class T>
mil_scale_pyramid_builder_2d<T>::mil_scale_pyramid_builder_2d(double scale_step)
{
  set_scale_step(scale_step);
}

//=======================================================================

template <class T>
mil_scale_pyramid_builder_2d<T>::~mil_scale_pyramid_builder_2d()
{
}

template <class T>
void mil_scale_pyramid_builder_2d<T>::scale_reduce(
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


  for (int i=0;i<n_planes;++i)
    scale_reduce(dest_im.plane(i),dest_im.ystep(),
                 src_im.plane(i),src_nx,src_ny,dest_nx,dest_ny,ystep);
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
void mil_scale_pyramid_builder_2d<T>::scale_reduce(
           T* dest_im, int dest_ystep,
           const T* src_im,
           int src_nx, int src_ny, int dest_nx, int dest_ny, int src_ystep) const
{
  T* dest_row = dest_im;

  const double init_x = 0.5 * (src_nx-1 - (dest_nx-1)*scale_step());
  double y = 0.5 * (src_ny -1 - (dest_ny-1)*scale_step());
  for (int yi=0; yi<dest_ny; yi++)
  {
    double x=init_x;
    for (int xi=0; xi<dest_nx; xi++)
    {
      dest_row[xi] = l_round (mil_safe_extend_bilin_interp_2d(x, y,
          src_im,  src_nx, src_ny, src_ystep), (T)0);
      x += scale_step_;
    }
    y+= scale_step_;
    dest_row += dest_ystep;
  }
}


//: Set the Scale step
template <class T>
void mil_scale_pyramid_builder_2d<T>::set_scale_step(double scaleStep)
{
  assert(scaleStep> 1.0  && scaleStep<=2.0);
  scale_step_ = scaleStep;
}


//=======================================================================
//: Deletes all data in im_pyr
template<class T>
void mil_scale_pyramid_builder_2d<T>::emptyPyr(mil_image_pyramid& im_pyr) const
{
  for (int i=0; i<im_pyr.n_levels();++i)
    delete im_pyr.data()[i];
}

//=======================================================================
//: Checks pyramid has at least n levels
template<class T>
void mil_scale_pyramid_builder_2d<T>::checkPyr(mil_image_pyramid& im_pyr,  int n_levels) const
{
  const int got_levels = im_pyr.n_levels();
  if (got_levels >= n_levels) //&& im_pyr(0).is_a()==work_im_.is_a())
  {
    if (im_pyr.n_levels()==n_levels) return;
    else
    {
      for (int i=n_levels;i<got_levels;++i)
        delete im_pyr.data()[i];
    }
    im_pyr.data().resize(n_levels);
    return;
  }

  im_pyr.data().resize(n_levels);
  emptyPyr(im_pyr);

  for (int i=0;i<n_levels;++i)
    im_pyr.data()[i] = new mil_image_2d_of<T>;
}

//=======================================================================

template <class T>
void mil_scale_pyramid_builder_2d<T>::build(
                  mil_image_pyramid& im_pyr,
                  const mil_image& im) const
{
  const mil_image_2d_of<T>& base_image = (const mil_image_2d_of<T>&) im;

  int nx = base_image.nx();
  int ny = base_image.ny();

  // Compute number of levels to pyramid so that top is no less
  // than minXSize_ x minYSize_
  double s = scale_step();
  int max_levels = 1;
  while ( ((unsigned int)(nx/s+0.5)>=min_x_size_)
          &&
          ((unsigned int)(ny/s+0.5)>=min_y_size_)
        )
  {
    max_levels++;
    s *= scale_step();
  }

  if (max_levels>maxLevels())
    max_levels=maxLevels();


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
    scale_reduce(im_i0,im_i1);
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
void mil_scale_pyramid_builder_2d<T>::extend(mil_image_pyramid& image_pyr) const
{
  assert(image_pyr.scale_step() == scale_step());

  const int nx = image_pyr(0).nx();
  const int ny = image_pyr(0).ny();

  // Compute number of levels to pyramid so that top is no less
  // than 5 x 5
  double s = scale_step();
  int max_levels = 1;
  while (((unsigned int)(nx/s+0.5)>=min_x_size_) &&
         ((unsigned int)(ny/s+0.5)>=min_y_size_))
  {
     max_levels++;
     s*=scale_step();
  }

  if (max_levels>maxLevels())
      max_levels=maxLevels();


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
      scale_reduce(im_i0,im_i1);
    }
  }
}

//=======================================================================
//: Define maximum number of levels to build
//  Limits levels built in subsequent calls to build()
template<class T>
void mil_scale_pyramid_builder_2d<T>::setMaxLevels(int max_l)
{
  if (max_l<1)
  {
    vcl_cerr<<"mil_scale_pyramid_builder_2d<T>::setMaxLevels() parameter is "
            <<max_l<<", must be >=1\n";
    vcl_abort();
  }
  max_levels_ = max_l;
}

//: Get the current maximum number levels allowed
template<class T>
int mil_scale_pyramid_builder_2d<T>::maxLevels() const
{
  return max_levels_;
}


//=======================================================================
//: Create new (empty) pyramid on heap
//  Caller responsible for its deletion
template<class T>
mil_image_pyramid* mil_scale_pyramid_builder_2d<T>::newImagePyramid() const
{
  return new mil_image_pyramid;
}


//=======================================================================

template <class T>
bool mil_scale_pyramid_builder_2d<T>::is_class(vcl_string const& s) const
{
  return s==mil_scale_pyramid_builder_2d<T>::is_a() || mil_scale_pyramid_builder_2d<T>::is_class(s);
}

//=======================================================================

template <class T>
short mil_scale_pyramid_builder_2d<T>::version_no() const
{
  return 1;
}

//=======================================================================

template <class T>
mil_image_pyramid_builder* mil_scale_pyramid_builder_2d<T>::clone() const
{
  return new mil_scale_pyramid_builder_2d(*this);
}

//=======================================================================

template <class T>
void mil_scale_pyramid_builder_2d<T>::print_summary(vcl_ostream& /*os*/) const
{
  vcl_cerr << "mil_scale_pyramid_builder_2d<T>::print_summary() NYI\n";
}

//=======================================================================

template <class T>
void mil_scale_pyramid_builder_2d<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,scale_step());
}

//=======================================================================

template <class T>
void mil_scale_pyramid_builder_2d<T>::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  double scale;

  switch (version)
  {
  case (1):
    vsl_b_read(bfs,scale);
    set_scale_step(scale);
    break;
  default:
    vcl_cerr << "I/O ERROR: mil_scale_pyramid_builder_2d<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#undef MIL_SCALE_PYRAMID_BUILDER_2D_INSTANTIATE
#define MIL_SCALE_PYRAMID_BUILDER_2D_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string mil_scale_pyramid_builder_2d<T >::is_a() const \
{ return vcl_string("mil_scale_pyramid_builder_2d<" #T ">"); } \
template class mil_scale_pyramid_builder_2d<T >

#endif // mil_scale_pyramid_builder_2d_txx_
