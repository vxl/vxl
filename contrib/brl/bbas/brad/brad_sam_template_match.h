// This is brl/bbas/brad/brad_sam_template_match.h
#ifndef brad_sam_template_match_h
#define brad_sam_template_match_h

#include <vector>
#include <vil/vil_image_view.h>
#include <brad_spectral_angle_mapper.h>

//:
// \file
// \brief 
// \author Tom Pollard and Selene Chew
// \date May 10, 2017

bool brad_sam_template_match(
  const vil_image_view<float>& img,
  const vil_image_view<float>& chip,
  const vil_image_view<vxl_byte>& chip_mask,
  vil_image_view<float>& sam,
  float angle_step = 5.0f);

#endif // brad_sam_template_match_h