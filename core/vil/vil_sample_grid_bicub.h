// This is core/vil/vil_sample_grid_bicub.h
#ifndef vil_sample_grid_bicub_h_
#define vil_sample_grid_bicub_h_
//:
// \file
// \brief Bicubic grid sampling function for 2D images
//
// The vil bicub source files were derived from the corresponding
// vil bilin files, thus the vil bilin/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bilin file that would likely also benefit from
// the same change.

#include <vil/vil_image_view.h>

//: Sample grid from image, using bicubic interpolation
//  Grid points are (x0+i.dx1+j.dx2,y0+i.dy1+j.dy2), where i=[0..n1-1], j=[0..n2-1]
//  Vector v is filled with n1*n2*np elements, where np=image.nplanes()*image.ncomponents()
//  v[0]..v[np-1] are the values from point (x0,y0)
//  Samples are taken along direction (dx2,dy2) first, then along (dx1,dy1).
//  Points outside image return zero.
// \relatesalso vil_image_view
template <class imType, class vecType>
void vil_sample_grid_bicub(vecType* v,
                           const vil_image_view<imType>& image,
                           double x0, double y0, double dx1, double dy1,
                           double dx2, double dy2, int n1, int n2);

#endif // vil_sample_grid_bicub_h_
