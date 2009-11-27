#ifndef vil3d_binary_opening_h_
#define vil3d_binary_opening_h_
//:
//  \file
//  \brief Perform binary opening on 3D images
//  \author Tim Cootes

#include <vil3d/algo/vil3d_binary_dilate.h>
#include <vil3d/algo/vil3d_binary_erode.h>

//: Applies morphological opening operation with given structuring element
//  Applies erosion followed by dilation
// \relatesalso vil3d_image_view
// \relatesalso vil3d_structuring_element
inline void vil3d_binary_opening(const vil3d_image_view<bool>& src_image,
                                vil3d_image_view<bool>& dest_image,
                                const vil3d_structuring_element& element)
{
  vil3d_image_view<bool> eroded_im;
  vil3d_binary_erode(src_image,eroded_im,element);
  vil3d_binary_dilate(eroded_im,dest_image,element);
}

#endif // vil3d_binary_opening_h_
