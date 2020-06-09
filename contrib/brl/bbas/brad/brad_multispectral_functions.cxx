#include <vector>
#include <vil/vil_image_view.h>
#include <vgl/vgl_polygon.h>

#include "brad_multispectral_functions.h"


//----------------------------------------------------
void brad_normalize_spectra(
  float* spectra,
  int num_channels)
{
  float tol = 0.000000001f;

  float sumsq = 0.0f;
  for (int b = 0; b < num_channels; b++) {
    if (spectra[b] == -1.0) continue;
    sumsq += spectra[b] * spectra[b];
  }

  float norm_factor = 0.0f;
  if (sumsq > tol) norm_factor = 1.0f / sqrt(sumsq);

  for (int b = 0; b < num_channels; b++) {
    if (spectra[b] == -1.0) continue;
    spectra[b] *= norm_factor;
  }
}


//---------------------------------------------------------------------------
//: Convert so vgl_polygon to mask
//---------------------------------------------------------------------------
void brad_polygon2mask(const vil_image_view<float>& image,
  const vgl_polygon<double>& poly,
  vil_image_view<bool>& mask) {
  mask.set_size(image.ni(), image.nj());
  mask.fill(false);
  for (size_t y = 0; y < image.nj(); y++) {
    for (size_t x = 0; x < image.ni(); x++) {
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

  if (mul_img.nplanes() != 8 && mul_img.nplanes() != 16) return false;
  idx.set_size(mul_img.ni(), mul_img.nj());

  for (size_t y = 0; y < mul_img.nj(); y++) {
    for (size_t x = 0; x < mul_img.ni(); x++) {
      float denom = mul_img(x, y, b1) + mul_img(x, y, b2);
      if (fabs(denom) > tol)
        idx(x, y) = (mul_img(x, y, b1) - mul_img(x, y, b2)) / denom;
    }
  }
  return true;
};
