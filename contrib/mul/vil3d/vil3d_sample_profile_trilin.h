#ifndef vil3d_sample_profile_trilin_h_
#define vil3d_sample_profile_trilin_h_
//:
// \file
// \brief Profile sampling functions for 3D images
// \author Tim Cootes

#include <vil3d/vil3d_image_view.h>

//: Sample along profile, using trilinear interpolation
//  Profile points are (x0+i.dx,y0+i.dy,z0+i.dz), where i=[0..n-1].
//  Vector v is filled with n*np elements, where np=image.nplanes()*image.ncomponents()
//  v[0]..v[np-1] are the values from point (x0,y0,z0)
//  Points outside image return zero.
// \relatesalso vil3d_image_view
template <class imType, class vecType>
void vil3d_sample_profile_trilin(
  vecType* v,
  const vil3d_image_view<imType>& image,
  double x0, double y0, double z0,
  double dx, double dy, double dz,
  unsigned n);

//: Sample along profile, using safe-extend trilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return nearest valid voxel value.
// \relatesalso vil3d_image_view
template <class imType, class vecType>
void vil3d_sample_profile_trilin_extend(
  vecType* v,
  const vil3d_image_view<imType>& image,
  double x0, double y0, double z0,
  double dx, double dy, double dz,
  unsigned n);

//: Sample along profile, using safe-extend trilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return NA.
// \relatesalso vil3d_image_view
template <class imType, class vecType>
void vil3d_sample_profile_trilin_edgena(
  vecType* v,
  const vil3d_image_view<imType>& image,
  double x0, double y0, double z0,
  double dx, double dy, double dz,
  unsigned n);

#endif // vil3d_sample_profile_trilin_h_
