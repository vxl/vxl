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

#include <vil1/vil1_byte.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_convolve.h>
#include <vil1/vil1_convolve.txx>

#define inst(pixel_type, float_type) \
template \
void vil1_convolve_separable(float const kernel[], unsigned N, \
                             vil1_memory_image_of<pixel_type> & buf, \
                             vil1_memory_image_of<float_type>& tmp, \
                             vil1_memory_image_of<float_type>& out); \
template \
vil1_image vil1_convolve_separable(vil1_image const &, float const*, int, pixel_type*, float_type* )

inst(vil1_byte, float);
inst(int, float);


vil1_image vil1_smooth_gaussian(vil1_image const & in, double sigma)
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
  if (vil1_pixel_format(in) == VIL1_BYTE)
    return vil1_convolve_separable(in, /* xxx */&mask[0], size-1, (vil1_byte*)0, (float*)0);

  if (vil1_pixel_format(in) == VIL1_FLOAT)
    return vil1_convolve_separable(in, /* xxx */&mask[0], size-1, (float*)0, (float*)0);

  return 0;
}

