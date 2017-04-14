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
#include <brad_multispectral_functions.h>
#include <brad_spectral_angle_mapper.h>

//---------------------------------------------------------------------------
//: The spectral bands present in a World View 3 multi-spectral image.
//---------------------------------------------------------------------------
void brad_wv3_bands(
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

//---------------------------------------------------------------------------
//: The spectral bands present in a World View 3 multi-spectral image with SWIR.
//---------------------------------------------------------------------------
void brad_wv3_bands_swir(
  std::vector<float>& bands_min,
  std::vector<float>& bands_max)
{
  bands_min.resize(16);
  bands_max.resize(16);
  bands_min[0] = 0.400f; bands_max[0] = 0.450f; // Coastal
  bands_min[1] = 0.450f; bands_max[1] = 0.510f; // Blue
  bands_min[2] = 0.510f; bands_max[2] = 0.580f; // Green
  bands_min[3] = 0.585f; bands_max[3] = 0.625f; // Yellow
  bands_min[4] = 0.630f; bands_max[4] = 0.690f; // Red
  bands_min[5] = 0.705f; bands_max[5] = 0.745f; // Red Edge
  bands_min[6] = 0.770f; bands_max[6] = 0.895f; // Near-IR1
  bands_min[7] = 0.860f; bands_max[7] = 1.040f; // Near-IR2
  bands_min[8] = 1.195f; bands_max[8] = 1.225f; // SWIR-1
  bands_min[9] = 1.550f; bands_max[9] = 1.590f; // SWIR-2
  bands_min[10] = 1.640f; bands_max[10] = 1.680f; // SWIR-3
  bands_min[11] = 1.710f; bands_max[11] = 1.750f; // SWIR-4
  bands_min[12] = 2.145f; bands_max[12] = 2.185f; // SWIR-5
  bands_min[13] = 2.185f; bands_max[13] = 2.225f; // SWIR-6
  bands_min[14] = 2.235f; bands_max[14] = 2.285f; // SWIR-7
  bands_min[15] = 2.295f; bands_max[15] = 2.365f; // SWIR-8
};

//---------------------------------------------------------------------------
//: Normalize a vector of spectra to a unit vector.
//---------------------------------------------------------------------------
void brad_normalize_spectra(
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
//---------------------------------------------------------------------------
//: Convert so vgl_polygon to mask
//---------------------------------------------------------------------------
void brad_polygon2mask(const vil_image_view<float>& image, 
  const vgl_polygon<double>& poly,
  vil_image_view<bool>& mask) {
  mask.set_size(image.ni(), image.nj());
  mask.fill(false);
  for (int y = 0; y < image.nj(); y++) {
    for (int x = 0; x < image.ni(); x++) {
      if (poly.contains(x, y)) mask(x, y) = true;
    }
  } 
}

//---------------------------------------------------------------------------
//: Compute a normalized index image, used in remote sensing literature 
// to classify materials, given two bands.  Examples include:
// Normalized Difference Vegitation Index, b1=red, b2=NIR2
// WorldView Water Index, b1=coastal, b2=NIR2
// Normalized Difference Soil Index, b1=green, b2=yellow
// WorldView Non-Homogeneous Feature Difference, b1=red-edge, b2=coastal
//---------------------------------------------------------------------------
bool brad_compute_normalized_index_image(
  const vil_image_view<float>& mul_img,
  unsigned b1,
  unsigned b2,
  vil_image_view<float>& idx)
{
  float tol = 0.000000001f;

  if (mul_img.nplanes() != 8) return false;
  idx.set_size(mul_img.ni(), mul_img.nj());

  for (int y = 0; y < mul_img.nj(); y++) {
    for (int x = 0; x < mul_img.ni(); x++) {
      float denom = mul_img(x, y, b1) + mul_img(x, y, b2);
      if (fabs(denom) > tol)
        idx(x, y) = (mul_img(x, y, b1) - mul_img(x, y, b2)) / denom;
    }
  }
  return true;
};