// This is core/vil/vil_sample_profile_bilin.h
#ifndef vil_sample_profile_bilin_h_
#define vil_sample_profile_bilin_h_
//:
// \file
// \brief Bilinear profile sampling functions for 2D images
// \author Tim Cootes
//
// The vil bicub source files were derived from the corresponding
// vil bilin files, thus the vil bilin/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bicub file that would likely also benefit from
// the same change.

#include <vil/vil_image_view.h>

//: Sample along profile, using bilinear interpolation
//  Profile points are (x0+i.dx,y0+i.dy), where i=[0..n-1].
//  Vector v is filled with n*np elements, where np=image.nplanes()*image.ncomponents()
//  v[0]..v[np-1] are the values from point (x0,y0)
//  Points outside image return zero.
// \relatesalso vil_image_view
template <class imType, class vecType>
void vil_sample_profile_bilin(vecType* v,
                              const vil_image_view<imType>& image,
                              double x0, double y0, double dx, double dy,
                              int n);

//: Sample along profile, using bilinear interpolation
//  Profile points are (x0+i.dx,y0+i.dy), where i=[0..n-1].
//  Vector v is filled with n*np elements, where np=image.nplanes()*image.ncomponents()
//  v[0]..v[np-1] are the values from point (x0,y0)
//  Points outside image return NA.
// \relatesalso vil_image_view
template <class imType, class vecType>
void vil_sample_profile_bilin_edgena(vecType* v,
                                     const vil_image_view<imType>& image,
                                     double x0, double y0, double dx, double dy,
                                     int n);

#endif // vil_sample_profile_bilin_h_
