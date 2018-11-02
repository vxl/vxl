// This is mul/vimt/vimt_crop.h
#ifndef vimt_crop_h_
#define vimt_crop_h_
//:
//  \file
//  \brief Create windows into vimt_images.
//  \author Tim Cootes, Ian Scott

#include <iostream>
#include <cmath>
#include <vimt/vimt_image_2d_of.h>
#include <vil/vil_crop.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Create windowed view of given image
//  The parameters should be in image co-ords.
//  The world2im transform is set to match
//  so this appears identical to im when addressed
//  in world co-ords. O(1).
template <class T>
vimt_image_2d_of<T> vimt_crop(const vimt_image_2d_of<T>& im,
                              unsigned x0, unsigned nx,
                              unsigned y0, unsigned ny)
{
  vimt_transform_2d trans;
  trans.set_translation(-double(x0),-double(y0));
  return vimt_image_2d_of<T>(vil_crop(im.image(),x0,nx,y0,ny),trans*im.world2im());
}


//: Create windowed view of given image by specifying a bounding box in world co-ords.
//  The world2im transform is set so that this appears identical to im when addressed in world co-ords.
//  \param im The input image.
//  \param bbox Bounding box of desired crop region in world coords.
//  \return A cropped view of the original image.
//  \note The crop region may be expanded slightly as required to fit the voxel grid.
//  \note If the crop region extends outside the image, it is truncated to fit the image.
template <class T>
vimt_image_2d_of<T> vimt_crop(const vimt_image_2d_of<T>& im,
                              const vgl_box_2d<double>& bbox)
{
  // Compute the bounding box in image coords.
  vgl_point_2d<double> pi = im.world2im()(bbox.min_point());
  vgl_point_2d<double> qi = im.world2im()(bbox.max_point());
  vgl_box_2d<double> bbox_img;
  bbox_img.add(pi);
  bbox_img.add(qi);

  // Get the lower and upper corner points, rounding down and up respectively.
  pi = bbox_img.min_point();
  qi = bbox_img.max_point();
  pi.set(std::floor(pi.x()), std::floor(pi.y()));
  qi.set(std::ceil(qi.x()),  std::ceil(qi.y()));

  // Restrict to image bounds - perhaps we could use vgl_box intersection instead?
  unsigned ni = im.image().ni();
  unsigned nj = im.image().nj();
  unsigned pix = pi.x()<0 ? 0 : static_cast<unsigned>(pi.x());
  unsigned piy = pi.y()<0 ? 0 : static_cast<unsigned>(pi.y());
  unsigned qix = qi.x()+1>ni ? ni-1 : static_cast<unsigned>(qi.x());
  unsigned qiy = qi.y()+1>nj ? nj-1 : static_cast<unsigned>(qi.y());

  // Crop image
  assert (qix>=pix && qiy>=piy);
  unsigned nx = qix - pix + 1;
  unsigned ny = qiy - piy + 1;
  return vimt_crop(im, pix, nx, piy, ny);
}


#endif // vimt_crop_h_
