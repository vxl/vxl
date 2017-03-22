// This is brl/bseg/baml/baml_ms_band_diff.h
#ifndef baml_ms_band_diff_h
#define baml_ms_band_diff_h

#include <vector>
#include <vil/vil_image_view.h>

//:
// \file
// \brief Computes NDWI, NDVI, NDSI, NHFD
// \author Tom Pollard and Selene Chew
// \date March 20, 2017

//: Compute a normalized index image, used in remote sensing literature 
// to classify materials, given two bands.  Examples include:
// Normalized Difference Vegitation Index, b1=red, b2=NIR2
// WorldView Water Index, b1=coastal, b2=NIR2
// Normalized Difference Soil Index, b1=green, b2=yellow
// WorldView Non-Homogeneous Feature Difference, b1=red-edge, b2=coastal
bool baml_compute_normalized_index_image(
  const vil_image_view<float>& mul_img,
  unsigned b1,
  unsigned b2,
  vil_image_view<float>& idx);


#endif // baml_ms_band_diff_h