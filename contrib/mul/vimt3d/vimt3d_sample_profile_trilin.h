#ifndef vimt3d_sample_profile_trilin_h_
#define vimt3d_sample_profile_trilin_h_
//:
// \file
// \brief Profile sampling functions for 3D images
// \author Tim Cootes

#include <vimt3d/vimt3d_image_3d_of.h>
#include <vil3d/vil3d_sample_profile_trilin.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_vector_3d.h>

//: Sample along profile, using trilinear interpolation
//  Profile points are p+iu, where i=[0..n-1] (world co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples outside the image are set to zero.
template <class imType, class vecType>
void vimt3d_sample_profile_trilin(
  vnl_vector<vecType>& vec,
  const vimt3d_image_3d_of<imType>& image,
  const vgl_point_3d<double>& p0,
  const vgl_vector_3d<double>& u,
  int n)
{
  vgl_point_3d<double> im_p0 = image.world2im()(p0);
  vgl_point_3d<double> im_p1 = image.world2im()(p0+u);
  int np = image.image().nplanes();
  vec.set_size(n*np);
  vecType *v = vec.data_block();

  // Can do all work in image co-ordinates under an affine transformation
  double dx = (im_p1.x()-im_p0.x());
  double dy = (im_p1.y()-im_p0.y());
  double dz = (im_p1.z()-im_p0.z());

  // Sample along profile between im_p0 and im_p1
  vil3d_sample_profile_trilin(v,image.image(),
    im_p0.x(),im_p0.y(),im_p0.z(),dx,dy,dz,n);
}


//: Sample along profile, using trilinear interpolation
//  Profile points are p+iu, where i=[0..n-1] (world co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples outside the image are set to the nearest voxel.
template <class imType, class vecType>
void vimt3d_sample_profile_trilin_extend(
  vnl_vector<vecType>& vec,
  const vimt3d_image_3d_of<imType>& image,
  const vgl_point_3d<double>& p0,
  const vgl_vector_3d<double>& u,
  int n)
{
  vgl_point_3d<double> im_p0 = image.world2im()(p0);
  vgl_point_3d<double> im_p1 = image.world2im()(p0+u);
  int np = image.image().nplanes();
  vec.set_size(n*np);
  vecType *v = vec.data_block();

  // Can do all work in image co-ordinates under an affine transformation
  double dx = (im_p1.x()-im_p0.x());
  double dy = (im_p1.y()-im_p0.y());
  double dz = (im_p1.z()-im_p0.z());

  // Sample along profile between im_p0 and im_p1
  vil3d_sample_profile_trilin_extend(v, image.image(),
    im_p0.x(),im_p0.y(),im_p0.z(),dx,dy,dz,n);
}

#endif // vimt3d_sample_profile_trilin_h_
