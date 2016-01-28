// This is mul/vimt3d/vimt3d_sample_grid_trilin.h
#ifndef vimt3d_sample_grid_trilin_h_
#define vimt3d_sample_grid_trilin_h_
// :
// \file
// \brief Grid sampling functions for 3D images
// \author Graham Vincent, Tim Cootes

#include <vimt3d/vimt3d_image_3d_of.h>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>

// : Sample grid p+i.u+j.v+k.w using trilinear interpolation in world coordinates
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along each image plane first, then direction w, then v, and u.
template <class imType, class vecType>
void vimt3d_sample_grid_trilin(vnl_vector<vecType>& vec, const vimt3d_image_3d_of<imType>& image,
                               const vgl_point_3d<double>& p, const vgl_vector_3d<double>& u,
                               const vgl_vector_3d<double>& v, const vgl_vector_3d<double>& w, unsigned nu, unsigned nv,
                               unsigned nw);

// : Sample grid p+i.u+j.v+k.w using trilinear interpolation in world coordinates.
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p.
//  Samples are taken along each image plane first, then direction w, then v, and u.
//  Samples outside the image are set to the value of the nearest voxel's value.
template <class imType, class vecType>
void vimt3d_sample_grid_trilin_extend(vnl_vector<vecType>& vec, const vimt3d_image_3d_of<imType>& image,
                                      const vgl_point_3d<double>& p, const vgl_vector_3d<double>& u,
                                      const vgl_vector_3d<double>& v, const vgl_vector_3d<double>& w, unsigned nu,
                                      unsigned nv, unsigned nw);

// : Sample grid p+i.u+j.v+k.w using trilinear interpolation in world coordinates.
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p.
//  Samples are taken along each image plane first, then direction w, then v, and u.
//  Samples outside the image are set to NA.
template <class imType, class vecType>
void vimt3d_sample_grid_trilin_edgena(vnl_vector<vecType>& vec, const vimt3d_image_3d_of<imType>& image,
                                      const vgl_point_3d<double>& p, const vgl_vector_3d<double>& u,
                                      const vgl_vector_3d<double>& v, const vgl_vector_3d<double>& w, unsigned nu,
                                      unsigned nv, unsigned nw);

// : Sample grid p+i.u+j.v+k.w using trilinear interpolation in image coordinates
//  Profile points are im_p+i.im_u+j.im_v+k.im_w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along each image plane first, then direction w, then v, and u.
template <class imType, class vecType>
void vimt3d_sample_grid_trilin_ic(vnl_vector<vecType>& vec, const vil3d_image_view<imType>& image,
                                  const vgl_point_3d<double>& im_p, const vgl_vector_3d<double>& im_u,
                                  const vgl_vector_3d<double>& im_v, const vgl_vector_3d<double>& im_w, unsigned nu,
                                  unsigned nv, unsigned nw);

#endif // vimt3d_sample_grid_trilin_h_
