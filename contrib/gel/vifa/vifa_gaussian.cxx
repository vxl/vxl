#include <vcl_cmath.h>
#include <vifa/vifa_gaussian.h>

vifa_gaussian::vifa_gaussian(float  mu,
                             float  sigma)
{
  _mu = mu;
  _sigma = sigma;
}


float vifa_gaussian::pdf(float  x)
{
  if (_sigma < StatEPSILON)
  {
    // Degenerate distribution:
    // The variance is zero, so the pdf is a unit impulse at the mean.
    // Return 1.0 if x == mean, and 0.0 otherwise.

    return vcl_fabs(x - _mu) < StatEPSILON ? 1.0 : 0.0;
  }
  else
  {
    float  xp = (x - _mu) / _sigma;
    return norm_dens(xp) / _sigma;
  }
}


float vifa_gaussian::norm_dens(float  x)
{
  // Check to see if the magnitude of x is large enough to
  // warrant clipping the pdf to 0.0
  return x < -EXPLIMIT || x > EXPLIMIT ? 0.0 :
                                         I_SQRT_2PI * vcl_exp(-0.5 * x * x);
}
