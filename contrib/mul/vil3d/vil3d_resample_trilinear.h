// This is mul/vil3d/vil3d_resample_trilinear.h
#ifndef vil3d_resample_trilinear_h_
#define vil3d_resample_trilinear_h_
//:
// \file
// \brief Resample a 3D image by an integer factor in each dimension
// \author Kevin de Souza

#include <vil3d/vil3d_image_view.h>

//: Resample a 3D image by a different factor in each dimension.
//  \p dst_image resized by factors \p dx, \p dy, \p dz.
//  dst_image(i, j, k, p) is sampled from src_image(i/dx, j/dy, k/dz, p).
//  Trilinear interpolation is performed on all voxels except ones near
//  the upper boundaries.
template <class T>
void vil3d_resample_trilinear(const vil3d_image_view<T>& src_image,
                              vil3d_image_view<T>& dst_image,
                              const double dx,
                              const double dy,
                              const double dz);

#endif // vil3d_resample_trilinear_h_
