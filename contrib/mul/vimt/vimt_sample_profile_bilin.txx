// This is mul/vimt/vimt_sample_profile_bilin.txx
#ifndef vimt_sample_profile_bilin_txx_
#define vimt_sample_profile_bilin_txx_
//:
//  \file
//  \brief Profile sampling functions for 2D images
//  \author Tim Cootes

#include "vimt_sample_profile_bilin.h"
#include <vil2/vil2_sample_profile_bilin.h>
#include <vil2/vil2_bilin_interp.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_vector_2d.h>

inline bool vimt_profile_in_image(const vgl_point_2d<double>& p0,
                                  const vgl_point_2d<double>& p1,
                                  const vil2_image_view_base& image)
{
  if (p0.x()<1) return false;
  if (p0.y()<1) return false;
  if (p0.x()+2>image.ni()) return false;
  if (p0.y()+2>image.nj()) return false;
  if (p1.x()<1) return false;
  if (p1.y()<1) return false;
  if (p1.x()+2>image.ni()) return false;
  if (p1.y()+2>image.nj()) return false;

  return true;
}

//: Sample along profile, using bilinear interpolation
//  Profile points are p+iu, where i=[0..n-1].
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
  vec.resize(n*np);
  vecType *v = vec.data_block();

  if (image.world2im().form()!=vimt_transform_2d::Projective)
  {
    // Can do all work in image co-ordinates under an affine transformation
    double dx = (im_p1.x()-im_p0.x())/(n-1);
    double dy = (im_p1.y()-im_p0.y())/(n-1);

    // Sample along profile between im_p0 and im_p1
    vil2_sample_profile_bilin(v,image.image(),im_p0.x(),im_p0.y(),dx,dy,n);
    return;
  }

  // Otherwise do more fiddly projective calculations

  int istep = image.image().istep();
  int jstep = image.image().jstep();
  int pstep = image.image().planestep();
  const imType* plane0 = image.image().top_left_ptr();
  vgl_point_2d<double> im_p, p=p0;
  const vimt_transform_2d& w2i = image.world2im();

  if (vimt_profile_in_image(im_p0,im_p1,image.image_base()))
  {
    if (np==1)
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        v[i] = vil2_bilin_interp(im_p.x(),im_p.y(),plane0,istep,jstep);
      }
    }
    else
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        for (int j=0;j<np;++j,++v)
          *v = vil2_bilin_interp(im_p.x(),im_p.y(),plane0+j*pstep,istep,jstep);
      }
    }
  }
  else
  {
    // Use safe interpolation
    int ni = image.image().ni();
    int nj = image.image().nj();
    if (np==1)
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        v[i] = vil2_safe_bilin_interp(im_p.x(),im_p.y(),plane0,ni,nj,istep,jstep);
      }
    }
    else
    {
      for (int i=0;i<n;++i,p+=u)
      {
        im_p = w2i(p);
        for (int j=0;j<np;++j,++v)
          *v = vil2_safe_bilin_interp(im_p.x(),im_p.y(),plane0+j*pstep,ni,nj,istep,jstep);
      }
    }
  }
}

#define VIMT_SAMPLE_PROFILE_BILIN_INSTANTIATE( imType, vecType ) \
template void vimt_sample_profile_bilin(vnl_vector<vecType >& v, \
                                        const vimt_image_2d_of<imType >& image, \
                                        const vgl_point_2d<double >& p, \
                                        const vgl_vector_2d<double >& u, \
                                        int n)

#endif // vimt_sample_profile_bilin_txx_
