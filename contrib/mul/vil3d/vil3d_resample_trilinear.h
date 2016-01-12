// This is mul/vil3d/vil3d_resample_trilinear.h
#ifndef vil3d_resample_trilinear_h_
#define vil3d_resample_trilinear_h_

//:
// \file
// \brief Resample a 3D image by a different factor in each dimension
// \author Kevin de Souza, Ian Scott

#include <vil3d/vil3d_image_view.h>


//: Sample grid of points in one image and place in another, using trilinear interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1].
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside image return zero or \a outval
template <class S, class T>
void vil3d_resample_trilinear(const vil3d_image_view<S>& src_image,
                              vil3d_image_view<T>& dest_image,
                              double x0, double y0, double z0,
                              double dx1, double dy1, double dz1,
                              double dx2, double dy2, double dz2,
                              double dx3, double dy3, double dz3,
                              int n1, int n2, int n3,
                              T outval=0, double edge_tol=0);


//: Sample grid of points in one image and place in another, using trilinear interpolation and edge extension.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1].
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside src_image return the value of the nearest valid pixel.
template <class S, class T>
void vil3d_resample_trilinear_edge_extend(const vil3d_image_view<S>& src_image,
                                          vil3d_image_view<T>& dest_image,
                                          double x0, double y0, double z0,
                                          double dx1, double dy1, double dz1,
                                          double dx2, double dy2, double dz2,
                                          double dx3, double dy3, double dz3,
                                          int n1, int n2, int n3);


//: Resample image to a specified dimensions (n1 * n2 * n3)
template <class S, class T>
void vil3d_resample_trilinear(const vil3d_image_view<S>& src_image,
                              vil3d_image_view<T>& dest_image,
                              int n1, int n2, int n3);


//: Resample a 3D image by a different factor in each dimension.
//  \p dst_image resized by factors \p dx, \p dy, \p dz.
// \note The upper image boundaries are extended.
// \param dx Scaling factor >1
// \param dy Scaling factor >1
// \param dz Scaling factor >1
//  dst_image(i, j, k, p) is sampled from src_image(i/dx, j/dy, k/dz, p).
// Interpolated values are rounded when the type T is smaller than double.
template <class T>
void vil3d_resample_trilinear(const vil3d_image_view<T>& src_image,
                              vil3d_image_view<T>& dst_image,
                              const double dx,
                              const double dy,
                              const double dz);

//: Resample a 3D image by a factor of 2 in each dimension.
// \p dst_image is resized to 2*src_image.n?()-1 in each direction.
// Interpolated values are truncated when the type T is smaller than double.
template <class T>
void vil3d_resample_trilinear_scale_2(
  const vil3d_image_view<T>& src_image,
  vil3d_image_view<T>& dst_image);

#endif // vil3d_resample_trilinear_h_
