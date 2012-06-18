#include "boxm2_gauss_rgb_processor.h"

vnl_vector_fixed<float,3>  boxm2_gauss_rgb_processor::expected_color( vnl_vector_fixed<unsigned char, 8> mog)
{
  float y = (float)mog[0] / 255.0f;
  float u = (float)mog[1] / 255.0f * 0.872f - 0.436f;
  float v = (float)mog[2] / 255.0f * 1.23f - 0.615f;

  vnl_vector_fixed<float,3> exp_intensity;
  exp_intensity[0] = y                         + 1.1402508551881f * v;
  exp_intensity[1] = y - 0.39473137491174f * u - 0.5808092090311f * v;
  exp_intensity[2] = y + 2.0325203252033f  * u;

  return exp_intensity;
}
