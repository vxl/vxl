#ifndef vil2_sample_grid_bilin_h_
#define vil2_sample_grid_bilin_h_

//: \file
//  \brief Grid sampling function for 2D images
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>

//: Sample grid from image, using bilinear interpolation
//  Grid points are (x0+i.dx1+j.dx2,y0+i.dy1+j.dy2), where i=[0..n1-1], j=[0..n2-1]
//  Vector v is filled with n1*n2*np elements, where np=image.nplanes()*image.ncomponents()
//  v[0]..v[np-1] are the values from point (x0,y0)
//  Samples are taken along direction (dx2,dy2) first, then along (dx1,dy1).
//  Points outside image return zero.
template <class imType, class vecType>
void vil2_sample_grid_bilin(vecType* v,
                           const vil2_image_view<imType>& image,
                           double x0, double y0, double dx1, double dy1,
                           double dx2, double dy2, int n1, int n2);

#endif // vil2_sample_grid_bilin_h_
