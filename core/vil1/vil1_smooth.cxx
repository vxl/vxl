// This is core/vil1/vil1_smooth.cxx
//:
// \file
// \author fsm

#include <cmath>
#include <vector>
#include "vil1_smooth.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vxl_config.h>
#include <vil1/vil1_convolve.h>
#include <vil1/vil1_convolve.hxx>

vil1_image vil1_smooth_gaussian(vil1_image const & in, double sigma)
{
  // Create 1-D mask:
  double cutoff = 0.01;
  double lc = -2 * std::log(cutoff); // cutoff guaranteed > 0
  int radius = (lc<=0) ? 0 : 1 + int(std::sqrt(lc)*sigma); // sigma guaranteed >= 0
  int size = 2*radius + 1;
  std::vector<double> mask(size);
  double halfnorm = 0.0;
  mask[radius] = 1.0;
  for (int x=1; x<=radius; ++x) {
    double v = std::exp(-0.5*x*x/(sigma*sigma));
    mask[radius - x] = mask[radius + x] = v;
    halfnorm += v;
  }

  // normalise mask
  double mass_scale = 1.0/(1 + 2*halfnorm);
  for (int x=0; x< size; ++x)
    mask[x] *= mass_scale;

  // Call convolver
  switch (vil1_pixel_format(in))
  {
    case VIL1_BYTE:  return vil1_convolve_separable(in, &mask[0], size-1, (vxl_byte*)nullptr, (double*)nullptr, (float*)nullptr);
    case VIL1_UINT16:return vil1_convolve_separable(in, &mask[0], size-1, (vxl_uint_16*)nullptr, (double*)nullptr, (float*)nullptr);
    case VIL1_UINT32:return vil1_convolve_separable(in, &mask[0], size-1, (vxl_uint_32*)nullptr, (double*)nullptr, (float*)nullptr);
    case VIL1_FLOAT: return vil1_convolve_separable(in, &mask[0], size-1, (float*)nullptr, (double*)nullptr, (float*)nullptr);
    case VIL1_DOUBLE:return vil1_convolve_separable(in, &mask[0], size-1, (double*)nullptr, (double*)nullptr, (double*)nullptr);
    default: return nullptr;
  }
}
