// This is mul/vimt/vimt_gaussian_pyramid_builder_2d_general.txx
#ifndef vimt_gaussian_pyramid_builder_2d_general_txx_
#define vimt_gaussian_pyramid_builder_2d_general_txx_
//:
// \file
// \brief Build gaussian image pyramids at any scale separation
// \author Ian Scott

#include "vimt_gaussian_pyramid_builder_2d_general.h"
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vil/algo/vil_gauss_reduce.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_crop.h>

//=======================================================================

template <class T>
vimt_gaussian_pyramid_builder_2d_general<T>::vimt_gaussian_pyramid_builder_2d_general():
  scale_params_(2.0)
{}

//: Construct with given scale_step
template <class T>
vimt_gaussian_pyramid_builder_2d_general<T>::vimt_gaussian_pyramid_builder_2d_general(double scale_step):
  scale_params_(scale_step)
{}

//=======================================================================

template <class T>
vimt_gaussian_pyramid_builder_2d_general<T>::~vimt_gaussian_pyramid_builder_2d_general()
{}

//: Set the Scale step
template <class T>
void vimt_gaussian_pyramid_builder_2d_general<T>::set_scale_step(double scaleStep)
{
  scale_params_ = vil_gauss_reduce_params(scaleStep);
}

//=======================================================================

template <class T>
void vimt_gaussian_pyramid_builder_2d_general<T>::build(
                  vimt_image_pyramid& im_pyr,
                  const vimt_image& im) const
{
  assert(im.is_class(vimt_image_2d_of<T>().is_a()));

  const vimt_image_2d_of<T>& base_image = static_cast<const vimt_image_2d_of<T>&>(im);

  int ni = base_image.image().ni();
  int nj = base_image.image().nj();

  // Compute number of levels to pyramid so that top is no less
  // than minXSize_ x minYSize_
  double s = scale_step();
  int maxlevels = 1;
  while (((unsigned int)(ni/s+0.5)>=this->min_x_size()) &&
         ((unsigned int)(nj/s+0.5)>=this->min_y_size()))
  {
    maxlevels++;
    s *= scale_step();
  }

  if (maxlevels > this->max_levels())
    maxlevels=this->max_levels();

  worka_.set_size(ni,nj);
  workb_.set_size(ni,nj);

  // Set up image pyramid
  this->check_pyr(im_pyr,maxlevels);

  vimt_image_2d_of<T>& im0 = static_cast<vimt_image_2d_of<T>&>(im_pyr(0));

  // Shallow copy of part of base_image
  im0 = vimt_crop(base_image,0,ni,0,nj);

  s = scale_step();
  for (int i=1;i<maxlevels;i++)
  {
    vimt_image_2d_of<T>& dest = static_cast<vimt_image_2d_of<T>&>(im_pyr(i));
    const vimt_image_2d_of<T>& src = static_cast<const vimt_image_2d_of<T>&>(im_pyr(i-1));

    s*=scale_step();
    vil_gauss_reduce_general(src.image(), dest.image(), worka_, workb_, scale_params_);

    // Sort out world to image transformation for destination image
    vimt_transform_2d scaling;

    // use n-1 because we are trying to align inter-pixel spaces, so that the
    // centre pixel is most accurately registered despite buildup of rounding errors.
    const double init_x = 0.5 * (src.image().ni() - 1 - (dest.image().ni()-1)*scale_params_.scale_step());
    const double init_y = 0.5 * (src.image().nj() - 1 - (dest.image().nj()-1)*scale_params_.scale_step());

    scaling.set_zoom_only(1/scale_params_.scale_step(),-init_x,-init_y);
    dest.set_world2im(scaling * src.world2im());
  }

  // Estimate width of pixels in base image
  vgl_point_2d<double>  c0(0.5*(ni-1),0.5*(nj-1));
  vgl_point_2d<double>  c1 = c0 + vgl_vector_2d<double> (1,1);
  vimt_transform_2d im2world = base_image.world2im().inverse();
  vgl_vector_2d<double>  dw = im2world(c1) - im2world(c0);

  double base_pixel_width = vcl_sqrt(0.5*(dw.x()*dw.x() + dw.y()*dw.y()));

  im_pyr.set_widths(base_pixel_width,scale_step());
}


//=======================================================================
//: Extend pyramid
// The first layer of the pyramid must already be set.
template<class T>
void vimt_gaussian_pyramid_builder_2d_general<T>::extend(vimt_image_pyramid& image_pyr) const
{
  //  Require image vil_image_view<T>
  assert(image_pyr(0).is_class(vimt_image_2d_of<T>().is_a()));

  assert(image_pyr.scale_step() == scale_step());

  const vimt_image_2d_of<T>& base_image = static_cast<const vimt_image_2d_of<T>&>(image_pyr(0));


  const int ni = base_image.image().ni();
  const int nj = base_image.image().nj();

  // Compute number of levels to pyramid so that top is no less
  // than 5 x 5
  double s = scale_step();
  int maxlevels = 1;
  while (((unsigned int)(ni/s+0.5) >= this->min_x_size()) &&
         ((unsigned int)(nj/s+0.5) >= this->min_y_size()))
  {
     maxlevels++;
     s*=scale_step();
  }

  if (maxlevels > this->max_levels())
    maxlevels=this->max_levels();

  worka_.set_size(ni,nj);
  workb_.set_size(ni,nj);

  // Set up image pyramid
  int oldsize = image_pyr.n_levels();
  if (oldsize<maxlevels) // only extend, if it isn't already tall enough
  {
    image_pyr.data().resize(maxlevels);

    s = vcl_pow(scale_step(), oldsize);
    for (int i=oldsize;i<maxlevels;i++)
    {
      image_pyr.data()[i] = new vimt_image_2d_of<T>;
      vimt_image_2d_of<T>& im_i0 = static_cast<vimt_image_2d_of<T>&>(image_pyr(i));
      vimt_image_2d_of<T>& im_i1 = static_cast<vimt_image_2d_of<T>&>(image_pyr(i-1));
      im_i0.image().set_size((unsigned)(ni/s+0.5),(unsigned)(nj/s+0.5),
        im_i1.image().nplanes());

      s*=scale_step();
      vil_gauss_reduce_general(im_i1.image(), im_i0.image(), worka_, workb_, scale_params_);
    }
  }
}


//=======================================================================
#if 0
template <class T>
vcl_string vimt_gaussian_pyramid_builder_2d_general<T>::is_a() const
{
  return vcl_string("vimt_gaussian_pyramid_builder_2d_general<T>");
}
#endif // 0
//=======================================================================

template <class T>
bool vimt_gaussian_pyramid_builder_2d_general<T>::is_class(vcl_string const& s) const
{
  return s==vimt_gaussian_pyramid_builder_2d_general<T>::is_a() ||
         vimt_gaussian_pyramid_builder_2d<T>::is_class(s);
}

//=======================================================================

template <class T>
short vimt_gaussian_pyramid_builder_2d_general<T>::version_no() const
{
  return 1;
}

//=======================================================================

template <class T>
vimt_image_pyramid_builder* vimt_gaussian_pyramid_builder_2d_general<T>::clone() const
{
  return new vimt_gaussian_pyramid_builder_2d_general(*this);
}

//=======================================================================

template <class T>
void vimt_gaussian_pyramid_builder_2d_general<T>::print_summary(vcl_ostream& os) const
{
  vimt_gaussian_pyramid_builder_2d<T>::print_summary(os);
}

//=======================================================================

template <class T>
void vimt_gaussian_pyramid_builder_2d_general<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vimt_gaussian_pyramid_builder_2d<T>::b_write(bfs);
  vsl_b_write(bfs,scale_step());
}

//=======================================================================

template <class T>
void vimt_gaussian_pyramid_builder_2d_general<T>::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  double scale;

  switch (version)
  {
   case 1:
    vimt_gaussian_pyramid_builder_2d<T>::b_read(bfs);

    vsl_b_read(bfs,scale);
    set_scale_step(scale);
    break;
   default:
    vcl_cerr << "I/O ERROR: vimt_gaussian_pyramid_builder_2d_general<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#define VIMT_GAUSSIAN_PYRAMID_BUILDER_2D_GENERAL_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string vimt_gaussian_pyramid_builder_2d_general<T >::is_a() const \
{  return vcl_string("vimt_gaussian_pyramid_builder_2d_general<" #T ">"); }\
template class vimt_gaussian_pyramid_builder_2d_general<T >


#endif // vimt_gaussian_pyramid_builder_2d_general_txx_
