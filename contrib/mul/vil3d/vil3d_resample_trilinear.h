// This is mul/vil3d/vil3d_resample_trilinear.h
#ifndef vil3d_resample_trilinear_h_
#define vil3d_resample_trilinear_h_
//:
// \file
// \brief Resample a 3D image by an integer factor in each dimension
// \author Kevin de Souza

#include <vil3d/vil3d_image_view.h>


//: Resample a 3D image by an integer factor in each dimension.
//  dst_image resized by factors dx, dy, dz.
//  dst_image(i, j, k, p) is sampled from src_image(i/dx, j/dy, k/dz, p).
//  Trilinear interpolation is performed.
template <class T>
void vil3d_resample_trilinear(const vil3d_image_view<T>& src_image,
                              vil3d_image_view<T>& dst_image,
                              const unsigned dx,
                              const unsigned dy,
                              const unsigned dz);

#endif // vil3d_resample_trilinear_h_
