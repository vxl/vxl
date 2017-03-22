#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>
#include <vul/vul_awk.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_image_atmospherics_est.h>
#include <baml_ms_aster.h>
#include <baml_ms_band_diff.h>

//: The spectral bands present in a World View 3 multi-spectral image.
void baml_wv3_bands(
  std::vector<float>& bands_min,
  std::vector<float>& bands_max)
{
  bands_min.resize(8);
  bands_max.resize(8);
  bands_min[0] = 0.400f; bands_max[0] = 0.450f; // Coastal
  bands_min[1] = 0.450f; bands_max[1] = 0.510f; // Blue
  bands_min[2] = 0.510f; bands_max[2] = 0.580f; // Green
  bands_min[3] = 0.585f; bands_max[3] = 0.625f; // Yellow
  bands_min[4] = 0.630f; bands_max[4] = 0.690f; // Red
  bands_min[5] = 0.705f; bands_max[5] = 0.745f; // Red Edge
  bands_min[6] = 0.770f; bands_max[6] = 0.895f; // Near-IR1
  bands_min[7] = 0.860f; bands_max[7] = 1.040f; // Near-IR2
};


//: Normalize a vector of spectra to a unit vector.
void baml_normalize_spectra(
  std::vector<float>& spectra)
{
  float tol = 0.000000001f;

  float sumsq = 0.0f;
  for (int b = 0; b < spectra.size(); b++)
    sumsq += spectra[b] * spectra[b];

  float norm_factor = 0.0f;
  if (sumsq > tol) norm_factor = 1.0f / sqrt(sumsq);

  for (int b = 0; b < spectra.size(); b++)
    spectra[b] *= norm_factor;
}

