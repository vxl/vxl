// This is mul/mil3d/mil3d_sample_grid_3d.h
#ifndef mil3d_sample_grid_3d_h_
#define mil3d_sample_grid_3d_h_
//: \file
//  \brief Profile sampling functions for 3D images
//  \author Graham Vincent (to fit in with mil 2d framework by Tim Cootes et al)

#include <mil3d/mil3d_image_3d_of.h>
#include <vnl/vnl_fwd.h>

//: Sample grid p+i.u+j.v+k.w using trilinear interpolation in world coordinates
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
template <class imType, class vecType>
void mil3d_sample_grid_3d(vnl_vector<vecType>& vec,
                          const mil3d_image_3d_of<imType>& image,
                          const vgl_point_3d<double>& p,
                          const vgl_vector_3d<double>& u,
                          const vgl_vector_3d<double>& v,
                          const vgl_vector_3d<double>& w,
                          int nu, int nv, int nw);


//: Sample grid p+i.u+j.v+k.w using trilinear interpolation in image coordinates
//  Profile points are im_p+i.im_u+j.im_v+k.im_w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
template <class imType, class vecType>
void mil3d_sample_grid_3d_ic(vnl_vector<vecType>& vec,
                             const mil3d_image_3d_of<imType>& image,
                             const vgl_point_3d<double>& im_p,
                             const vgl_vector_3d<double>& im_u,
                             const vgl_vector_3d<double>& im_v,
                             const vgl_vector_3d<double>& im_w,
                             int nu, int nv, int nw);


//: Sample grid p+i.u+j.v+k.w using safe trilinear interpolation.
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
//  Points outside image return zero.
template <class imType, class vecType>
void mil3d_sample_grid_3d_safe(vnl_vector<vecType>& vec,
                               const mil3d_image_3d_of<imType>& image,
                               const vgl_point_3d<double>& p0,
                               const vgl_vector_3d<double>& u,
                               const vgl_vector_3d<double>& v,
                               const vgl_vector_3d<double>& w,
                               int nu, int nv, int nw);

//: Sample grid p+i.u+j.v+k.w using trilinear interpolation (no checks)
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
//  Points outside image return zero.
template <class imType, class vecType>
void mil3d_sample_grid_3d_no_checks(vnl_vector<vecType>& vec,
                                    const mil3d_image_3d_of<imType>& image,
                                    const vgl_point_3d<double>& p0,
                                    const vgl_vector_3d<double>& u,
                                    const vgl_vector_3d<double>& v,
                                    const vgl_vector_3d<double>& w,
                                    int nu, int nv, int nw);

//: Sample grid p+i.u+j.v+k.w using trilinear interpolation (points in image co-ordinates)
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
//  Points outside image return zero.
template <class imType, class vecType>
void mil3d_sample_grid_3d_ic_safe(vnl_vector<vecType>& vec,
                                  const mil3d_image_3d_of<imType>& image,
                                  const vgl_point_3d<double>& p0,
                                  const vgl_vector_3d<double>& u,
                                  const vgl_vector_3d<double>& v,
                                  const vgl_vector_3d<double>& w,
                                  int nu, int nv, int nw);


//: Sample grid p+i.u+j.v+k.w using trilinear interpolation (points in image co-ordinates)
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
//  Points outside image return zero.
template <class imType, class vecType>
void mil3d_sample_grid_3d_ic_no_checks(vnl_vector<vecType>& vec,
                                       const mil3d_image_3d_of<imType>& image,
                                       const vgl_point_3d<double>& p0,
                                       const vgl_vector_3d<double>& u,
                                       const vgl_vector_3d<double>& v,
                                       const vgl_vector_3d<double>& w,
                                       int nu, int nv, int nw);

#endif // mil3d_sample_grid_3d_h_
