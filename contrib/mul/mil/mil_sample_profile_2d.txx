// This is mul/mil/mil_sample_profile_2d.txx
#ifndef mil_sample_profile_2d_txx_
#define mil_sample_profile_2d_txx_
//:
// \file
// \brief Profile sampling functions for 2D images
// \author Tim Cootes

#include "mil_sample_profile_2d.h"

#include <mil/mil_bilin_interp_2d.h>
#include <vnl/vnl_vector.h>

inline bool mil_profile_in_image(const vgl_point_2d<double>& p0,
                                 const vgl_point_2d<double>& p1,
                                 const mil_image_2d& image)
{
  if (p0.x()<1) return false;
  if (p0.y()<1) return false;
  if (p0.x()+2>image.nx()) return false;
  if (p0.y()+2>image.ny()) return false;
  if (p1.x()<1) return false;
  if (p1.y()<1) return false;
  if (p1.x()+2>image.nx()) return false;
  if (p1.y()+2>image.ny()) return false;

  return true;
}

//: Sample along profile, using bilinear interpolation
//  Profile points are p+iu, where i=[0..n-1].
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
template <class imType, class vecType>
void mil_sample_profile_2d(vnl_vector<vecType>& v,
                           const mil_image_2d_of<imType>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u,
                           int n)
{
  // Check that all the profile points are within the image.
  vgl_point_2d<double> im_p0 = image.world2im()(p);
  vgl_point_2d<double> im_p1 = image.world2im()(p+(n-1)*u);

  if (image.world2im().form()!=mil_transform_2d::Projective)
  {
    // Sample along profile between im_p0 and im_p1
    mil_sample_profile_2d_ic(v,image,im_p0,im_p1,n);
    return;
  }

  // Otherwise do fiddly projective calculations
  if (mil_profile_in_image(im_p0,im_p1,image))
    mil_sample_profile_2d_no_checks(v,image,p,u,n);
  else
    mil_sample_profile_2d_safe(v,image,p,u,n);
}

//: Sample along profile, using safe bilinear interpolation
//  Profile points are p+iu, where i=[0..n-1].
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void mil_sample_profile_2d_safe(vnl_vector<vecType>& vec,
                                const mil_image_2d_of<imType>& image,
                                const vgl_point_2d<double>& p0,
                                const vgl_vector_2d<double>& u,
                                int n)
{
  int np = image.n_planes();
  int nx = image.nx();
  int ny = image.ny();
  int xstep = image.xstep();
  int ystep = image.ystep();

  vec.set_size(n*np);
  vecType* v = vec.begin();

  vgl_point_2d<double> p = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<n;++i)
    {
      vgl_point_2d<double> im_p = image.world2im()(p);
      v[i] = mil_safe_bilin_interp_2d(im_p.x(),im_p.y(),plane0,nx,ny,xstep,ystep);
      p+=u;
    }
  }
  else
  {
    for (int i=0;i<n;++i)
    {
      for (int j=0;j<np;++j)
      {
        vgl_point_2d<double> im_p = image.world2im()(p);
        *v = mil_safe_bilin_interp_2d(im_p.x(),im_p.y(),image.plane(j),nx,ny,xstep,ystep);
        v++;
      }
      p+=u;
    }
  }
}

//: Sample along profile, using bilinear interpolation (no checks)
//  Profile points are p+iu, where i=[0..n-1], all of which are
//  assumed to be sufficiently inside the image that bilinear interpolation
//  will be safe.
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
template <class imType, class vecType>
void mil_sample_profile_2d_no_checks(vnl_vector<vecType>& vec,
                                     const mil_image_2d_of<imType>& image,
                                     const vgl_point_2d<double>& p0,
                                     const vgl_vector_2d<double>& u,
                                     int n)
{
  int np = image.n_planes();
  int xstep = image.xstep();
  int ystep = image.ystep();

  vec.set_size(n*np);
  vecType* v = vec.begin();

  vgl_point_2d<double> p = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<n;++i)
    {
      vgl_point_2d<double> im_p = image.world2im()(p);
      v[i] = mil_bilin_interp_2d(im_p.x(),im_p.y(),plane0,xstep,ystep);
      p+=u;
    }
  }
  else
  {
    for (int i=0;i<n;++i)
    {
      for (int j=0;j<np;++j)
      {
        vgl_point_2d<double> im_p = image.world2im()(p);
        *v = mil_bilin_interp_2d(im_p.x(),im_p.y(),image.plane(j),xstep,ystep);
        v++;
      }
      p+=u;
    }
  }
}

//: Sample along profile, using safe bilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void mil_sample_profile_2d_ic_safe(vnl_vector<vecType>& vec,
                                   const mil_image_2d_of<imType>& image,
                                   const vgl_point_2d<double>& p0,
                                   const vgl_point_2d<double>& p1,
                                   int n)
{
  vgl_vector_2d<double> u;
  if (n>1) u = (p1-p0)/(n-1);

  int np = image.n_planes();
  int xstep = image.xstep();
  int ystep = image.ystep();
  int nx = image.nx();
  int ny = image.ny();

  vec.set_size(n*np);
  vecType* v = vec.begin();

  vgl_point_2d<double> p = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<n;++i)
    {
      v[i] = mil_safe_bilin_interp_2d(p.x(),p.y(),plane0,nx,ny,xstep,ystep);
      p+=u;
    }
  }
  else
  {
    for (int i=0;i<n;++i)
    {
      for (int j=0;j<np;++j)
      {
        *v = mil_safe_bilin_interp_2d(p.x(),p.y(),image.plane(j),nx,ny,xstep,ystep);
        v++;
      }
      p+=u;
    }
  }
}


//: Sample along profile, using safe bilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void mil_sample_profile_2d_ic(vnl_vector<vecType>& vec,
                              const mil_image_2d_of<imType>& image,
                              const vgl_point_2d<double>& p0,
                              const vgl_point_2d<double>& p1,
                              int n)
{
  if (!mil_profile_in_image(p0,p1,image))
  {
    mil_sample_profile_2d_ic_safe(vec,image,p0,p1,n);
    return;
  }

  vgl_vector_2d<double> u;
  if (n>1) u = (p1-p0)/(n-1);

  int np = image.n_planes();
  int xstep = image.xstep();
  int ystep = image.ystep();

  vec.set_size(n*np);
  vecType* v = vec.begin();

  vgl_point_2d<double> p = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<n;++i)
    {
      v[i] = mil_bilin_interp_2d(p.x(),p.y(),plane0,xstep,ystep);
      p+=u;
    }
  }
  else
  {
    for (int i=0;i<n;++i)
    {
      for (int j=0;j<np;++j)
      {
        *v = mil_bilin_interp_2d(p.x(),p.y(),image.plane(j),xstep,ystep);
        v++;
      }
      p+=u;
    }
  }
}

#undef MIL_SAMPLE_PROFILE_2D_INSTANTIATE
#define MIL_SAMPLE_PROFILE_2D_INSTANTIATE( imType, vecType ) \
template void mil_sample_profile_2d(vnl_vector<vecType >& v, \
                                    const mil_image_2d_of<imType >& image, \
                                    const vgl_point_2d<double >& p, \
                                    const vgl_vector_2d<double >& u, \
                                    int n); \
template void mil_sample_profile_2d_safe(vnl_vector<vecType >& v, \
                                         const mil_image_2d_of<imType >& image, \
                                         const vgl_point_2d<double >& p, \
                                         const vgl_vector_2d<double >& u, \
                                         int n); \
template void mil_sample_profile_2d_no_checks(vnl_vector<vecType >& v, \
                                              const mil_image_2d_of<imType >& image, \
                                              const vgl_point_2d<double >& p, \
                                              const vgl_vector_2d<double >& u, \
                                              int n); \
template void mil_sample_profile_2d_ic_safe(vnl_vector<vecType >& v, \
                                            const mil_image_2d_of<imType >& image, \
                                            const vgl_point_2d<double >& p0, \
                                            const vgl_point_2d<double >& p1, \
                                            int n); \
template void mil_sample_profile_2d_ic(vnl_vector<vecType >& v, \
                                       const mil_image_2d_of<imType >& image, \
                                       const vgl_point_2d<double >& p0, \
                                       const vgl_point_2d<double >& p1, \
                                       int n)

#endif // mil_sample_profile_2d_txx_
