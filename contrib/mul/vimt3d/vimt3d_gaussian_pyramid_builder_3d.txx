// This is mul/vimt3d/vimt3d_gaussian_pyramid_builder_3d.txx
#ifndef vimt3d_gaussian_pyramid_builder_3d_txx_
#define vimt3d_gaussian_pyramid_builder_3d_txx_
//:
// \file
// \brief Class to build Gaussian pyramids of vimt3d_image_3d_of<T>
// \author Tim Cootes

#include "vimt3d_gaussian_pyramid_builder_3d.h"

#include <vcl_cstdlib.h>
#include <vcl_string.h>

#include <vimt/vimt_image_pyramid.h>
#include <vil3d/algo/vil3d_gauss_reduce.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>

//=======================================================================

template<class T>
vimt3d_gaussian_pyramid_builder_3d<T>::vimt3d_gaussian_pyramid_builder_3d()
: max_levels_(99),uniform_reduction_(false),filter_width_(5)
{
  set_min_size(5, 5, 5);
}

//=======================================================================

template<class T>
vimt3d_gaussian_pyramid_builder_3d<T>::~vimt3d_gaussian_pyramid_builder_3d()
{
}

//=======================================================================
//: Define maximum number of levels to build
//  Limits levels built in subsequent calls to build()
template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::set_max_levels(int max_l)
{
  if (max_l<1)
  {
    vcl_cerr<<"vimt3d_gaussian_pyramid_builder_3d<T>::setMaxLevels() param is "
            <<max_l<<", must be >=1\n";
    vcl_abort();
  }
  max_levels_ = max_l;
}

//: Get the current maximum number levels allowed
template<class T>
int vimt3d_gaussian_pyramid_builder_3d<T>::max_levels() const
{
  return max_levels_;
}

//: Select number of levels to use
template<class T>
int vimt3d_gaussian_pyramid_builder_3d<T>::n_levels(const vimt3d_image_3d_of<T>& base_image) const
{
  int ni = base_image.image().ni();
  int nj = base_image.image().nj();
  int nk = base_image.image().nk();
  double dx,dy,dz;
  get_pixel_size(dx,dy,dz,base_image);
  // Compute number of levels to pyramid so that top is no less
  // than min_x_size_ x min_y_size_ x min_z_size_
  int max_levels = 0;
  while ((ni>=int(min_x_size_)) && (nj>=int(min_y_size_)) && (nk>=int(min_z_size_)))
  {
    if (uniform_reduction_)
    {
      ni = (ni+1)/2; dx*=2;
      nj = (nj+1)/2; dy*=2;
      nk = (nk+1)/2; dz*=2;
    }
    else if (dz*dz/(dx*dx)>2.0)
    {
      // Pixels large in z, so don't smooth them
      ni = (ni+1)/2; dx*=2;
      nj = (nj+1)/2; dy*=2;
    }
    else if (dy*dy/(dx*dx)>2.0)
    {
      // Pixels large in y, so don't smooth them
      ni = (ni+1)/2; dx*=2;
      nk = (nk+1)/2; dz*=2;
    }
    else if (dx*dx/(dy*dy)>2.0)
    {
      // Pixels large in x, so don't smooth them
      nj = (nj+1)/2; dy*=2;
      nk = (nk+1)/2; dz*=2;
    }
    else
    {
      ni = (ni+1)/2; dx*=2;
      nj = (nj+1)/2; dy*=2;
      nk = (nk+1)/2; dz*=2;
    }
    max_levels++;
  }
  if (max_levels<1) max_levels = 1;
  if (max_levels>max_levels_)
    max_levels=max_levels_;

  return max_levels;
}

//=======================================================================
//: Create new (empty) pyramid on heap
//  Caller responsible for its deletion
template<class T>
vimt_image_pyramid* vimt3d_gaussian_pyramid_builder_3d<T>::new_image_pyramid() const
{
  return new vimt_image_pyramid;
}


//=======================================================================
//: Scale step between levels
template<class T>
double vimt3d_gaussian_pyramid_builder_3d<T>::scale_step() const
{
  return 2.0;
}

//: Set current filter width (must be 3 or 5 at present)
template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::set_filter_width(unsigned w)
{
  assert(w==5);
  filter_width_ = w;
}

//=======================================================================
//: Smooth and subsample src_im to produce dest_im
//  Applies 1-5-8-5-1 filter in x and y, then samples
//  every other pixel.
template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::gauss_reduce(vimt3d_image_3d_of<T>& dest_im,
                                                         vimt3d_image_3d_of<T>const& src_im) const
{
  // Assume filter width is 5 for the moment.
  if (filter_width_!=5)
  {
    vcl_cerr<<"vimt3d_gaussian_pyramid_builder_3d<T>::gauss_reduce()\n"
            <<" Cannot cope with filter width of "<<filter_width_<<'\n';
    vcl_abort();
  }

  double dx,dy,dz;
  get_pixel_size(dx,dy,dz,src_im);

  vimt3d_transform_3d scaling;

  if (uniform_reduction_)
  {
    vil3d_gauss_reduce(src_im.image(),dest_im.image(),work_im1_,work_im2_);
    scaling.set_zoom_only(0.5,0.5,0.5,0,0,0);
    dest_im.set_world2im(scaling * src_im.world2im());

    return;
  }

  // If dz is much larger than dx, then don't subsample in that direction
  if (dz*dz/(dx*dx)>2.0)
  {
    vil3d_gauss_reduce_ij(src_im.image(),dest_im.image(),work_im1_);
    scaling.set_zoom_only(0.5,0.5,1.0,0,0,0);
    dest_im.set_world2im(scaling * src_im.world2im());
  }
  else if (dy*dy/(dx*dx)>2.0)
  {
    vil3d_gauss_reduce_ik(src_im.image(),dest_im.image(),work_im1_);
    scaling.set_zoom_only(0.5,1.0,0.5,0,0,0);
    dest_im.set_world2im(scaling * src_im.world2im());
  }
  else if (dx*dx/(dy*dy)>2.0)
  {
    vil3d_gauss_reduce_jk(src_im.image(),dest_im.image(),work_im1_);
    scaling.set_zoom_only(1.0,0.5,0.5,0,0,0);
    dest_im.set_world2im(scaling * src_im.world2im());
  }
  else
  {
    vil3d_gauss_reduce(src_im.image(),dest_im.image(),work_im1_,work_im2_);

    scaling.set_zoom_only(0.5,0.5,0.5,0,0,0);
    dest_im.set_world2im(scaling * src_im.world2im());
  }
}

//=======================================================================
//: Deletes all data in im_pyr
template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::emptyPyr(vimt_image_pyramid& im_pyr) const
{
  for (int i=0; i<im_pyr.n_levels();++i)
    delete im_pyr.data()[i];
}

//=======================================================================
//: Checks pyramid has at least n levels
template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::checkPyr(vimt_image_pyramid& im_pyr,  int n_levels) const
{
  const int got_levels = im_pyr.n_levels();
  if (got_levels >= n_levels && im_pyr(0).is_a()==work_im1_.is_a())
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
    im_pyr.data()[i] = new vimt3d_image_3d_of<T>;
}

//: Set the minimum size of the top layer of the pyramid
template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::set_min_size(unsigned X, unsigned Y, unsigned Z)
{ min_x_size_ = X; min_y_size_ = Y; min_z_size_ = Z; }

//=======================================================================
//: Build pyramid
template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::build(vimt_image_pyramid& image_pyr,
                                                  vimt_image const& im) const
{
  // Check that the image is a 3d image
  assert(im.is_class("vimt3d_image_3d"));

  // Cast to the appropriate class
  const vimt3d_image_3d &im3d = static_cast<const vimt3d_image_3d &>(im);

  //  Require image vimt3d_image_3d_of<T>
  assert(im3d.image_base().is_a()==work_im1_.is_a());

  const vimt3d_image_3d_of<T>& base_image = static_cast<const vimt3d_image_3d_of<T>&>(im3d);

  int ni = base_image.image().ni();
  int nj = base_image.image().nj();
  int nk = base_image.image().nk();

  int max_levels=n_levels(base_image);

  work_im1_.set_size(ni,nj,nk);
  work_im2_.set_size(ni,nj,nk);

  // Set up image pyramid
  checkPyr(image_pyr,max_levels);

  vimt3d_image_3d_of<T>& im0 = static_cast<vimt3d_image_3d_of<T>&>(image_pyr(0));

  // Shallow copy of part of base_image
  im0=base_image;

  int i;
  for (i=1;i<max_levels;i++)
  {
    vimt3d_image_3d_of<T>& im_i0 = static_cast<vimt3d_image_3d_of<T>&>(image_pyr(i));
    vimt3d_image_3d_of<T>& im_i1 = static_cast<vimt3d_image_3d_of<T>&>(image_pyr(i-1));

    gauss_reduce(im_i0,im_i1);
  }

  // Estimate width of pixels in base image
  vgl_point_3d<double>  c0(0.5*(ni-1),0.5*(nj-1),0.5*(nk-1));
  vgl_point_3d<double>  c1 = c0 + vgl_vector_3d<double> (1,1,1);
  vimt3d_transform_3d im2world = base_image.world2im().inverse();
  vgl_vector_3d<double>  dw = im2world(c1) - im2world(c0);

  double base_pixel_width = dw.length()/vcl_sqrt(3.0);
  double scale_step = 2.0;

  image_pyr.set_widths(base_pixel_width,scale_step);
}

//: Compute real world size of pixel
template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::get_pixel_size(double &dx, double& dy, double& dz,
                                                           const vimt3d_image_3d_of<T>& image) const
{
  // Estimate width of pixels in base image
  vgl_point_3d<double>  c0(0,0,0);
  vgl_point_3d<double>  cx(1,0,0);
  vgl_point_3d<double>  cy(0,1,0);
  vgl_point_3d<double>  cz(0,0,1);
  vimt3d_transform_3d im2world = image.world2im().inverse();
  dx = (im2world(cx) - im2world(c0)).length();
  dy = (im2world(cy) - im2world(c0)).length();
  dz = (im2world(cz) - im2world(c0)).length();
}

//=======================================================================
//: Extend pyramid
template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::extend(vimt_image_pyramid& image_pyr) const
{
  //  Require image vimt3d_image_3d_of<T>
  assert(image_pyr(0).is_a() == work_im1_.is_a());

  assert(image_pyr.scale_step() == scale_step());

  vimt3d_image_3d_of<T>& im_base = static_cast<vimt3d_image_3d_of<T>&>(image_pyr(0));
  int ni = im_base.image().ni();
  int nj = im_base.image().nj();
  int nk = im_base.image().nk();

  int max_levels=n_levels(static_cast<const vimt3d_image_3d_of<T>&>(image_pyr(0)));

  work_im1_.set_size(ni,nj,nk);
  work_im2_.set_size(ni,nj,nk);

  // Set up image pyramid
  int oldsize = image_pyr.n_levels();
  if (oldsize<max_levels) // only extend, if it isn't already tall enough
  {
    image_pyr.data().resize(max_levels);

    int i;
    for (i=oldsize;i<max_levels;++i)
      image_pyr.data()[i] = new vimt3d_image_3d_of<T>;

    for (i=oldsize;i<max_levels;i++)
    {
      vimt3d_image_3d_of<T>& im_i0 = static_cast<vimt3d_image_3d_of<T>&>(image_pyr(i));
      vimt3d_image_3d_of<T>& im_i1 = static_cast<vimt3d_image_3d_of<T>&>(image_pyr(i-1));

      gauss_reduce(im_i0,im_i1);
    }
  }
}

//=======================================================================

template<class T>
bool vimt3d_gaussian_pyramid_builder_3d<T>::is_class(vcl_string const& s) const
{
  return s==vimt3d_gaussian_pyramid_builder_3d<T>::is_a() || vimt_image_pyramid_builder::is_class(s);
}

//=======================================================================

template<class T>
short vimt3d_gaussian_pyramid_builder_3d<T>::version_no() const
{
  return 1;
}

//=======================================================================

template<class T>
vimt_image_pyramid_builder* vimt3d_gaussian_pyramid_builder_3d<T>::clone() const
{
  return new vimt3d_gaussian_pyramid_builder_3d<T>(*this);
}

//=======================================================================

template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::print_summary(vcl_ostream&) const
{
}

//=======================================================================

template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,max_levels_);
  vsl_b_write(bfs,uniform_reduction_);
  vsl_b_write(bfs,filter_width_);
}

//=======================================================================

template<class T>
void vimt3d_gaussian_pyramid_builder_3d<T>::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs,max_levels_);
    vsl_b_read(bfs,uniform_reduction_);
    vsl_b_read(bfs,filter_width_);
    break;
  default:
    vcl_cerr << "I/O ERROR: vimt3d_gaussian_pyramid_builder_3d<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#undef VIMT3D_GAUSSIAN_PYRAMID_BUILDER_3D_INSTANTIATE
#define VIMT3D_GAUSSIAN_PYRAMID_BUILDER_3D_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string vimt3d_gaussian_pyramid_builder_3d<T >::is_a() const \
{ return vcl_string("vimt3d_gaussian_pyramid_builder_3d<" #T ">"); } \
template class vimt3d_gaussian_pyramid_builder_3d<T >

#endif // vimt3d_gaussian_pyramid_builder_3d_txx_
