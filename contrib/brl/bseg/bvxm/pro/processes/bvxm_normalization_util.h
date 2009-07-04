#ifndef bvxm_normalization_util_h_
#define bvxm_normalization_util_h_
//:
// \file
// \brief Utility functions for normalization processes.
// \author Isabel Restrepo (mir@lems.brown.edu)
// \date January 22, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vil/vil_image_view.h>

class bvxm_normalization_util
{
 public:

  //: Float specialized function to normalize and image given a,b where new_I = a*I +b;
  static bool normalize_image(const vil_image_view<float>& in_view,
                              vil_image_view<float>& out_img,
                              float a, float b, float max_value);

  //: Byte specialized function to normalize and image given a,b where new_I = a*I +b;
  static bool normalize_image(const vil_image_view<vxl_byte>& in_view,
                              vil_image_view<vxl_byte>& out_img,
                              float a, float b, vxl_byte max_value = 255);
};

#endif // bvxm_normalization_util_h_
