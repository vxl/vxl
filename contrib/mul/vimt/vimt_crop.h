// This is mul/vimt/vimt_crop.h
#ifndef vimt_crop_h_
#define vimt_crop_h_

//:
//  \file
//  \brief Create windows into vimt_images.
//  \author Tim Cootes, Ian Scott

#include <vimt/vimt_image_2d_of.h>
#include <vil2/vil2_crop.h>

//: Create windowed view of given image
//  The parameters should be in image co-ords.
//  The world2im transform is set to match
//  so this appears identical to im when addressed
//  in world co-ords. O(1).
template<class T>
vimt_image_2d_of<T> vimt_crop(const vimt_image_2d_of<T>& im,
                              unsigned x0, unsigned nx,
                              unsigned y0, unsigned ny)
{
  vimt_transform_2d trans;
  trans.set_translation(-double(x0),-double(y0));
  return vimt_image_2d_of<T>(vil2_crop(im.image(),x0,ny,y0,ny),trans*im.world2im());
}



#endif // vimt_crop_h_
