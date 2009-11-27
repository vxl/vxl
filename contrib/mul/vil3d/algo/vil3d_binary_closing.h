#ifndef vil3d_binary_closing_h_
#define vil3d_binary_closing_h_
//:
//  \file
//  \brief Perform binary closing on 3D images
//  \author Tim Cootes

#include <vil3d/algo/vil3d_binary_dilate.h>
#include <vil3d/algo/vil3d_binary_erode.h>

//: Applies morphological closing operation with given structuring element
//  Applies dilation followed by erosion
// \relatesalso vil3d_image_view
// \relatesalso vil3d_structuring_element
inline void vil3d_binary_closing(const vil3d_image_view<bool>& src_image,
                                vil3d_image_view<bool>& dest_image,
                                const vil3d_structuring_element& element)
{
  vil3d_image_view<bool> dilated_im;
  vil3d_binary_dilate(src_image,dilated_im,element);
  vil3d_binary_erode(dilated_im,dest_image,element);
}

#endif // vil3d_binary_closing_h_
