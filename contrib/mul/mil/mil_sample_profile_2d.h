// This is mul/mil/mil_sample_profile_2d.h
#ifndef mil_sample_profile_2d_h_
#define mil_sample_profile_2d_h_
//:
// \file
// \brief Profile sampling functions for 2D images
// \author Tim Cootes

#include <mil/mil_image_2d_of.h>
#include <vnl/vnl_fwd.h>

//: Sample along profile, using bilinear interpolation
//  Profile points are p+iu, where i=[0..n-1] (world co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
template <class imType, class vecType>
void mil_sample_profile_2d(vnl_vector<vecType>& v,
                           const mil_image_2d_of<imType>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u,
                           int n);

//: Sample along profile, using safe bilinear interpolation
//  Profile points are p+iu, where i=[0..n-1].
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void mil_sample_profile_2d_safe(vnl_vector<vecType>& v,
                                const mil_image_2d_of<imType>& image,
                                const vgl_point_2d<double>& p,
                                const vgl_vector_2d<double>& u,
                                int n);

//: Sample along profile, using bilinear interpolation (no checks)
//  Profile points are p+iu, where i=[0..n-1], all of which are
//  assumed to be sufficiently inside the image that bilinear interpolation
//  will be safe.
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
template <class imType, class vecType>
void mil_sample_profile_2d_no_checks(vnl_vector<vecType>& v,
                                     const mil_image_2d_of<imType>& image,
                                     const vgl_point_2d<double>& p,
                                     const vgl_vector_2d<double>& u,
                                     int n);

//: Sample along profile, using safe bilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void mil_sample_profile_2d_ic(vnl_vector<vecType>& v,
                              const mil_image_2d_of<imType>& image,
                              const vgl_point_2d<double>& p0,
                              const vgl_point_2d<double>& p1,
                              int n);

#define MIL_SAMPLE_PROFILE_2D_INSTANTIATE(imType, vecType) \
extern "please #include mil/mil_sample_profile_2d.txx instead"

#endif // mil_sample_profile_2d_h_
