// This is mul/mil3d/mil3d_sample_profile_3d.h
#ifndef mil3d_sample_profile_3d_h_
#define mil3d_sample_profile_3d_h_
//: \file
//  \brief Profile sampling functions for 2D images
//  \author Tim Cootes

#include <mil3d/mil3d_image_3d_of.h>
#include <vnl/vnl_fwd.h>

//: Sample along profile, using trilinear interpolation
//  Profile points are p+iu, where i=[0..n-1] (world co-ordinates).
//
//  If the ends are both inside image, fast interpolation is used.
//  If either end is outside, each sample point is checked to ensure
//  that it is inside the image - those outside are considered as zeroes.
//
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
template <class imType, class vecType>
void mil3d_sample_profile_3d(vnl_vector<vecType>& v,
                             const mil3d_image_3d_of<imType>& image,
                             const vgl_point_3d<double>& p,
                             const vgl_vector_3d<double>& u,
                             int n);

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
                                     int n);

//: Sample along profile, using safe trilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//
//  If the ends are both inside image, fast interpolation is used.
//  If either end is outside, calls mil3d_sample_profile_3d_ic_safe
//
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void mil3d_sample_profile_3d_ic(vnl_vector<vecType>& v,
                                const mil3d_image_3d_of<imType>& image,
                                const vgl_point_3d<double>& p0,
                                const vgl_point_3d<double>& p1,
                                int n);

#endif // mil3d_sample_profile_3d_h_
