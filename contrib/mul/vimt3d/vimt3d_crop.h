// This is mul/vimt3d/vimt3d_crop.h
#ifndef vimt3d_crop_h_
#define vimt3d_crop_h_

//:
//  \file
//  \brief Create windows into vimt3d_images.
//  \author Kevin de Souza, Tim Cootes, Ian Scott

#include <vimt3d/vimt3d_image_3d_of.h>
#include <vil3d/vil3d_crop.h>

//: Create windowed view of given image
//  The parameters should be in image co-ords.
//  The world2im transform is set to match
//  so this appears identical to im when addressed
//  in world co-ords. O(1).
// This 3d version is an adaptation of the 2d vimt_crop().
template<class T>
vimt3d_image_3d_of<T> vimt3d_crop(const vimt3d_image_3d_of<T>& im,
                                  unsigned x0, unsigned nx,
                                  unsigned y0, unsigned ny,
                                  unsigned z0, unsigned nz)
{
  vimt3d_transform_3d trans;
  trans.set_translation(-double(x0), -double(y0), -double(z0));
  return vimt3d_image_3d_of<T>(vil3d_crop(im.image(), x0, nx, y0, ny, z0, nz),
                               trans*im.world2im());
}

#endif // vimt3d_crop_h_
