//:
// \file
// \brief Grid sampling functions for 2D images
// \author Tim Cootes

#include "ihog_sample_grid_bilin.h"
#include <vil/vil_sample_grid_bilin.h>
#include <vil/vil_bilin_interp.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

inline bool ihog_grid_corner_in_image(const vgl_point_2d<double>& p,
                                      const vil_image_view_base& image)
{
  if (p.x()<1) return false;
  if (p.y()<1) return false;
  if (p.x()+2>image.ni()) return false;
  if (p.y()+2>image.nj()) return false;
  return true;
}

//: Sample grid from image, using bilinear interpolation
//  Grid points are p+i.u+j.v where i=[0..n1-1], j=[0..n2-1]
//  Vector vec is resized to n1*n2*np elements, where np=image.nplanes().
//  vec[0]..vec[np-1] are the values from point p0
//  Samples are taken along direction v first, then along u.
//  Points outside image return zero.
void ihog_sample_grid_bilin(vnl_vector<double>& vec,
                            const ihog_image<float>& image,
                            const vgl_point_2d<double>& p0,
                            const vgl_vector_2d<double>& u,
                            const vgl_vector_2d<double>& v,
                            int n1, int n2)
{
  vgl_point_2d<double> im_p0 = image.world2im()(p0);
  vgl_point_2d<double> im_p1 = image.world2im()(p0+(n1-1)*u);
  vgl_point_2d<double> im_p2 = image.world2im()(p0+(n2-1)*v);
  int np = image.image().nplanes();
  vec.set_size(n1*n2*np);
  double *vec_data = vec.data_block();

  if (image.world2im().form()!=ihog_transform_2d::Projective)
  {
    // Can do all work in image co-ordinates under an affine transformation
    vgl_vector_2d<double> im_u(0,0);
    if (n1>1) im_u = (im_p1-im_p0)/(n1-1);
    vgl_vector_2d<double> im_v(0,0);
    if (n2>1) im_v = (im_p2-im_p0)/(n2-1);

    vil_sample_grid_bilin(vec_data,image.image(),im_p0.x(),im_p0.y(),
                          im_u.x(),im_u.y(),im_v.x(),im_v.y(),n1,n2);

    return;
  }

  // Otherwise do more fiddly projective calculations

  // Check that all the grid points are within the image.
  const ihog_transform_2d& w2i = image.world2im();
  bool all_in_image =
      ihog_grid_corner_in_image(im_p0,image.image()) &&
      ihog_grid_corner_in_image(im_p1,image.image()) &&
      ihog_grid_corner_in_image(im_p2,image.image()) &&
      ihog_grid_corner_in_image(w2i(p0+(n1-1)*u+(n2-1)*v),image.image());

  vgl_point_2d<double> p1=p0;

  const float* plane0 = image.image().top_left_ptr();
  unsigned ni = image.image().ni();
  unsigned nj = image.image().nj();
  std::ptrdiff_t istep = image.image().istep();
  std::ptrdiff_t jstep = image.image().jstep();
  std::ptrdiff_t pstep = image.image().planestep();

  if (all_in_image)
  {
    if (np==1)
    {
      for (int i=0;i<n1;++i,p1+=u)
      {
        vgl_point_2d<double> p=p1;  // Start of j-th row
        for (int j=0; j<n2; ++j,p+=v,++vec_data)
        {
          vgl_point_2d<double> im_p = w2i(p);
          *vec_data = (double)vil_bilin_interp_raw(im_p.x(),im_p.y(),plane0,istep,jstep);
        }
      }
    }
    else
    {
      for (int i=0;i<n1;++i,p1+=u)
      {
        vgl_point_2d<double> p=p1;  // Start of j-th row
        for (int j=0;j<n2;++j,p+=v)
        {
          vgl_point_2d<double> im_p = w2i(p);
          for (int k=0;k<np;++k,++vec_data)
            *vec_data = (double)vil_bilin_interp_raw(im_p.x(),im_p.y(),plane0+k*pstep,istep,jstep);
        }
      }
    }
  }
  else
  {
    // Use safe interpolation
    if (np==1)
    {
      for (int i=0;i<n1;++i,p1+=u)
      {
        vgl_point_2d<double> p=p1;  // Start of j-th row
        for (int j=0;j<n2;++j,p+=v,++vec_data)
        {
          vgl_point_2d<double> im_p = w2i(p);
          *vec_data = (double)vil_bilin_interp_safe(im_p.x(),im_p.y(),plane0,ni,nj,istep,jstep);
        }
      }
    }
    else
    {
      for (int i=0;i<n1;++i,p1+=u)
      {
        vgl_point_2d<double> p=p1;  // Start of j-th row
        for (int j=0;j<n2;++j,p+=v)
        {
          vgl_point_2d<double> im_p = w2i(p);
          for (int k=0;k<np;++k,++vec_data)
            *vec_data = (double)vil_bilin_interp_safe(im_p.x(),im_p.y(),plane0+k*pstep,ni,nj,istep,jstep);
        }
      }
    }
  }
}
