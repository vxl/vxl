// This is mul/mil/mil_gaussian_pyramid_builder_2d.txx
#ifndef mil_gaussian_pyramid_builder_2d_txx_
#define mil_gaussian_pyramid_builder_2d_txx_
//:
// \file
// \brief Class to build Gaussian pyramids of mil_image_2d_of<T>
// \author Tim Cootes

#include "mil_gaussian_pyramid_builder_2d.h"

#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_cassert.h>

#include <vnl/vnl_math.h> // for sqrt2
#include <mil/mil_image_pyramid.h>
#include <mil/mil_gauss_reduce_2d.h>

//=======================================================================

template<class T>
mil_gaussian_pyramid_builder_2d<T>::mil_gaussian_pyramid_builder_2d()
: max_levels_(99),filter_width_(5)
{
  set_min_size(5, 5);
}

//=======================================================================

template<class T>
mil_gaussian_pyramid_builder_2d<T>::~mil_gaussian_pyramid_builder_2d()
{
}

//=======================================================================
//: Define maximum number of levels to build
//  Limits levels built in subsequent calls to build()
template<class T>
void mil_gaussian_pyramid_builder_2d<T>::setMaxLevels(int max_l)
{
  if (max_l<1)
  {
    vcl_cerr<<"mil_gaussian_pyramid_builder_2d<T>::setMaxLevels() param is "
            <<max_l<<", must be >=1\n";
    vcl_abort();
  }
  max_levels_ = max_l;
}

//: Get the current maximum number levels allowed
template<class T>
int mil_gaussian_pyramid_builder_2d<T>::maxLevels() const
{
  return max_levels_;
}

//=======================================================================
//: Create new (empty) pyramid on heap
//  Caller responsible for its deletion
template<class T>
mil_image_pyramid* mil_gaussian_pyramid_builder_2d<T>::newImagePyramid() const
{
  return new mil_image_pyramid;
}


//=======================================================================
//: Scale step between levels
template<class T>
double mil_gaussian_pyramid_builder_2d<T>::scale_step() const
{
  return 2.0;
}

//: Set current filter width (must be 3 or 5 at present)
template<class T>
void mil_gaussian_pyramid_builder_2d<T>::set_filter_width(unsigned w)
{
  assert(w==3 || w==5);
  filter_width_ = w;
}

//=======================================================================
//: Smooth and subsample src_im to produce dest_im
//  Applies 1-2-1 filter in x and y, then samples every other pixel.
template<class T>
void mil_gaussian_pyramid_builder_2d<T>::gauss_reduce_121(mil_image_2d_of<T>& dest_im,
                                                          const mil_image_2d_of<T>& src_im) const
{
  int nx = src_im.nx();
  int ny = src_im.ny();
  int n_planes = src_im.n_planes();

  // Output image size
  int nx2 = (nx+1)/2;
  int ny2 = (ny+1)/2;

  if (dest_im.n_planes()!=n_planes)
    dest_im.set_n_planes(n_planes);
  dest_im.resize(nx2,ny2);

  // Reduce plane-by-plane
  for (int i=0;i<n_planes;++i)
  {
    // Smooth and subsample in x, result in work_im
    mil_gauss_reduce_121_2d(dest_im.plane(i),dest_im.xstep(),dest_im.ystep(),
                            src_im.plane(i),nx,ny,
                            src_im.xstep(),src_im.ystep());
  }

  // Sort out world to image transformation for destination image
  mil_transform_2d scaling;
  scaling.set_zoom_only(0.5,0,0);
  dest_im.setWorld2im(scaling * src_im.world2im());
}

//=======================================================================
//: Smooth and subsample src_im to produce dest_im
//  Applies filter in x and y, then samples every other pixel.
template<class T>
void mil_gaussian_pyramid_builder_2d<T>::gauss_reduce_15851(mil_image_2d_of<T>& dest_im,
                                                            const mil_image_2d_of<T>& src_im) const
{
  int nx = src_im.nx();
  int ny = src_im.ny();
  int n_planes = src_im.n_planes();

  // Output image size
  int nx2 = (nx+1)/2;
  int ny2 = (ny+1)/2;

  if (dest_im.n_planes()!=n_planes)
    dest_im.set_n_planes(n_planes);
  dest_im.resize(nx2,ny2);

  if (work_im_.nx()<nx2 || work_im_.ny()<ny)
    work_im_.resize(nx2,ny);

  // Reduce plane-by-plane
  for (int i=0;i<n_planes;++i)
  {
    // Smooth and subsample in x, result in work_im
    mil_gauss_reduce_2d(work_im_.plane(0),work_im_.xstep(),work_im_.ystep(),
                        src_im.plane(i),nx,ny,
                        src_im.xstep(),src_im.ystep());

    // Smooth and subsample in y (by implicitly transposing)
    mil_gauss_reduce_2d(dest_im.plane(i),dest_im.ystep(),dest_im.xstep(),
                        work_im_.plane(0),ny,nx2,
                        work_im_.ystep(),work_im_.xstep());
  }

  // Sort out world to image transformation for destination image
  mil_transform_2d scaling;
  scaling.set_zoom_only(0.5,0,0);
  dest_im.setWorld2im(scaling * src_im.world2im());
}

//=======================================================================
//: Smooth and subsample src_im to produce dest_im
//  Applies 1-5-8-5-1 filter in x and y, then samples
//  every other pixel.
template<class T>
void mil_gaussian_pyramid_builder_2d<T>::gauss_reduce(mil_image_2d_of<T>& dest_im,
                                                      const mil_image_2d_of<T>& src_im) const
{
  switch (filter_width_)
  {
    case (3):
      gauss_reduce_121(dest_im,src_im);
      break;
    case (5):
      gauss_reduce_15851(dest_im,src_im);
      break;
    default:
      vcl_cerr<<"mil_gaussian_pyramid_builder_2d<T>::gauss_reduce()\n"
              <<" Cannot cope with filter width of "<<filter_width_<<vcl_endl;
      vcl_abort();
  }
}

//=======================================================================
//: Deletes all data in im_pyr
template<class T>
void mil_gaussian_pyramid_builder_2d<T>::emptyPyr(mil_image_pyramid& im_pyr) const
{
  for (int i=0; i<im_pyr.n_levels();++i)
    delete im_pyr.data()[i];
}

//=======================================================================
//: Checks pyramid has at least n levels
template<class T>
void mil_gaussian_pyramid_builder_2d<T>::checkPyr(mil_image_pyramid& im_pyr,  int n_levels) const
{
  const int got_levels = im_pyr.n_levels();
  if (got_levels >= n_levels && im_pyr(0).is_a()==work_im_.is_a())
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
//: Build pyramid
template<class T>
void mil_gaussian_pyramid_builder_2d<T>::build(mil_image_pyramid& image_pyr,
                                               const mil_image& im) const
{
  //  Require image mil_image_2d_of<T>
  assert(im.is_a()==work_im_.is_a());

  const mil_image_2d_of<T>& base_image = (const mil_image_2d_of<T>&) im;

  int nx = base_image.nx();
  int ny = base_image.ny();

  // Compute number of levels to pyramid so that top is no less
  // than minXSize_ x minYSize_
  int s = 1;
  int max_levels = 1;
  while ((nx/(2*s)>=int(minXSize_)) && (ny/(2*s)>=int(minYSize_)))
  {
    max_levels++;
    s*=2;
  }

  if (max_levels>max_levels_)
    max_levels=max_levels_;

  work_im_.resize(nx,ny);

  // Set up image pyramid
  checkPyr(image_pyr,max_levels);

  mil_image_2d_of<T>& im0 = (mil_image_2d_of<T>&) image_pyr(0);

  // Shallow copy of part of base_image
  im0.setToWindow(base_image,0,nx-1,0,ny-1);

  int i;
  for (i=1;i<max_levels;i++)
  {
    mil_image_2d_of<T>& im_i0 = (mil_image_2d_of<T>&) image_pyr(i);
    mil_image_2d_of<T>& im_i1 = (mil_image_2d_of<T>&) image_pyr(i-1);

    gauss_reduce(im_i0,im_i1);
  }

  // Estimate width of pixels in base image
  vgl_point_2d<double>  c0(0.5*(nx-1),0.5*(ny-1));
  vgl_point_2d<double>  c1 = c0 + vgl_vector_2d<double> (1,1);
  mil_transform_2d im2world = base_image.world2im().inverse();
  vgl_vector_2d<double>  dw = im2world(c1) - im2world(c0);

  double base_pixel_width = dw.length()/vnl_math::sqrt2;
  double scale_step = 2.0;

  image_pyr.setWidths(base_pixel_width,scale_step);
}

//=======================================================================
//: Extend pyramid
// The first layer of the pyramid must already be set.
template<class T>
void mil_gaussian_pyramid_builder_2d<T>::extend(mil_image_pyramid& image_pyr) const
{
  //  Require image mil_image_2d_of<T>
  assert(image_pyr(0).is_a() == work_im_.is_a());

  assert(image_pyr.scale_step() == scale_step());

  int nx = image_pyr(0).nx();
  int ny = image_pyr(0).ny();

  // Compute number of levels to pyramid so that top is no less
  // than 5 x 5
  double s = 1;
  int max_levels = 1;
  while ((nx/(scale_step()*s)>=minXSize_) && (ny/(scale_step()*s)>=minXSize_))
  {
    max_levels++;
    s*=scale_step();
  }

  if (max_levels>max_levels_)
    max_levels=max_levels_;

  work_im_.resize(nx,ny);

  // Set up image pyramid
  int oldsize = image_pyr.n_levels();
  if (oldsize<max_levels) // only extend, if it isn't already tall enough
  {
    image_pyr.data().resize(max_levels);

    int i;
    for (i=oldsize;i<max_levels;++i)
      image_pyr.data()[i] = new mil_image_2d_of<T>;

    for (i=oldsize;i<max_levels;i++)
    {
      mil_image_2d_of<T>& im_i0 = (mil_image_2d_of<T>&) image_pyr(i);
      mil_image_2d_of<T>& im_i1 = (mil_image_2d_of<T>&) image_pyr(i-1);

      gauss_reduce(im_i0,im_i1);
    }
  }
}


//=======================================================================

template<class T>
bool mil_gaussian_pyramid_builder_2d<T>::is_class(vcl_string const& s) const
{
  return s==mil_gaussian_pyramid_builder_2d<T>::is_a() || mil_image_pyramid_builder::is_class(s);
}

//=======================================================================

template<class T>
short mil_gaussian_pyramid_builder_2d<T>::version_no() const
{
  return 2;
}

//=======================================================================

template<class T>
mil_image_pyramid_builder* mil_gaussian_pyramid_builder_2d<T>::clone() const
{
  return new mil_gaussian_pyramid_builder_2d<T>(*this);
}

//=======================================================================

template<class T>
void mil_gaussian_pyramid_builder_2d<T>::print_summary(vcl_ostream&) const
{
}

//=======================================================================

template<class T>
void mil_gaussian_pyramid_builder_2d<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,max_levels_);
  vsl_b_write(bfs,filter_width_);
}

//=======================================================================

template<class T>
void mil_gaussian_pyramid_builder_2d<T>::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs,max_levels_);
    filter_width_=5;
    break;
  case (2):
    vsl_b_read(bfs,max_levels_);
    vsl_b_read(bfs,filter_width_);
    break;
  default:
    vcl_cerr << "I/O ERROR: mil_gaussian_pyramid_builder_2d<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#undef MIL_GAUSSIAN_PYRAMID_BUILDER_2D_INSTANTIATE
#define MIL_GAUSSIAN_PYRAMID_BUILDER_2D_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string mil_gaussian_pyramid_builder_2d<T >::is_a() const \
{ return vcl_string("mil_gaussian_pyramid_builder_2d<" #T ">"); } \
template class mil_gaussian_pyramid_builder_2d<T >

#endif // mil_gaussian_pyramid_builder_2d_txx_
