// This is mul/vimt/vimt_gaussian_pyramid_builder_2d.txx
#ifndef vimt_gaussian_pyramid_builder_2d_txx_
#define vimt_gaussian_pyramid_builder_2d_txx_
//:
// \file
// \brief Class to build gaussian pyramids of vimt_image_2d_of<T>
// \author Tim Cootes

#include "vimt_gaussian_pyramid_builder_2d.h"

#include <vcl_cstdlib.h>
#include <vcl_string.h>

#include <vcl_cassert.h>
#include <vnl/vnl_math.h> // for sqrt2
#include <vil/algo/vil_gauss_reduce.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_crop.h>

//=======================================================================

template<class T>
vimt_gaussian_pyramid_builder_2d<T>::vimt_gaussian_pyramid_builder_2d()
: max_levels_(99),filter_width_(5)
{
  set_min_size(5, 5);
}

//=======================================================================

template<class T>
vimt_gaussian_pyramid_builder_2d<T>::~vimt_gaussian_pyramid_builder_2d()
{
}

//=======================================================================
//: Define maximum number of levels to build
//  Limits levels built in subsequent calls to build()
template<class T>
void vimt_gaussian_pyramid_builder_2d<T>::set_max_levels(int max_l)
{
  if (max_l<1)
  {
    vcl_cerr<<"vimt_gaussian_pyramid_builder_2d<T>::setMaxLevels() ";
    vcl_cerr<<"Must be >=1\n";
    vcl_abort();
  }
  max_levels_ = max_l;
}

//: Get the current maximum number levels allowed
template<class T>
int vimt_gaussian_pyramid_builder_2d<T>::max_levels() const
{
  return max_levels_;
}

//=======================================================================
//: Create new (empty) pyramid on heap
//  Caller responsible for its deletion
template<class T>
vimt_image_pyramid* vimt_gaussian_pyramid_builder_2d<T>::new_image_pyramid() const
{
  return new vimt_image_pyramid;
}

//=======================================================================
//: Scale step between levels
template<class T>
double vimt_gaussian_pyramid_builder_2d<T>::scale_step() const
{
  return 2.0;
}

//: Set current filter width (must be 3 or 5 at present)
template<class T>
void vimt_gaussian_pyramid_builder_2d<T>::set_filter_width(unsigned w)
{
  assert(w==3 || w==5);
  filter_width_ = w;
}

//: Smooth and subsample src_im to produce dest_im
//  Applies filter in x and y, then samples every other pixel.
template<class T>
void vimt_gaussian_pyramid_builder_2d<T>::gauss_reduce(const vimt_image_2d_of<T>& src_im,
                                                       vimt_image_2d_of<T>& dest_im) const
{
  switch (filter_width_)
  {
    case (3):
      vil_gauss_reduce_121(src_im.image(),dest_im.image());
      break;
    case (5):
      vil_gauss_reduce(src_im.image(),dest_im.image(),work_im_.image());
      break;
    default:
      vcl_cerr<<"vimt_gaussian_pyramid_builder_2d<T>::gauss_reduce() "
              <<"cannot cope with filter width of "<<filter_width_<<'\n';
      vcl_abort();
  }

  // Sort out world to image transformation for destination image
  vimt_transform_2d scaling;
  scaling.set_zoom_only(0.5,0,0);
  dest_im.set_world2im(scaling * src_im.world2im());
}

//=======================================================================
//: Deletes all data in im_pyr
template<class T>
void vimt_gaussian_pyramid_builder_2d<T>::empty_pyr(vimt_image_pyramid& im_pyr) const
{
  for (int i=0; i<im_pyr.n_levels();++i)
    delete im_pyr.data()[i];
}

//=======================================================================
//: Checks pyramid has at least n levels
template<class T>
void vimt_gaussian_pyramid_builder_2d<T>::check_pyr(vimt_image_pyramid& im_pyr,  int n_levels) const
{
  const int got_levels = im_pyr.n_levels();
  if (got_levels >= n_levels && im_pyr(0).is_class(work_im_.is_a()))
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

  im_pyr.resize(n_levels,vimt_image_2d_of<T>());
}

//=======================================================================
//: Build pyramid
template<class T>
void vimt_gaussian_pyramid_builder_2d<T>::build(vimt_image_pyramid& image_pyr,
                                                const vimt_image& im) const
{
  //  Require image vimt_image_2d_of<T>
  assert(im.is_class(work_im_.is_a()));

  const vimt_image_2d_of<T>& base_image = static_cast<const vimt_image_2d_of<T>&>(im);

  int ni = base_image.image().ni();
  int nj = base_image.image().nj();

  // Compute number of levels to pyramid so that top is no less
  // than minXSize_ x minYSize_
  int s = 1;
  int max_levels = 1;
  while ((ni/(2*s)>=int(minXSize_)) && (nj/(2*s)>=int(minYSize_)))
  {
    max_levels++;
    s*=2;
  }

  if (max_levels>max_levels_)
    max_levels=max_levels_;

  // Set up image pyramid
  check_pyr(image_pyr,max_levels);

  vimt_image_2d_of<T>& im0 = static_cast<vimt_image_2d_of<T>&>( image_pyr(0));

  // Shallow copy of part of base_image
  im0 = vimt_crop(base_image,0,ni,0,nj);

  int i;
  for (i=1;i<max_levels;i++)
  {
    vimt_image_2d_of<T>& im_i0 = static_cast<vimt_image_2d_of<T>&>( image_pyr(i));
    vimt_image_2d_of<T>& im_i1 = static_cast<vimt_image_2d_of<T>&>(image_pyr(i-1));

    gauss_reduce(im_i1,im_i0);
  }

  // Estimate width of pixels in base image
  vgl_point_2d<double>  c0(0.5*(ni-1),0.5*(nj-1));
  vgl_point_2d<double>  c1 = c0 + vgl_vector_2d<double> (1,1);
  vimt_transform_2d im2world = base_image.world2im().inverse();
  vgl_vector_2d<double>  dw = im2world(c1) - im2world(c0);

  double base_pixel_width = dw.length()/vnl_math::sqrt2;
  double scale_step = 2.0;

  image_pyr.set_widths(base_pixel_width,scale_step);
}

//=======================================================================
//: Extend pyramid
// The first layer of the pyramid must already be set.
template<class T>
void vimt_gaussian_pyramid_builder_2d<T>::extend(vimt_image_pyramid& image_pyr) const
{
  //  Require image vimt_image_2d_of<T>
  assert(image_pyr(0).is_class(work_im_.is_a()));

  assert(image_pyr.scale_step() == scale_step());

  vimt_image_2d_of<T>& im_base = static_cast<vimt_image_2d_of<T>&>( image_pyr(0));
  int ni = im_base.image().ni();
  int nj = im_base.image().nj();

  // Compute number of levels to pyramid so that top is no less
  // than 5 x 5
  double s = 1;
  int max_levels = 1;
  while ((ni/(scale_step()*s)>=minXSize_) && (nj/(scale_step()*s)>=minYSize_))
  {
    max_levels++;
    s*=scale_step();
  }

  if (max_levels>max_levels_)
    max_levels=max_levels_;

  // Set up image pyramid
  int oldsize = image_pyr.n_levels();
  if (oldsize<max_levels) // only extend, if it isn't already tall enough
  {
    image_pyr.data().resize(max_levels);

    int i;
    for (i=oldsize;i<max_levels;++i)
      image_pyr.data()[i] = new vimt_image_2d_of<T>;

    for (i=oldsize;i<max_levels;i++)
    {
      vimt_image_2d_of<T>& im_i0 = static_cast<vimt_image_2d_of<T>&>( image_pyr(i));
      vimt_image_2d_of<T>& im_i1 = static_cast<vimt_image_2d_of<T>&>(image_pyr(i-1));

      gauss_reduce(im_i1,im_i0);
    }
  }
}


//=======================================================================

#if 0
template<class T>
vcl_string vimt_gaussian_pyramid_builder_2d<T>::is_a() const
{
  return vcl_string("vimt_gaussian_pyramid_builder_2d<T>");
}
#endif

//=======================================================================

template<class T>
bool vimt_gaussian_pyramid_builder_2d<T>::is_class(vcl_string const& s) const
{
  return s==vimt_gaussian_pyramid_builder_2d<T>::is_a() || vimt_image_pyramid_builder::is_class(s);
}

//=======================================================================

template<class T>
short vimt_gaussian_pyramid_builder_2d<T>::version_no() const
{
  return 2;
}

//=======================================================================

template<class T>
vimt_image_pyramid_builder* vimt_gaussian_pyramid_builder_2d<T>::clone() const
{
  return new vimt_gaussian_pyramid_builder_2d<T>(*this);
}

//=======================================================================

template<class T>
void vimt_gaussian_pyramid_builder_2d<T>::print_summary(vcl_ostream&) const
{
}

//=======================================================================

template<class T>
void vimt_gaussian_pyramid_builder_2d<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,max_levels_);
  vsl_b_write(bfs,filter_width_);
}

//=======================================================================

template<class T>
void vimt_gaussian_pyramid_builder_2d<T>::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  // version number starts at 2 to follow on from the old mil stuff
  case (2):
    vsl_b_read(bfs,max_levels_);
    vsl_b_read(bfs,filter_width_);
    break;
  default:
    vcl_cerr << "I/O ERROR: vimt_gaussian_pyramid_builder_2d<T>::b_read(vsl_b_istream&)\n";
    vcl_cerr << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#define VIMT_GAUSSIAN_PYRAMID_BUILDER_2D_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string vimt_gaussian_pyramid_builder_2d<T >::is_a() const \
{  return vcl_string("vimt_gaussian_pyramid_builder_2d<" #T ">"); }\
template class vimt_gaussian_pyramid_builder_2d<T >

#endif // vimt_gaussian_pyramid_builder_2d_txx_
