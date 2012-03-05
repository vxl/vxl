#ifndef vimt_sample_profile_bilin_h_
#define vimt_sample_profile_bilin_h_
//:
//  \file
//  \brief Profile sampling functions for 2D images
//  \author Tim Cootes

#include <vimt/vimt_image_2d_of.h>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>

//: Sample along profile, using bilinear interpolation
//  Profile points are p+iu, where i=[0..n-1] (world co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
template <class imType, class vecType>
void vimt_sample_profile_bilin(vnl_vector<vecType>& v,
                               const vimt_image_2d_of<imType>& image,
                               const vgl_point_2d<double>& p,
                               const vgl_vector_2d<double>& u,
                               int n);

//: Sample along profile, using bilinear interpolation
//  Profile points are p+iu, where i=[0..n-1] (world co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p.
//  Values outside the images are set to NA
template <class imType, class vecType>
void vimt_sample_profile_bilin_edgena(vnl_vector<vecType>& v,
                               const vimt_image_2d_of<imType>& image,
                               const vgl_point_2d<double>& p,
                               const vgl_vector_2d<double>& u,
                               int n);


#endif // vimt_sample_profile_bilin_h_
