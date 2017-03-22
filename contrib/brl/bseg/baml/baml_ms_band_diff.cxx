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
