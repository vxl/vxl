// This is core/vil1/vil1_smooth.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil1_smooth.h"

#include <vcl_cmath.h>
#include <vcl_vector.h>

#include <vxl_config.h>
#include <vil1/vil1_convolve.h>
#include <vil1/vil1_convolve.txx>

vil1_image vil1_smooth_gaussian(vil1_image const & in, double sigma)
{
  // Create 1-D mask:
  double cutoff = 0.01;
  double lc = -2 * vcl_log(cutoff); // cutoff guaranteed > 0
  int radius = (lc<=0) ? 0 : 1 + int(vcl_sqrt(lc)*sigma); // sigma guaranteed >= 0
  int size = 2*radius + 1;
  vcl_vector<double> mask(size);
  double halfnorm = 0.0;
  mask[radius] = 1.0;
  for (int x=1; x<=radius; ++x) {
    double v = vcl_exp(-0.5*x*x/(sigma*sigma));
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
    case VIL1_BYTE:  return vil1_convolve_separable(in, &mask[0], size-1, (vxl_byte*)0, (double*)0, (float*)0);
    case VIL1_UINT16:return vil1_convolve_separable(in, &mask[0], size-1, (vxl_uint_16*)0, (double*)0, (float*)0);
    case VIL1_UINT32:return vil1_convolve_separable(in, &mask[0], size-1, (vxl_uint_32*)0, (double*)0, (float*)0);
    case VIL1_FLOAT: return vil1_convolve_separable(in, &mask[0], size-1, (float*)0, (double*)0, (float*)0);
    case VIL1_DOUBLE:return vil1_convolve_separable(in, &mask[0], size-1, (double*)0, (double*)0, (double*)0);
    default: return 0;
  }
}

