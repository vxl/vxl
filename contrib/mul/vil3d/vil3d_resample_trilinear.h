// This is mul/vil3d/vil3d_resample_trilinear.h
#ifndef vil3d_resample_trilinear_h_
#define vil3d_resample_trilinear_h_

//:
// \file
// \brief Resample a 3D image by a different factor in each dimension
// \author Kevin de Souza, Ian Scott

#include <vil3d/vil3d_image_view.h>

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
