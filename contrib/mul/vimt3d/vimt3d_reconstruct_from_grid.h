// This is mul/vimt3d/vimt3d_reconstruct_from_grid.h
#ifndef vimt3d_reconstruct_from_grid_h_
#define vimt3d_reconstruct_from_grid_h_
//:
// \file
// \brief Reconstructs a sample vector into a grid (mirroring vimt3d_sample_grid_trilin)
// \author Graham Vincent

#include <vimt3d/vimt3d_image_3d_of.h>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_math.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Fill voxel which x,y,z is in with val
//  Image is nx * ny * nz array of T. x,y,z element is data[z*zstep+ystep*y+x*xstep]
//  Bound checks are made
// \param val value at image coordinates x,y,z
// \param add_data adds data to existing values
template<class T>
inline void vimt3d_reconstruct_ic_safe(double val, double x, double y, double z,
                                       T* data, int ni, int nj, int nk,
                                       std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep, bool add_data)
{
  int ix=vnl_math::rnd(x);
  int iy=vnl_math::rnd(y);
  int iz=vnl_math::rnd(z);
  std::ptrdiff_t index = (ix  *xstep) + (iy  *ystep) + (iz  *zstep);

  // now add or set the image values
  if (add_data)
  {
      if (ix>=0 && iy>=0 && iz>=0
          && ix<=(ni-1) && iy<=(nj-1) && iz<(nk-1) )
      {
          data[index] += T(val);
      }
  }
  else
  {
      if (ix>=0 && iy>=0 && iz>=0
          && ix<=(ni-1) && iy<=(nj-1) && iz<(nk-1) )
      {
          data[index] = T(val);
      }
  }

  return;
}


//: Fill voxel which x,y,z is in with val
//  Image is ni * nj * nk array of T. x,y,z element is data[z*zstep+y*ystep+x*xstep]
//  No bound checks are made in release mode
// \param val value at image coordinates x,y,z
// \param add_data adds data to existing values
template<class T>
inline void vimt3d_reconstruct_ic_no_checks(double val, double x, double y, double z,
                                            T* data, int ni, int nj, int nk,
                                            std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep, bool add_data)
{
  int ix=vnl_math::rnd(x);
  int iy=vnl_math::rnd(y);
  int iz=vnl_math::rnd(z);
  assert (ix >= 0 && ix < ni);
  assert (iy >= 0 && iy < nj);
  assert (iz >= 0 && iz < nk);
  std::ptrdiff_t index = (ix  *xstep) + (iy  *ystep) + (iz  *zstep);

  // now add or set the image values
  double new_val = add_data ? data[index]+val : val;
  data[index] = T(new_val);
#if 0 // doesn't seem to help ...
  // avoid rounding errors with integer type T:
  if (T(new_val + 1e-2) == T(new_val + 1e-1))
    data[index] = T(new_val+1e-2);
#endif

  return;
}


//: Reconstruct a smoothed image grid p+i.u+j.v+k.w from vector (in world coordinates)
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Reconstruction occurs along direction w first
// \param add_data adds data to existing values
template <class imType, class vecType>
void vimt3d_reconstruct_from_grid(vimt3d_image_3d_of<imType>& image,
                                  const vnl_vector<vecType>& vec,
                                  const vgl_point_3d<double>& p,
                                  const vgl_vector_3d<double>& u,
                                  const vgl_vector_3d<double>& v,
                                  const vgl_vector_3d<double>& w,
                                  int nu, int nv, int nw, bool add_data);


//: Reconstruct a smoothed image grid p+i.u+j.v+k.w from vector (in image coordinates)
//  Profile points are im_p+i.im_u+j.im_v+k.im_w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Reconstruction occurs along direction w first
// \param add_data adds data to existing values
template <class imType, class vecType>
void vimt3d_reconstruct_from_grid_ic(vil3d_image_view<imType>& image,
                                     const vnl_vector<vecType>& vec,
                                     const vgl_point_3d<double>& im_p,
                                     const vgl_vector_3d<double>& im_u,
                                     const vgl_vector_3d<double>& im_v,
                                     const vgl_vector_3d<double>& im_w,
                                     int nu, int nv, int nw, bool add_data);

#endif // vimt3d_reconstruct_from_grid_h_
