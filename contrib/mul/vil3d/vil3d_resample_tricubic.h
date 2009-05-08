#ifndef vil3d_resample_tricubic_h_
#define vil3d_resample_tricubic_h_
//:
// \file
// \brief Resample a 3D image by a tricubic method
// \author Gwenael Guillard

#include <vil3d/vil3d_image_view.h>


//: Sample grid of points in one image and place in another, using tricubic interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1].
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside interpolatable region return zero or \a outval
template <class S, class T>
void vil3d_resample_tricubic(const vil3d_image_view<S>& src_image,
                             vil3d_image_view<T>& dest_image,
                             double x0, double y0, double z0,
                             double dx1, double dy1, double dz1,
                             double dx2, double dy2, double dz2,
                             double dx3, double dy3, double dz3,
                             int n1, int n2, int n3,
                             T outval=0);


//: Sample grid of points in one image and place in another, using tricubic interpolation and edge extension.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1].
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside interpolatable return the value of the nearest valid pixel.
template <class S, class T>
void vil3d_resample_tricubic_edge_extend(const vil3d_image_view<S>& src_image,
                                         vil3d_image_view<T>& dest_image,
                                         double x0, double y0, double z0,
                                         double dx1, double dy1, double dz1,
                                         double dx2, double dy2, double dz2,
                                         double dx3, double dy3, double dz3,
                                         int n1, int n2, int n3);

//: Sample grid of points in one image and place in another, using tricubic interpolation and edge extension.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1].
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside interpolatable return the value of the nearest valid pixel.
template <class S, class T>
void vil3d_resample_tricubic_edge_extend(const vil3d_image_view<S>& src_image,
                                         vil3d_image_view<T>& dest_image,
                                         int n1, int n2, int n3);

//: Sample grid of points in one image and place in another, using tricubic interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1]
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside interpolatable return the trilinear interpolated value of the nearest valid pixels.
template <class S, class T>
void vil3d_resample_tricubic_edge_trilin_extend(const vil3d_image_view<S>& src_image,
                                                vil3d_image_view<T>& dest_image,
                                                double x0, double y0, double z0,
                                                double dx1, double dy1, double dz1,
                                                double dx2, double dy2, double dz2,
                                                double dx3, double dy3, double dz3,
                                                int n1, int n2, int n3);

//: Sample grid of points in one image and place in another, using tricubic interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1]
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside interpolatable return the trilinear interpolated value of the nearest valid pixels.
template <class S, class T>
void vil3d_resample_tricubic_edge_trilin_extend(const vil3d_image_view<S>& src_image,
                                                vil3d_image_view<T>& dest_image,
                                                int n1, int n2, int n3);

//: Resample image to a specified dimensions (n1 * n2 * n3)
template <class S, class T>
void vil3d_resample_tricubic(const vil3d_image_view<S>& src_image,
                             vil3d_image_view<T>& dest_image,
                             int n1, int n2, int n3);


#endif // vil3d_resample_tricubic_h_
