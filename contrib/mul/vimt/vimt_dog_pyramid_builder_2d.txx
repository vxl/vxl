// This is mul/vimt/vimt_dog_pyramid_builder_2d.txx
#ifndef vimt_dog_pyramid_builder_2d_txx_
#define vimt_dog_pyramid_builder_2d_txx_
//:
// \file
//  \brief Build difference of gaussian pyramids of vimt_image_2d_of<T>
// \author Tim Cootes

#include "vimt_dog_pyramid_builder_2d.h"
#include "vimt_image_pyramid.h"

#include <vcl_cstdlib.h>
#include <vcl_string.h>

#include <vcl_cassert.h>
#include <vnl/vnl_math.h> // for sqrt2
#include <vil/algo/vil_gauss_filter.h>
#include <vil/vil_resample_bilin.h>
#include <vil/vil_math.h>
#include <vimt/vimt_crop.h>

//=======================================================================

template<class T>
vimt_dog_pyramid_builder_2d<T>::vimt_dog_pyramid_builder_2d()
: max_levels_(99)
{
  set_min_size(5, 5);
}

//=======================================================================

template<class T>
vimt_dog_pyramid_builder_2d<T>::~vimt_dog_pyramid_builder_2d()
{
}

//=======================================================================
//: Define maximum number of levels to build
//  Limits levels built in subsequent calls to build()
template<class T>
void vimt_dog_pyramid_builder_2d<T>::set_max_levels(int max_l)
{
  if (max_l<1)
  {
    vcl_cerr<<"vimt_dog_pyramid_builder_2d<T>::set_max_levels() argument must be >=1\n";
    vcl_abort();
  }
  max_levels_ = max_l;
}

//: Get the current maximum number levels allowed
template<class T>
int vimt_dog_pyramid_builder_2d<T>::max_levels() const
{
  return max_levels_;
}

//=======================================================================
//: Create new (empty) pyramid on heap
//  Caller responsible for its deletion
template<class T>
vimt_image_pyramid* vimt_dog_pyramid_builder_2d<T>::new_image_pyramid() const
{
  return new vimt_image_pyramid;
}

//=======================================================================
//: Scale step between levels
template<class T>
double vimt_dog_pyramid_builder_2d<T>::scale_step() const
{
  return 1.5;
}


//=======================================================================
//: Deletes all data in im_pyr
template<class T>
void vimt_dog_pyramid_builder_2d<T>::empty_pyr(vimt_image_pyramid& im_pyr) const
{
  for (int i=0; i<im_pyr.n_levels();++i)
    delete im_pyr.data()[i];
}

//=======================================================================
//: Checks pyramid has at least n levels
template<class T>
void vimt_dog_pyramid_builder_2d<T>::check_pyr(vimt_image_pyramid& im_pyr,  int n_levels) const
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

  im_pyr.data().resize(n_levels);
  empty_pyr(im_pyr);

  for (int i=0;i<n_levels;++i)
    im_pyr.data()[i] = new vimt_image_2d_of<T>;
}

//: Build pyramid
template<class T>
void vimt_dog_pyramid_builder_2d<T>::build(vimt_image_pyramid& dog_pyr,
                                           const vimt_image& im) const
{
  vimt_image_pyramid smooth_pyr;
  build_dog(dog_pyr,smooth_pyr,im);
}

//=======================================================================
//: Build pyramid
template<class T>
void vimt_dog_pyramid_builder_2d<T>::build_dog(vimt_image_pyramid& dog_pyr,
                                               vimt_image_pyramid& smooth_pyr,
                                               const vimt_image& im, bool abs_diff) const
{
  //  Require image vimt_image_2d_of<T>
  assert(im.is_class(work_im_.is_a()));

  const vimt_image_2d_of<T>& base_image = static_cast<const vimt_image_2d_of<T>&>(im);

  unsigned ni = base_image.image().ni();
  unsigned nj = base_image.image().nj();

  // Compute number of levels to pyramid so that top is no less
  // than min_x_size_ x min_y_size_
  int max_levels = 1;
  while (ni>min_x_size_ && nj>min_y_size_)
  {
    max_levels++;
    ni = 2*ni/3;
    nj = 2*nj/3;
  }

  if (max_levels>max_levels_)
    max_levels=max_levels_;

  // Set up image pyramid
  check_pyr(dog_pyr,max_levels);
  check_pyr(smooth_pyr,max_levels);

  vimt_image_2d_of<T>& smooth0 = static_cast<vimt_image_2d_of<T>&>( smooth_pyr(0));
  vimt_image_2d_of<T>& dog0 = static_cast<vimt_image_2d_of<T>&>( dog_pyr(0));

  vil_gauss_filter_5tap_params smooth_params(0.75);

  vil_gauss_filter_5tap(base_image.image(),smooth0.image(),smooth_params,
                        dog0.image());  // Workspace

  if (abs_diff)
    vil_math_image_abs_difference(base_image.image(),smooth0.image(),dog0.image());
  else
    vil_math_image_difference(base_image.image(),smooth0.image(),dog0.image());

  smooth0.set_world2im(base_image.world2im());
  dog0.set_world2im(base_image.world2im());

  unsigned n_planes = base_image.image().nplanes();

  // Sort out world to image transformation for destination image
  vimt_transform_2d scaling_trans;
  scaling_trans.set_zoom_only(2.0/3.0,0,0);

  // Workspace
  vil_image_view<T> sub_sampled_image;


  // Subsequent levels
  for (int i=1;i<max_levels;++i)
  {
    vimt_image_2d_of<T>& smooth0 = static_cast<vimt_image_2d_of<T>&>( smooth_pyr(i-1));
    vimt_image_2d_of<T>& smooth1 = static_cast<vimt_image_2d_of<T>&>( smooth_pyr(i));
    vimt_image_2d_of<T>& dog1 = static_cast<vimt_image_2d_of<T>&>( dog_pyr(i));

    // Subsample by a factor of 2/3
    // Note - this could be implemented more efficiently
    //        since bilinear is sampling at pixel positions
    //        and on edges.
    unsigned ni = smooth0.image().ni();
    unsigned nj = smooth0.image().nj();
    ni = 2*ni/3;
    nj = 2*nj/3;
    sub_sampled_image.set_size(ni,nj,n_planes);

    vil_resample_bilin(smooth0.image(),sub_sampled_image,
                       0.0,0.0, 1.5,0.0,  0.0,1.5, ni,nj);

    vil_gauss_filter_5tap(sub_sampled_image,smooth1.image(),
                          smooth_params,
                          dog1.image());  // Workspace
    if (abs_diff)
      vil_math_image_abs_difference(sub_sampled_image,smooth1.image(),
                                    dog1.image());
    else
      vil_math_image_difference(sub_sampled_image,smooth1.image(),
                              dog1.image());

    smooth1.set_world2im(scaling_trans*smooth0.world2im());
    dog1.set_world2im(smooth1.world2im());
  }

  // Estimate width of pixels in base image
  vgl_point_2d<double>  c0(0.5*(ni-1),0.5*(nj-1));
  vgl_point_2d<double>  c1 = c0 + vgl_vector_2d<double> (1,1);
  vimt_transform_2d im2world = base_image.world2im().inverse();
  vgl_vector_2d<double>  dw = im2world(c1) - im2world(c0);

  double base_pixel_width = dw.length()/vnl_math::sqrt2;
  double scale_step = 1.5;

  dog_pyr.set_widths(base_pixel_width,scale_step);
  smooth_pyr.set_widths(base_pixel_width,scale_step);
}

//=======================================================================
//: Extend pyramid (not implemented)
template<class T>
void vimt_dog_pyramid_builder_2d<T>::extend(vimt_image_pyramid& image_pyr) const
{
}


//=======================================================================

#if 0
template<class T>
vcl_string vimt_dog_pyramid_builder_2d<T>::is_a() const
{
  return vcl_string("vimt_dog_pyramid_builder_2d<T>");
}
#endif

//=======================================================================

template<class T>
bool vimt_dog_pyramid_builder_2d<T>::is_class(vcl_string const& s) const
{
  return s==vimt_dog_pyramid_builder_2d<T>::is_a() || vimt_image_pyramid_builder::is_class(s);
}

//=======================================================================

template<class T>
short vimt_dog_pyramid_builder_2d<T>::version_no() const
{
  return 1;
}

//=======================================================================

template<class T>
vimt_image_pyramid_builder* vimt_dog_pyramid_builder_2d<T>::clone() const
{
  return new vimt_dog_pyramid_builder_2d<T>(*this);
}

//=======================================================================

template<class T>
void vimt_dog_pyramid_builder_2d<T>::print_summary(vcl_ostream&) const
{
}

//=======================================================================

template<class T>
void vimt_dog_pyramid_builder_2d<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,max_levels_);
  vsl_b_write(bfs,min_x_size_);
  vsl_b_write(bfs,min_y_size_);
}

//=======================================================================

template<class T>
void vimt_dog_pyramid_builder_2d<T>::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  // version number starts at 2 to follow on from the old mil stuff
  case (1):
    vsl_b_read(bfs,max_levels_);
    vsl_b_read(bfs,min_x_size_);
    vsl_b_read(bfs,min_y_size_);
    break;
  default:
    vcl_cerr << "I/O ERROR: vimt_dog_pyramid_builder_2d<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#define VIMT_DOG_PYRAMID_BUILDER_2D_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string vimt_dog_pyramid_builder_2d<T >::is_a() const \
{  return vcl_string("vimt_dog_pyramid_builder_2d<" #T ">"); }\
template class vimt_dog_pyramid_builder_2d<T >

#endif // vimt_dog_pyramid_builder_2d_txx_
