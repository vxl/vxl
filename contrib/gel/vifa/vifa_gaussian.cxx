#include <iostream>
#include <cmath>
#include "vifa_gaussian.h"
//:
// \file
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vnl/vnl_math.h"

// From GeneralUtility/Stat/stat_constants.h in TargetJr
#define  StatEPSILON 1e-8
#define  EXPLIMIT    37.0

float vifa_gaussian::pdf(float  x)
{
  if (sigma_ < StatEPSILON)
  {
    // Degenerate distribution:
    // The variance is zero, so the pdf is a unit impulse at the mean.
    // Return 1.0 if x == mean, and 0.0 otherwise.

    return std::fabs(x - mu_) < StatEPSILON ? 1.0f : 0.0f;
  }
  else
  {
    float  xp = (x - mu_) / sigma_;
    return norm_dens(xp) / sigma_;
  }
}


float vifa_gaussian::norm_dens(float  x)
{
  // Check to see if the magnitude of x is large enough to
  // warrant clipping the pdf to 0.0
  return x < -EXPLIMIT ||
         x > EXPLIMIT ? 0.0f : float(vnl_math::one_over_sqrt2pi * std::exp(-0.5*x*x));
}
