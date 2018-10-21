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

class vifa_gaussian
{
 protected:
  float  mu_;
  float  sigma_;

 public:
  vifa_gaussian(float  mu, float  sigma) : mu_(mu), sigma_(sigma) {}

  virtual ~vifa_gaussian(void) = default;

  float  pdf(float  x);

 protected:
  float  norm_dens(float  x);
};


#endif  // VIFA_GAUSSIAN_H
