// This is brl/bbas/brad/brad_multispectral_functions.h
#ifndef brad_multispectral_functions_h
#define brad_multispectral_functions_h

#include <vector>
#include <vil/vil_image_view.h>
#include <vgl_polygon.h>

//:
// \file
// \brief Various utililites for multispectral classification
// \author Tom Pollard and Selene Chew
// \date March 24, 2017


//: Normalize a vector of spectra to a unit vector.
void brad_normalize_spectra(
  float* spectra,
  int num_channels );

//: Convert so vgl_polygon to mask
void brad_polygon2mask(const vil_image_view<float>& image,
  const vgl_polygon<double>& poly,
  vil_image_view<bool>& mask);

//: Compute a normalized index image, used in remote sensing literature
// to classify materials, given two bands.  Examples include:
// Normalized Difference Vegitation Index, b1=red, b2=NIR2
// WorldView Water Index, b1=coastal, b2=NIR2
// Normalized Difference Soil Index, b1=green, b2=yellow
// WorldView Non-Homogeneous Feature Difference, b1=red-edge, b2=coastal
bool brad_compute_normalized_index_image(
  const vil_image_view<float>& mul_img,
  unsigned b1,
  unsigned b2,
  vil_image_view<float>& idx);
#endif // brad_multispectral_functions_h
