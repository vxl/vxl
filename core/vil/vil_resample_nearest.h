// This is core/vil/vil_resample_nearest.h
#ifndef vil_resample_nearest_h_
#define vil_resample_nearest_h_
//:
// \file
// \brief Sample image with nearest neighbour interpolation in one image and place in another
// \author dac
//
// The vil nearest neighbour source files were derived from the corresponding
// vil bilin files, thus the vil bilin/bicub/nearest source files are very
// similar.  If you modify something in this file, there is a
// corresponding bicub/bilin file that would likely also benefit from
// the same change.

#include "vil_image_view.h"

//: Sample grid of points in one image and place in another, using nearest neighbour interpolation.
//  dest_image(i,j,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2,y0+i.dy1+j.dy2), where i=[0..n1-1], j=[0..n2-1]
//  dest_image resized to (n1,n2,src_image.nplanes())
//  Points outside image return zero.
// \relatesalso vil_image_view
template <class sType, class dType>
void vil_resample_nearest(const vil_image_view<sType>& src_image,
                          vil_image_view<dType>& dest_image,
                          double x0, double y0, double dx1, double dy1,
                          double dx2, double dy2, int n1, int n2);

//: Resample image to a specified width (n1) and height (n2)
// \relatesalso vil_image_view
template <class sType, class dType>
void vil_resample_nearest(const vil_image_view<sType>& src_image,
                          vil_image_view<dType>& dest_image,
                          int n1, int n2);

//: Sample grid of points in one image and place in another, using nearest neighbour interpolation.
//  dest_image(i,j,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2,y0+i.dy1+j.dy2), where i=[0..n1-1], j=[0..n2-1]
//  dest_image resized to (n1,n2,src_image.nplanes())
//  Points outside image return the value of the nearest valid pixel.
// \relatesalso vil_image_view
template <class sType, class dType>
void vil_resample_nearest_edge_extend(const vil_image_view<sType>& src_image,
                                      vil_image_view<dType>& dest_image,
                                      double x0, double y0, double dx1, double dy1,
                                      double dx2, double dy2, int n1, int n2);

//: Resample image to a specified width (n1) and height (n2)
// \relatesalso vil_image_view
template <class sType, class dType>
void vil_resample_nearest_edge_extend(const vil_image_view<sType>& src_image,
                                      vil_image_view<dType>& dest_image,
                                      int n1, int n2);

#endif // vil_resample_nearest_h_
