#ifndef mil3d_sample_profile_3d_txx_
#define mil3d_sample_profile_3d_txx_

//: \file
//  \brief Profile sampling functions for 3D images
//  \author Tim Cootes

#include <mil3d/mil3d_sample_profile_3d.h>
#include <mil3d/mil3d_trilin_interp_3d.h>
#include <vnl/vnl_vector.h>

inline bool mil3d_profile_in_image(const vgl_point_3d<double>& p0,
                                 const vgl_point_3d<double>& p1,
                                 const mil3d_image_3d& image)
{
  if (p0.x()<1) return false;
  if (p0.y()<1) return false;
  if (p0.z()<1) return false;
  if (p0.x()+2>image.nx()) return false;
  if (p0.y()+2>image.ny()) return false;
  if (p0.z()+2>image.nz()) return false;
  if (p1.x()<1) return false;
  if (p1.y()<1) return false;
  if (p1.z()<1) return false;
  if (p1.x()+2>image.nx()) return false;
  if (p1.y()+2>image.ny()) return false;
  if (p1.z()+2>image.nz()) return false;

  return true;
}

//: Sample along profile, using trilinear interpolation
//  Profile points are p+iu, where i=[0..n-1].
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
template <class imType, class vecType>
void mil3d_sample_profile_3d(vnl_vector<vecType>& v,
                           const mil3d_image_3d_of<imType>& image,
                           const vgl_point_3d<double>& p,
                           const vgl_vector_3d<double>& u,
                           int n)
{
  // Check that all the profile points are within the image.
  vgl_point_3d<double> im_p0 = image.world2im()(p);
  vgl_point_3d<double> im_p1 = image.world2im()(p+(n-1)*u);

  // Sample along profile between im_p0 and im_p1
  mil3d_sample_profile_3d_ic(v,image,im_p0,im_p1,n);
}


//: Sample along profile, using safe trilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void mil3d_sample_profile_3d_ic_safe(vnl_vector<vecType>& vec,
                                   const mil3d_image_3d_of<imType>& image,
                                   const vgl_point_3d<double>& p0,
                                   const vgl_point_3d<double>& p1,
                                   int n)
{
  vgl_vector_3d<double> u;
  if (n>1) u = (p1-p0)/(n-1);

  int np = image.n_planes();
  int xstep = image.xstep();
  int ystep = image.ystep();
  int zstep = image.zstep();
  int nx = image.nx();
  int ny = image.ny();
  int nz = image.nz();

  vec.resize(n*np);
  vecType* v = vec.begin();

  vgl_point_3d<double> p = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<n;++i)
    {
      v[i] = mil3d_safe_trilin_interp_3d(p.x(),p.y(),p.z(),
			                                  plane0,nx,ny,nz,xstep,ystep,zstep);
      p+=u;
    }
  }
  else
  {
    for (int i=0;i<n;++i)
    {
      for (int j=0;j<np;++j)
      {
        *v = mil3d_safe_trilin_interp_3d(p.x(),p.y(),p.z(),
				                                image.plane(j),nx,ny,nz,
																				xstep,ystep,zstep);
        v++;
      }
      p+=u;
    }
  }
}


//: Sample along profile, using safe trilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void mil3d_sample_profile_3d_ic(vnl_vector<vecType>& vec,
                              const mil3d_image_3d_of<imType>& image,
                              const vgl_point_3d<double>& p0,
                              const vgl_point_3d<double>& p1,
                              int n)
{
  if (!mil3d_profile_in_image(p0,p1,image))
  {
    mil3d_sample_profile_3d_ic_safe(vec,image,p0,p1,n);
    return;
  }

  vgl_vector_3d<double> u;
  if (n>1) u = (p1-p0)/(n-1);

  int np = image.n_planes();
  int xstep = image.xstep();
  int ystep = image.ystep();
  int zstep = image.zstep();

  vec.resize(n*np);
  vecType* v = vec.begin();

  vgl_point_3d<double> p = p0;

  if (np==1)
  {
    const imType* plane0 = image.plane(0);
    for (int i=0;i<n;++i)
    {
      v[i] = mil3d_trilin_interp_3d(p.x(),p.y(),p.z(),plane0,xstep,ystep,zstep);
      p+=u;
    }
  }
  else
  {
    for (int i=0;i<n;++i)
    {
      for (int j=0;j<np;++j)
      {
        *v = mil3d_trilin_interp_3d(p.x(),p.y(),p.z(),
				                           image.plane(j),xstep,ystep,zstep);
        v++;
      }
      p+=u;
    }
  }
}

#define MIL3D_SAMPLE_PROFILE_3D_INSTANTIATE( imType, vecType ) \
template void mil3d_sample_profile_3d(vnl_vector<vecType >& v, \
                           const mil3d_image_3d_of<imType >& image, \
                           const vgl_point_3d<double >& p, \
                           const vgl_vector_3d<double >& u, \
                           int n); \
template void mil3d_sample_profile_3d_ic_safe(vnl_vector<vecType >& v, \
                           const mil3d_image_3d_of<imType >& image, \
                           const vgl_point_3d<double >& p0, \
                           const vgl_point_3d<double >& p1, \
                           int n); \
template void mil3d_sample_profile_3d_ic(vnl_vector<vecType >& v, \
                           const mil3d_image_3d_of<imType >& image, \
                           const vgl_point_3d<double >& p0, \
                           const vgl_point_3d<double >& p1, \
                           int n)

#endif // mil3d_sample_profile_3d_txx_
