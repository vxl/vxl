#ifndef vil2_greyscale_closing_h_
#define vil2_greyscale_closing_h_
//:
//  \file
//  \brief Perform greyscale closing on images
//  \author Tim Cootes

#include <vil2/algo/vil2_greyscale_dilate.h>
#include <vil2/algo/vil2_greyscale_erode.h>

//: Applies morphological closing operation with given structuring element
//  Applies dilation followed by erosion
// \relates vil2_image_view
// \relates vil2_structuring_element
template <class T>
inline void vil2_greyscale_closing(const vil2_image_view<T>& src_image,
                                   vil2_image_view<T>& dest_image,
                                   const vil2_structuring_element& element)
{
  vil2_image_view<T> dilated_im;
  vil2_greyscale_dilate(src_image,dilated_im,element);
  vil2_greyscale_erode(dilated_im,dest_image,element);
}

#endif // vil2_greyscale_closing_h_
