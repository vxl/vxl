#ifndef vil2_sample_profile_bilin_h_
#define vil2_sample_profile_bilin_h_

//: \file
//  \brief Profile sampling functions for 2D images
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>
#include <vnl/vnl_fwd.h>

//: Sample along profile, using bilinear interpolation
//  Profile points are (x0+i.dx,y0+i.dy), where i=[0..n-1].
//  Vector v is filled with n*np elements, where np=image.nplanes()*image.ncomponents()
//  v[0]..v[np-1] are the values from point (x0,y0)
//  Points outside image return zero.
template <class imType, class vecType>
void vil2_sample_profile_bilin(vecType* v,
                           const vil2_image_view<imType>& image,
                           double x0, double y0, double dx, double dy,
                           int n);

#endif // vil2_sample_profile_bilin_h_
