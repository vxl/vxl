// This is mul/vimt3d/vimt3d_crop.h
#ifndef vimt3d_crop_h_
#define vimt3d_crop_h_
//:
//  \file
//  \brief Create windows into vimt3d_images.
//  \author Kevin de Souza, Tim Cootes, Ian Scott

#include <cmath>
#include <iostream>
#include <algorithm>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vil3d/vil3d_crop.h>
#include <vgl/vgl_box_3d.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Create windowed view of given image by specifying a cropping region in image co-ords.
//  The world2im transform is set so that this appears identical to im when addressed in world co-ords.
//  \param im The input image.
//  \param x0 The origin (lower corner) of the cropping region in image coords.
//  \param y0 The origin (lower corner) of the cropping region in image coords.
//  \param z0 The origin (lower corner) of the cropping region in image coords.
//  \param nx The width of the cropping region in image coords.
//  \param ny The height of the cropping region in image coords.
//  \param nz The depth of the cropping region in image coords.
//  \return A cropped view of the original image.
template <class T>
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


//: Create windowed view of given image by specifying a bounding box in world co-ords.
//  The world2im transform is set so that this appears identical to im when addressed in world co-ords.
//  \param im The input image.
//  \param bbox Bounding box of desired crop region in world coords.
//  \return A cropped view of the original image.
//  \note The crop region may be expanded slightly as required to fit the voxel grid.
//  \note If the crop region extends outside the image, it is truncated to fit the image.
template <class T>
vimt3d_image_3d_of<T> vimt3d_crop(const vimt3d_image_3d_of<T>& im,
                                  const vgl_box_3d<double>& bbox)
{
  // Compute the bounding box in image coords.
  vgl_point_3d<double> pi = im.world2im()(bbox.min_point());
  vgl_point_3d<double> qi = im.world2im()(bbox.max_point());
  vgl_box_3d<double> bbox_img;
  bbox_img.add(pi);
  bbox_img.add(qi);

  // Get the lower and upper corner points, rounding down and up respectively.
  pi = bbox_img.min_point();
  qi = bbox_img.max_point();
  pi.set(std::floor(pi.x()), std::floor(pi.y()), std::floor(pi.z()));
  qi.set(std::ceil(qi.x()),  std::ceil(qi.y()),  std::ceil(qi.z()));

  // Restrict to image bounds - perhaps we could use vgl_box intersection instead?
  unsigned ni = im.image().ni();
  unsigned nj = im.image().nj();
  unsigned nk = im.image().nk();
  unsigned pix = std::min(std::max((int) pi.x(), 0), (int)ni-1);
  unsigned piy = std::min(std::max((int) pi.y(), 0), (int)nj-1);
  unsigned piz = std::min(std::max((int) pi.z(), 0), (int)nk-1);
  unsigned qix = std::min(std::max((int) qi.x(), 0), (int)ni-1);
  unsigned qiy = std::min(std::max((int) qi.y(), 0), (int)nj-1);
  unsigned qiz = std::min(std::max((int) qi.z(), 0), (int)nk-1);

  // Crop image
  assert (qix>=pix && qiy>=piy && qiz>=piz);
  unsigned nx = qix - pix + 1;
  unsigned ny = qiy - piy + 1;
  unsigned nz = qiz - piz + 1;
  return vimt3d_crop(im, pix, nx, piy, ny, piz, nz);
}


#endif // vimt3d_crop_h_
