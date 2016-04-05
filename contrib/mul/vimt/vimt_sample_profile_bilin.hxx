// This is mul/vimt/vimt_sample_profile_bilin.hxx
#ifndef vimt_sample_profile_bilin_hxx_
#define vimt_sample_profile_bilin_hxx_
//:
//  \file
//  \brief Profile sampling functions for 2D images
//  \author Tim Cootes

#include "vimt_sample_profile_bilin.h"
#include <vil/vil_sample_profile_bilin.h>
#include <vil/vil_bilin_interp.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

inline bool vimt_profile_in_image(const vgl_point_2d<double>& p0,
                                  const vgl_point_2d<double>& p1,
                                  const vil_image_view_base& image)
{
  return p0.x()>=0 && p0.x()<=image.ni()-1 &&
         p0.y()>=0 && p0.y()<=image.nj()-1 &&
         p1.x()>=0 && p1.x()<=image.ni()-1 &&
         p1.y()>=0 && p1.y()<=image.nj()-1;
}

//: Sample along profile, using bilinear interpolation.
//  Profile points are p+i*u, where i=[0..n-1].
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
template <class imType, class vecType>
void vimt_sample_profile_bilin(vnl_vector<vecType>& vec,
                               const vimt_image_2d_of<imType>& image,
                               const vgl_point_2d<double>& p0,
                               const vgl_vector_2d<double>& u,
                               int n)
{
  // Check that all the profile points are within the image.
  vgl_point_2d<double> im_p0 = image.world2im()(p0);
  vgl_point_2d<double> im_p1 = image.world2im()(p0+(n-1)*u);
  int np = image.image().nplanes();
  vec.set_size(n*np);
  vecType *v = vec.data_block();

  if (image.world2im().form()!=vimt_transform_2d::Projective)
  {
    // Can do all work in image co-ordinates under an affine transformation
    double dx = (im_p1.x()-im_p0.x())/(n-1);
    double dy = (im_p1.y()-im_p0.y())/(n-1);

    // Sample along profile between im_p0 and im_p1
    vil_sample_profile_bilin(v,image.image(),im_p0.x(),im_p0.y(),dx,dy,n);
    return;
  }

  // Otherwise do more fiddly projective calculations

  vgl_point_2d<double> im_p, p=p0;
  const vimt_transform_2d& w2i = image.world2im();

  if (vimt_profile_in_image(im_p0,im_p1,image.image_base()))
  {
    if (np==1)
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        v[i] = vil_bilin_interp(image.image(),im_p.x(),im_p.y());
      }
    }
    else
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        for (int plane=0;plane<np;++plane,++v)
          *v = vil_bilin_interp(image.image(),im_p.x(),im_p.y(),plane);
      }
    }
  }
  else // Use safe interpolation, setting v to zero if outside the image:
  {
    if (np==1)
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        v[i] = vil_bilin_interp_safe(image.image(),im_p.x(),im_p.y());
      }
    }
    else
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        for (int plane=0;plane<np;++plane,++v)
          *v = vil_bilin_interp_safe(image.image(),im_p.x(),im_p.y(),plane);
      }
    }
  }
}


//: Sample along profile, using bilinear interpolation.
//  Profile points are p+i*u, where i=[0..n-1].
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Values outside image are set to NA
template <class imType, class vecType>
void vimt_sample_profile_bilin_edgena(vnl_vector<vecType>& vec,
                                     const vimt_image_2d_of<imType>& image,
                                     const vgl_point_2d<double>& p0,
                                     const vgl_vector_2d<double>& u,
                                     int n)
{
  // Check that all the profile points are within the image.
  vgl_point_2d<double> im_p0 = image.world2im()(p0);
  vgl_point_2d<double> im_p1 = image.world2im()(p0+(n-1)*u);
  int np = image.image().nplanes();
  vec.set_size(n*np);
  vecType *v = vec.data_block();

  if (image.world2im().form()!=vimt_transform_2d::Projective)
  {
    // Can do all work in image co-ordinates under an affine transformation
    double dx = (im_p1.x()-im_p0.x())/(n-1);
    double dy = (im_p1.y()-im_p0.y())/(n-1);

    // Sample along profile between im_p0 and im_p1
    vil_sample_profile_bilin_edgena(v,image.image(),im_p0.x(),im_p0.y(),dx,dy,n);
    return;
  }

  // Otherwise do more fiddly projective calculations

  vgl_point_2d<double> im_p, p=p0;
  const vimt_transform_2d& w2i = image.world2im();

  if (vimt_profile_in_image(im_p0,im_p1,image.image_base()))
  {
    if (np==1)
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        v[i] = vil_bilin_interp(image.image(),im_p.x(),im_p.y());
      }
    }
    else
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        for (int plane=0;plane<np;++plane,++v)
          *v = vil_bilin_interp(image.image(),im_p.x(),im_p.y(),plane);
      }
    }
  }
  else // Use safe interpolation, setting v to NA if outside the image:
  {
    if (np==1)
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        v[i] = vil_bilin_interp_safe_edgena(image.image(),im_p.x(),im_p.y());
      }
    }
    else
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        for (int plane=0;plane<np;++plane,++v)
          *v = vil_bilin_interp_safe_edgena(image.image(),im_p.x(),im_p.y(),plane);
      }
    }
  }
}


#define VIMT_SAMPLE_PROFILE_BILIN_INSTANTIATE( imType, vecType ) \
template void vimt_sample_profile_bilin(vnl_vector<vecType >& v, \
                                        const vimt_image_2d_of<imType >& image, \
                                        const vgl_point_2d<double >& p, \
                                        const vgl_vector_2d<double >& u, \
                                        int n); \
template void vimt_sample_profile_bilin_edgena(vnl_vector<vecType >& v, \
                                               const vimt_image_2d_of<imType >& image, \
                                               const vgl_point_2d<double >& p, \
                                               const vgl_vector_2d<double >& u, \
                                               int n)

#endif // vimt_sample_profile_bilin_hxx_
