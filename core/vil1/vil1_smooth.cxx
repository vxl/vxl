// This is vxl/vil/vil_smooth.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil_smooth.h"

#include <vcl_cmath.h>
#include <vcl_vector.h>

#include <vil/vil_byte.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_convolve.h>
#include <vil/vil_convolve.txx>

#define inst(pixel_type, float_type) \
template \
void vil_convolve_separable(float const kernel[], unsigned N, \
                            vil_memory_image_of<pixel_type> & buf, \
                            vil_memory_image_of<float_type>& tmp, \
                            vil_memory_image_of<float_type>& out); \
template \
vil_image vil_convolve_separable(vil_image const &, float const*, int, pixel_type*, float_type* )

inst(vil_byte, float);
inst(int, float);


vil_image vil_smooth_gaussian(vil_image const & in, double sigma)
{
  // Create 1-D mask:
  double cutoff = 0.01;
  double lc = -2 * vcl_log(cutoff); // cutoff guaranteed > 0
  int radius = (lc<=0) ? 0 : 1 + int(vcl_sqrt(lc)*sigma); // sigma guaranteed >= 0
  int size = 2*radius + 1;
  vcl_vector<float> mask(size);
  double halfnorm = 0.0;
  mask[radius] = 1.0f;
  for (int x=1; x<=radius; ++x) {
    double v = vcl_exp(-0.5*x*x/(sigma*sigma));
    mask[radius - x] = mask[radius + x] = float(v);
    halfnorm += v;
  }

  // normalise mask
  double mass_scale = 1.0/(1 + 2*halfnorm);
  for (int x=0; x< size; ++x)
    mask[x] *= float(mass_scale);

  // Call convolver
  if (vil_pixel_format(in) == VIL_BYTE)
    return vil_convolve_separable(in, /* xxx */&mask[0], size-1, (vil_byte*)0, (float*)0);

  if (vil_pixel_format(in) == VIL_FLOAT)
    return vil_convolve_separable(in, /* xxx */&mask[0], size-1, (float*)0, (float*)0);

  return 0;
}

