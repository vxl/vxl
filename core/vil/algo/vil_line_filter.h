// This is mul/vil2/algo/vil2_line_filter.h
#ifndef vil2_line_filter_h_
#define vil2_line_filter_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
//  \brief Find line-like structures in a 2D image
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>

//: Find line-like structures in a 2D image
//  Generates two images: line_str contains line strength at each pixel,
//  essentially the difference between the average of the pixels on
//  the line and the average of those nearby off the line.
//  and line_dir contains value indicating direction [0,4]
//  0 = Undefined, 1 = horizontal, 2 = 45 degrees etc

template<class Type>
class vil2_line_filter
{
 public:
  //: Find line like structures in image (light lines on dark backgrounds)
  //  On exit line_str contains line strength at each pixel,
  //  line_dir contains value indicating direction [0,4]
  //  0 = Undefined, 1 = horizontal, 2 = 45 degrees etc
  //  This version looks for light lines on a dark background only.
  void light_lines_3x3(vil2_image_view<vxl_byte>& line_dir,
                       vil2_image_view<float>& line_str,
                       const vil2_image_view<Type>& image,
                       float edge_thresh=0.1);

  //: Find line like structures in image (dark lines on light backgrounds)
  //  On exit line_str contains line strength at each pixel,
  //  line_dir contains value indicating direction [0,4]
  //  0 = Undefined, 1 = horizontal, 2 = 45 degrees etc
  //  This version looks for dark lines on a light background only.
  void dark_lines_3x3(vil2_image_view<vxl_byte>& line_dir,
                      vil2_image_view<float>& line_str,
                      const vil2_image_view<Type>& image,
                      float edge_thresh=0.1);

  //: Find line like structures in image (light lines on dark backgrounds)
  //  On exit line_str contains line strength at each pixel,
  //  line_dir contains value indicating direction [0,4]
  //  0 = Undefined, 1 = horizontal, 2 = 45 degrees etc
  //  This version looks for light lines on a dark background only
  //  using a 5x5 filter
  void light_lines_5x5(vil2_image_view<vxl_byte>& line_dir,
                       vil2_image_view<float>& line_str,
                       const vil2_image_view<Type>& image,
                       float edge_thresh=0.1);

  //: Find line like structures in image (dark lines on light backgrounds)
  //  On exit line_str contains line strength at each pixel,
  //  line_dir contains value indicating direction [0,4]
  //  0 = Undefined, 1 = horizontal, 2 = 45 degrees etc
  //  This version looks for dark lines on a light background only
  //  using a 5x5 filter
  void dark_lines_5x5(vil2_image_view<vxl_byte>& line_dir,
                      vil2_image_view<float>& line_str,
                      const vil2_image_view<Type>& image,
                      float edge_thresh=0.1);
};

#endif

