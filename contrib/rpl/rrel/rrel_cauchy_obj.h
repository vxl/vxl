#ifndef rrel_cauchy_obj_h_
#define rrel_cauchy_obj_h_

//:
// \file
// \author Amitha Perera (perera@cs.rpi.edu)
// Cauchy loss function.

#include <rrel/rrel_m_est_obj.h>

//: Cauchy robust loss function.
//  The objective function for the Cauchy M-estimator is
//  \f[
//      \rho(u) = 0.5 \log\bigl( 1 + \frac{u^2}{c^2} \bigr),
//  \f]
//  where u is a scale-normalized residual ($r/\sigma$) and $c$ is a
//  tuning constant.  This should be used when a more gradual
//  rejection of outliers is desired than something like the
//  Beaton-Tukey biweight.

class rrel_cauchy_obj : public rrel_m_est_obj {
public:
  //: Constructor.
  rrel_cauchy_obj( double C );

  //: Destructor.
  virtual ~rrel_cauchy_obj();

  //: The robust loss function for the M-estimator.
  virtual double rho( double u ) const;

  //: The weight of the residual.
  virtual double wgt( double u ) const;

private:
  double C_;
};

#endif
