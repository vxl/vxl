#ifndef ipts_local_entropy_h_
#define ipts_local_entropy_h_

//:
//  \file
//  \brief Compute entropy in region around each image pixel
//  \author Tim Cootes

#include <vil/vil_image_view.h>

//: Compute local entropy in square region around each pixel in image
//  For each pixel in image, compute entropy in region (2h+1)x(2h+1)
//  centred on the pixel.  Result put in entropy image, which is of
//  size (image.ni()-2h) x (image.nj()-2h). Thus entropy(i,j)
//  corresponds to the value in the box around image point (i+h,j+h).
//
//  Values in image are assumed to lie in the range [min_v,max_v].
//  Any values outside that range will be ignored in the entropy calculation.
//
//  Local peaks are interesting points (see work of S.Gilles and that of T.Kadir).
void ipts_local_entropy(const vil_image_view<vxl_byte>& image,
                        vil_image_view<float>& entropy,
                        unsigned h, unsigned min_v = 0, unsigned max_v = 255);

#endif // ipts_local_entropy_h_
