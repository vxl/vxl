// This is mul/mil/mil_sample_grid_2d.txx
#ifndef mil_sample_grid_2d_txx_
#define mil_sample_grid_2d_txx_
//:
// \file
// \brief Profile sampling functions for 2D images
// \author Tim Cootes

#include "mil_sample_grid_2d.h"

#include <mil/mil_bilin_interp_2d.h>
#include <vnl/vnl_vector.h>

//: True if p clearly inside the image
inline bool mil_point_in_image(const vgl_point_2d<double>& p, const mil_image_2d& image)
{
  if (p.x()<1) return false;
  if (p.y()<1) return false;
  if (p.x()+2>image.nx()) return false;
  if (p.y()+2>image.ny()) return false;
  return true;
}

//: True if grid of size nu * nv (in steps of u,v) is entirely in the image.
//  p defines centre of one size.
inline bool mil_grid_in_image(const vgl_point_2d<double>& p,
                              const vgl_vector_2d<double>& u,
                              const vgl_vector_2d<double>& v,
                              int nu, int nv,
                              const mil_image_2d& image)
{
  vgl_vector_2d<double> u1=(nu-1)*u;
  vgl_vector_2d<double> v1=(nv-1)*v;
  if (!mil_point_in_image(image.world2im()(p),image)) return false;
  if (!mil_point_in_image(image.world2im()(p+u1),image)) return false;
  if (!mil_point_in_image(image.world2im()(p+v1),image)) return false;
  if (!mil_point_in_image(image.world2im()(p+u1+v1),image)) return false;

  return true;
}

//: Sample grid p+i.u+j.v using bilinear interpolation.
//  Profile points are p+i.u+j.v, where i=[0..nu-1],j=[0..nv-1]
//  Vector v is resized to nu*nv*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction v first
template <class imType, class vecType>
void mil_sample_grid_2d(vnl_vector<vecType>& vec,
                        const mil_image_2d_of<imType>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        const vgl_vector_2d<double>& v,
                        int nu, int nv)
{
  if (image.world2im().form()!=mil_transform_2d::Projective)
  {
    vgl_point_2d<double> im_p0 = image.world2im()(p);
    vgl_vector_2d<double> im_u = image.world2im()(p+u)-im_p0;
    vgl_vector_2d<double> im_v = image.world2im()(p+v)-im_p0;

    if (mil_grid_in_image(p,u,v,nu,nv,image))
      mil_sample_grid_2d_ic_no_checks(vec,image,im_p0,im_u,im_v,nu,nv);
    else
      mil_sample_grid_2d_ic_safe(vec,image,im_p0,im_u,im_v,nu,nv);

    return;
  }

  // Otherwise do fiddly projective calculations
  if (mil_grid_in_image(p,u,v,nu,nv,image))
    mil_sample_grid_2d_no_checks(vec,image,p,u,v,nu,nv);
  else
    mil_sample_grid_2d_safe(vec,image,p,u,v,nu,nv);
}

//: Sample grid, using safe bilinear interpolation.
//  Profile points are p+i.u+j.v, where i=[0..nu-1],j=[0..nv-1]
//  Vector v is resized to nu*nv*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction v first
//  Points outside image return zero.
template <class imType, class vecType>
void mil_sample_grid_2d_safe(vnl_vector<vecType>& vec,
                             const mil_image_2d_of<imType>& image,
                             const vgl_point_2d<double>& p0,
                             const vgl_vector_2d<double>& u,
                             const vgl_vector_2d<double>& v,
                             int nu, int nv)
{
  int np = image.n_planes();
  int nx = image.nx();
  int ny = image.ny();
  int xstep = image.xstep();
  int ystep = image.ystep();

  vec.set_size(nu*nv*np);
  vecType* vc = vec.begin();

  vgl_point_2d<double> p1 = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<nu;++i)
    {
      vgl_point_2d<double> p = p1;
      // Sample each row (along v)
      for (int j=0;j<nv;++j)
      {
        vgl_point_2d<double> im_p = image.world2im()(p);
        *vc = mil_safe_bilin_interp_2d(im_p.x(),im_p.y(),plane0,nx,ny,xstep,ystep);
        vc++;
        p+=v;
      }
      p1+=u;
    }
  }
  else
  {
    for (int i=0;i<nu;++i)
    {
      vgl_point_2d<double> p = p1;
      for (int j=0;j<nv;++j)
      {
        vgl_point_2d<double> im_p = image.world2im()(p);
        for (int k=0;k<np;++k)
        {
          *vc = mil_safe_bilin_interp_2d(im_p.x(),im_p.y(),image.plane(k),nx,ny,xstep,ystep);
          vc++;
        }
         p+=v;
      }
      p1+=u;
    }
  }
}

//: Sample grid, using bilinear interpolation (no checks).
//  Profile points are p+i.u+j.v, where i=[0..nu-1],j=[0..nv-1]
//  Vector v is resized to nu*nv*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction v first
//  Points outside image return zero.
template <class imType, class vecType>
void mil_sample_grid_2d_no_checks(vnl_vector<vecType>& vec,
                                  const mil_image_2d_of<imType>& image,
                                  const vgl_point_2d<double>& p0,
                                  const vgl_vector_2d<double>& u,
                                  const vgl_vector_2d<double>& v,
                                  int nu, int nv)
{
  int np = image.n_planes();
  int xstep = image.xstep();
  int ystep = image.ystep();

  vec.set_size(nu*nv*np);
  vecType* vc = vec.begin();

  vgl_point_2d<double> p1 = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<nu;++i)
    {
      vgl_point_2d<double> p = p1;
      // Sample each row (along v)
      for (int j=0;j<nv;++j)
      {
        vgl_point_2d<double> im_p = image.world2im()(p);
        *vc = mil_bilin_interp_2d(im_p.x(),im_p.y(),plane0,xstep,ystep);
        vc++;
        p+=v;
      }
      p1+=u;
    }
  }
  else
  {
    for (int i=0;i<nu;++i)
    {
      vgl_point_2d<double> p = p1;
      for (int j=0;j<nv;++j)
      {
        vgl_point_2d<double> im_p = image.world2im()(p);
        for (int k=0;k<np;++k)
        {
          *vc = mil_bilin_interp_2d(im_p.x(),im_p.y(),image.plane(k),xstep,ystep);
          vc++;
        }
        p+=v;
      }
      p1+=u;
    }
  }
}

//: Sample grid, using safe bilinear interpolation (points in image co-ordinates).
//  Grid points are p+i.u+j.v, where i=[0..nu-1],j=[0..nv-1]
//  Vector v is resized to nu*nv*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction v first
//  Points outside image return zero.
template <class imType, class vecType>
void mil_sample_grid_2d_ic_safe(vnl_vector<vecType>& vec,
                                const mil_image_2d_of<imType>& image,
                                const vgl_point_2d<double>& p0,
                                const vgl_vector_2d<double>& u,
                                const vgl_vector_2d<double>& v,
                                int nu, int nv)
{
  int np = image.n_planes();
  int nx = image.nx();
  int ny = image.ny();
  int xstep = image.xstep();
  int ystep = image.ystep();

  vec.set_size(nu*nv*np);
  vecType* vc = vec.begin();

  vgl_point_2d<double> p1 = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<nu;++i)
    {
      vgl_point_2d<double> p = p1;
      // Sample each row (along v)
      for (int j=0;j<nv;++j)
      {
        *vc = mil_safe_bilin_interp_2d(p.x(),p.y(),plane0,nx,ny,xstep,ystep);
        vc++;
        p+=v;
      }
      p1+=u;
    }
  }
  else
  {
    for (int i=0;i<nu;++i)
    {
      vgl_point_2d<double> p = p1;
      for (int j=0;j<nv;++j)
      {
        for (int k=0;k<np;++k)
        {
          *vc = mil_safe_bilin_interp_2d(p.x(),p.y(),image.plane(k),nx,ny,xstep,ystep);
          vc++;
        }
        p+=v;
      }
      p1+=u;
    }
  }
}

//: Sample grid, using bilinear interpolation (no checks) (points in image co-ordinates).
//  Profile points are p+i.u+j.v, where i=[0..nu-1],j=[0..nv-1]
//  Vector v is resized to nu*nv*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction v first
//  Points are assumed to be inside the image.
template <class imType, class vecType>
void mil_sample_grid_2d_ic_no_checks(vnl_vector<vecType>& vec,
                                     const mil_image_2d_of<imType>& image,
                                     const vgl_point_2d<double>& p0,
                                     const vgl_vector_2d<double>& u,
                                     const vgl_vector_2d<double>& v,
                                     int nu, int nv)
{
  int np = image.n_planes();
  int xstep = image.xstep();
  int ystep = image.ystep();

  vec.set_size(nu*nv*np);
  vecType* vc = vec.begin();

  vgl_point_2d<double> p1 = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<nu;++i)
    {
      vgl_point_2d<double> p = p1;
      // Sample each row (along v)
      for (int j=0;j<nv;++j)
      {
        *vc = mil_bilin_interp_2d(p.x(),p.y(),plane0,xstep,ystep);
        vc++;
        p+=v;
      }
      p1+=u;
    }
  }
  else
  {
    for (int i=0;i<nu;++i)
    {
      vgl_point_2d<double> p = p1;
      for (int j=0;j<nv;++j)
      {
        for (int k=0;k<np;++k)
        {
          *vc = mil_bilin_interp_2d(p.x(),p.y(),image.plane(k),xstep,ystep);
          vc++;
        }
        p+=v;
      }
      p1+=u;
    }
  }
}

#undef MIL_SAMPLE_GRID_2D_INSTANTIATE
#define MIL_SAMPLE_GRID_2D_INSTANTIATE( imType, vecType ) \
template void mil_sample_grid_2d(vnl_vector<vecType >& vec, \
                                 const mil_image_2d_of<imType >& image, \
                                 const vgl_point_2d<double >& p, \
                                 const vgl_vector_2d<double >& u, \
                                 const vgl_vector_2d<double >& v, \
                                 int nu, int nv); \
template void mil_sample_grid_2d_safe(vnl_vector<vecType >& vec, \
                                      const mil_image_2d_of<imType >& image, \
                                      const vgl_point_2d<double >& p, \
                                      const vgl_vector_2d<double >& u, \
                                      const vgl_vector_2d<double >& v, \
                                      int nu, int nv); \
template void mil_sample_grid_2d_no_checks(vnl_vector<vecType >& vec, \
                                           const mil_image_2d_of<imType >& image, \
                                           const vgl_point_2d<double >& p, \
                                           const vgl_vector_2d<double >& u, \
                                           const vgl_vector_2d<double >& v, \
                                           int nu, int nv); \
template void mil_sample_grid_2d_ic_safe(vnl_vector<vecType >& vec, \
                                         const mil_image_2d_of<imType >& image, \
                                         const vgl_point_2d<double >& p, \
                                         const vgl_vector_2d<double >& u, \
                                         const vgl_vector_2d<double >& v, \
                                         int nu, int nv); \
template void mil_sample_grid_2d_ic_no_checks(vnl_vector<vecType >& vec, \
                                              const mil_image_2d_of<imType >& image, \
                                              const vgl_point_2d<double >& p0, \
                                              const vgl_vector_2d<double >& u, \
                                              const vgl_vector_2d<double >& v, \
                                              int nu, int nv)

#endif // mil_sample_grid_2d_txx_
