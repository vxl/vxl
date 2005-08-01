// This is mul/vimt3d/vimt3d_resample_trilinear.h
#ifndef vimt3d_resample_trilinear_h_
#define vimt3d_resample_trilinear_h_

//:
// \file
// \brief Resample a 3D image by a different factor in each dimension
// \author Kevin de Souza

#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_resample_trilinear.h>
#include <vimt3d/vimt3d_image_3d_of.h>


//: Resample a 3D image by a factor of 2 in each dimension.
// \p dst_image has size 2*src.image().n?()-1 in each direction.
// Transform is modified by an appropriate scaling of 0.5
// Interpolated values are truncated when the type T is smaller than double.
// \sa vil3d_resample_trilinear_scale_2()
template <class T>
void vimt3d_resample_trilinear_scale_2(
  const vimt3d_image_3d_of<T>& src,
  vimt3d_image_3d_of<T>& dst)
{
  vil3d_resample_trilinear_scale_2(src.image(), dst.image());

  vimt3d_transform_3d scaling;
  scaling.set_zoom_only(2.0, 2.0, 2.0, 0.0, 0.0, 0.0);
  dst.set_world2im(scaling * src.world2im());
}

#endif // vimt3d_resample_trilinear_h_
