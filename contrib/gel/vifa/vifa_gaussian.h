// This is gel/vifa/vifa_gaussian.h
#ifndef VIFA_GAUSSIAN_H
#define VIFA_GAUSSIAN_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Compute the Gaussian probability density function at a point.
//
// \author Jim Farley, 11/6/1991
//
// \verbatim
//  Modifications:
//   MPP Jun 2003, Ported to VXL from TargetJr
// \endverbatim
//-----------------------------------------------------------------------------

// From GeneralUtility/Stat/stat_constants.h in TargetJr
#define  StatEPSILON 1e-8
#define  EXPLIMIT    37.0
#define  I_SQRT_2PI  0.39894228040143267794


class vifa_gaussian
{
 protected:
  float  _mu;
  float  _sigma;

 public:
  vifa_gaussian(float  mu,
                float  sigma
               );
  virtual ~vifa_gaussian(void) {}

  float  pdf(float  x);

 protected:
  float  norm_dens(float  x);
};


#endif  // VIFA_GAUSSIAN_H
