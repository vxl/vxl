#ifndef vil2_resample_bilin_h_
#define vil2_resample_bilin_h_

//: \file
//  \brief Sample grid of points in one image and place in another
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>

//: Sample grid of points in one image and place in another, using bilinear interpolation
//  dest_image(i,j,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2,y0+i.dy1+j.dy2), where i=[0..n1-1], j=[0..n2-1]
//  dest_image resized to (n1,n2,src_image.nplanes())
//  Points outside image return zero.
// \relates vil2_image_view
template <class sType, class dType>
void vil2_resample_bilin(const vil2_image_view<sType>& src_image,
                         vil2_image_view<dType>& dest_image,
                         double x0, double y0, double dx1, double dy1,
                         double dx2, double dy2, int n1, int n2);

#endif // vil2_resample_bilin_h_
