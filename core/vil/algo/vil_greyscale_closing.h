#ifndef vil_greyscale_closing_h_
#define vil_greyscale_closing_h_
//:
//  \file
//  \brief Perform greyscale closing on images
//  \author Tim Cootes

#include <vil/algo/vil_greyscale_dilate.h>
#include <vil/algo/vil_greyscale_erode.h>

//: Applies morphological closing operation with given structuring element
//  Applies dilation followed by erosion
// \relates vil_image_view
// \relates vil_structuring_element
template <class T>
inline void vil_greyscale_closing(const vil_image_view<T>& src_image,
                                   vil_image_view<T>& dest_image,
                                   const vil_structuring_element& element)
{
  vil_image_view<T> dilated_im;
  vil_greyscale_dilate(src_image,dilated_im,element);
  vil_greyscale_erode(dilated_im,dest_image,element);
}

#endif // vil_greyscale_closing_h_
