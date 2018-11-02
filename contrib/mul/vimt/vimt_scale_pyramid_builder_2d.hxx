// This is mul/vimt/vimt_scale_pyramid_builder_2d.hxx
#ifndef vimt_scale_pyramid_builder_2d_hxx_
#define vimt_scale_pyramid_builder_2d_hxx_
//:
// \file
// \brief Build Gaussian image pyramids at any scale separation
// \author Ian Scott

#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include "vimt_scale_pyramid_builder_2d.h"
#include <cassert>
#include <vil/vil_bilin_interp.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_image_pyramid.h>
#include <vsl/vsl_indent.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vimt/vimt_crop.h>

//=======================================================================

template <class T>
vimt_scale_pyramid_builder_2d<T>::vimt_scale_pyramid_builder_2d()
: max_levels_(99)
{
  set_scale_step(2.0);
  set_min_size(5, 5);
}

//: Construct with given scale_step
template <class T>
vimt_scale_pyramid_builder_2d<T>::vimt_scale_pyramid_builder_2d(double scale_step)
{
  set_scale_step(scale_step);
}

//=======================================================================

template <class T>
vimt_scale_pyramid_builder_2d<T>::~vimt_scale_pyramid_builder_2d() = default;

template <class T>
void vimt_scale_pyramid_builder_2d<T>::scale_reduce(
           vimt_image_2d_of<T>& dest_im,
           const vimt_image_2d_of<T>& src_im) const
{
  int src_ni = src_im.image().ni();
  int src_nj = src_im.image().nj();
  int dest_ni = dest_im.image().ni();
  int dest_nj = dest_im.image().nj();
  std::ptrdiff_t istep = src_im.image().istep();
  std::ptrdiff_t jstep = src_im.image().jstep();
  int n_planes = src_im.image().nplanes();

  // Reduce plane-by-plane

  // use n-1 because we are trying to align inter-pixel spaces, so that the
  // centre pixel is most accurately registered despite buildup of rounding errors.
  double init_x = 0.5 * (src_ni - 1 - (dest_ni-1)*scale_step());
  double init_y = 0.5 * (src_nj - 1 - (dest_nj-1)*scale_step());


  for (int i=0;i<n_planes;++i)
    scale_reduce(&dest_im.image()(0,0,i),dest_im.image().jstep(),
                 &src_im.image()(0,0,i),src_ni,src_nj,dest_ni,dest_nj,istep,jstep);
#if 0
  vsl_indent_inc(std::cout);
  std::cout << vsl_indent() << "Work image B\n";
  workb_.print_all(std::cout);
  vsl_indent_dec(std::cout);
#endif

    // Sort out world to image transformation for destination image
  vimt_transform_2d scaling;
  scaling.set_zoom_only(1/scale_step(),-init_x,-init_y);
  dest_im.set_world2im(scaling * src_im.world2im());
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
void vimt_scale_pyramid_builder_2d<T>::scale_reduce(
           T* dest_im, std::ptrdiff_t dest_jstep,
           const T* src_im,
           int src_ni, int src_nj, int dest_ni, int dest_nj,
           std::ptrdiff_t src_istep, std::ptrdiff_t src_jstep) const
{
  T* dest_row = dest_im;

  const double init_x = 0.5 * (src_ni-1 - (dest_ni-1)*scale_step());
  double y = 0.5 * (src_nj -1 - (dest_nj-1)*scale_step());
  for (int yi=0; yi<dest_nj; yi++)
  {
    double x=init_x;
    for (int xi=0; xi<dest_ni; xi++)
    {
      dest_row[xi] = l_round (vil_bilin_interp_safe_extend(x, y,
                              src_im, src_ni, src_nj, src_istep, src_jstep), (T)0);
      x += scale_step_;
    }
    y+= scale_step_;
    dest_row += dest_jstep;
  }
}


//: Set the Scale step
template <class T>
void vimt_scale_pyramid_builder_2d<T>::set_scale_step(double scaleStep)
{
  assert(scaleStep> 1.0  && scaleStep<=2.0);
  scale_step_ = scaleStep;
}


//=======================================================================
//: Deletes all data in im_pyr
template<class T>
void vimt_scale_pyramid_builder_2d<T>::emptyPyr(vimt_image_pyramid& im_pyr) const
{
  for (int i=0; i<im_pyr.n_levels();++i)
    delete im_pyr.data()[i];
}

//=======================================================================
//: Checks pyramid has at least n levels
template<class T>
void vimt_scale_pyramid_builder_2d<T>::checkPyr(vimt_image_pyramid& im_pyr,  int n_levels) const
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
    im_pyr.data()[i] = new vimt_image_2d_of<T>;
}

//=======================================================================

template <class T>
void vimt_scale_pyramid_builder_2d<T>::build(
                  vimt_image_pyramid& im_pyr,
                  const vimt_image& im) const
{
  const vimt_image_2d_of<T>& base_image = static_cast<const vimt_image_2d_of<T>&>( im);

  int ni = base_image.image().ni();
  int nj = base_image.image().nj();

  // Compute number of levels to pyramid so that top is no less
  // than miniSize_ x minjSize_
  double s = scale_step();
  int max_lev = 1;
  while ( ((unsigned int)(ni/s+0.5)>=min_x_size_)
          &&
          ((unsigned int)(nj/s+0.5)>=min_y_size_)
        )
  {
    max_lev++;
    s *= scale_step();
  }

  if (max_lev>max_levels())
    max_lev=max_levels();


  // Set up image pyramid
  checkPyr(im_pyr,max_lev);

  vimt_image_2d_of<T>& im0 = static_cast<vimt_image_2d_of<T>&>(im_pyr(0));

  // Shallow copy of part of base_image
  im0 = vimt_crop(base_image,0,ni,0,nj);

  s = scale_step();
  for (int i=1;i<max_lev;i++)
  {
    vimt_image_2d_of<T>& im_i0 = static_cast<vimt_image_2d_of<T>&>(im_pyr(i));
    vimt_image_2d_of<T>& im_i1 = static_cast<vimt_image_2d_of<T>&>(im_pyr(i-1));
    im_i0.image().set_size((int) (ni/s+0.5), (int) (nj/s+0.5),im_i1.image().nplanes());

    s*=scale_step();
    scale_reduce(im_i0,im_i1);
  }

  // Estimate width of pixels in base image
  vgl_point_2d<double>  c0(0.5*(ni-1),0.5*(nj-1));
  vgl_point_2d<double>  c1 = c0 + vgl_vector_2d<double> (1,1);
  vimt_transform_2d im2world = base_image.world2im().inverse();
  vgl_vector_2d<double>  dw = im2world(c1) - im2world(c0);

  double base_pixel_width = std::sqrt(0.5*(dw.x()*dw.x() + dw.y()*dw.y()));

  im_pyr.set_widths(base_pixel_width,scale_step());
}


//=======================================================================
//: Extend pyramid
// The first layer of the pyramid must already be set.
template<class T>
void vimt_scale_pyramid_builder_2d<T>::extend(vimt_image_pyramid& image_pyr) const
{
  assert(image_pyr.scale_step() == scale_step());

  const int ni = static_cast<const vimt_image_2d&>(image_pyr(0)).image_base().ni();
  const int nj = static_cast<const vimt_image_2d&>(image_pyr(0)).image_base().nj();

  // Compute number of levels to pyramid so that top is no less
  // than 5 x 5
  double s = scale_step();
  int max_lev = 1;
  while (((unsigned int)(ni/s+0.5)>=min_x_size_) &&
         ((unsigned int)(nj/s+0.5)>=min_y_size_))
  {
     max_lev++;
     s*=scale_step();
  }

  if (max_lev>max_levels())
      max_lev=max_levels();

  // Set up image pyramid
  int oldsize = image_pyr.n_levels();
  if (oldsize<max_lev) // only extend, if it isn't already tall enough
  {
    image_pyr.data().resize(max_lev);

    s = std::pow(scale_step(), oldsize);
    for (int i=oldsize;i<max_lev;i++)
    {
      image_pyr.data()[i] = new vimt_image_2d_of<T>;
      vimt_image_2d_of<T>& im_i0 = static_cast<vimt_image_2d_of<T>&>(image_pyr(i));
      vimt_image_2d_of<T>& im_i1 = static_cast<vimt_image_2d_of<T>&>(image_pyr(i-1));
      im_i0.image().set_size((int)(ni/s+0.5),(int)(nj/s+0.5),im_i1.image().nplanes());

      s*=scale_step();
      scale_reduce(im_i0,im_i1);
    }
  }
}

//=======================================================================
//: Define maximum number of levels to build
//  Limits levels built in subsequent calls to build()
template<class T>
void vimt_scale_pyramid_builder_2d<T>::set_max_levels(int max_l)
{
  if (max_l<1)
  {
    std::cerr << "vimt_gaussian_pyramid_builder_2d<T>::set_max_levels() "
             << "Must be >=1, not " << max_l << '\n';
    std::abort();
  }
  max_levels_ = max_l;
}

//: Get the current maximum number levels allowed
template<class T>
int vimt_scale_pyramid_builder_2d<T>::max_levels() const
{
  return max_levels_;
}


//=======================================================================
//: Create new (empty) pyramid on heap
//  Caller responsible for its deletion
template<class T>
vimt_image_pyramid* vimt_scale_pyramid_builder_2d<T>::new_image_pyramid() const
{
  return new vimt_image_pyramid;
}


//=======================================================================
#if 0
template <class T>
std::string vimt_scale_pyramid_builder_2d<T>::is_a() const
{
  return std::string("vimt_scale_pyramid_builder_2d<T>");
}
#endif

//=======================================================================

template <class T>
bool vimt_scale_pyramid_builder_2d<T>::is_class(std::string const& s) const
{
  return s==vimt_scale_pyramid_builder_2d<T>::is_a() || vimt_scale_pyramid_builder_2d<T>::is_class(s);
}

//=======================================================================

template <class T>
short vimt_scale_pyramid_builder_2d<T>::version_no() const
{
  return 1;
}

//=======================================================================

template <class T>
vimt_image_pyramid_builder* vimt_scale_pyramid_builder_2d<T>::clone() const
{
  return new vimt_scale_pyramid_builder_2d(*this);
}

//=======================================================================

template <class T>
void vimt_scale_pyramid_builder_2d<T>::print_summary(std::ostream& /*os*/) const
{
  std::cerr << "vimt_scale_pyramid_builder_2d<T>::print_summary() NYI\n";
}

//=======================================================================

template <class T>
void vimt_scale_pyramid_builder_2d<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,scale_step());
}

//=======================================================================

template <class T>
void vimt_scale_pyramid_builder_2d<T>::b_read(vsl_b_istream& bfs)
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
    std::cerr << "I/O ERROR: vimt_scale_pyramid_builder_2d<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#define VIMT_SCALE_PYRAMID_BUILDER_2D_INSTANTIATE(T) \
template <> std::string vimt_scale_pyramid_builder_2d<T >::is_a() const \
{  return std::string("vimt_scale_pyramid_builder_2d<" #T ">"); }\
template class vimt_scale_pyramid_builder_2d<T >

#endif // vimt_scale_pyramid_builder_2d_hxx_
