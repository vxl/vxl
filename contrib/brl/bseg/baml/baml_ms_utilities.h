// This is brl/bseg/baml/baml_ms_utilities.h
#ifndef baml_ms_utilities_h
#define baml_ms_utilities_h

#include <vector>
#include <vil/vil_image_view.h>

//:
// \file
// \brief Various utililites for multispectral classification
// \author Tom Pollard and Selene Chew
// \date March 20, 2017

//: The spectral bands present in a World View 3 multi-spectral image.
void baml_wv3_bands(
  std::vector<float>& bands_min,
  std::vector<float>& bands_max);

//: Normalize a vector of spectra to a unit vector.
void baml_normalize_spectra(
  std::vector<float>& spectra);

#endif // baml_ms_utilities_h