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

  //: The robust loss function for the M-estimator. Overriding the
  //  overloaded version rho(u) hides the superclass' implementation of
  //  this version of rho(). This implementation simply calls the
  //  superclass' version of the same routine.
  //  \a r is the residual and
  //  \a s is the scale for that residual.
  inline  double rho( double r, double s ) const
    { return rrel_m_est_obj::rho(r, s); }

  //: The weight of the residual.
  virtual double wgt( double u ) const;

  //: Evaluate the objective function on heteroscedastic
  //  residuals.  Overriding the overloaded version wgt(u) hides the
  //  superclass' implementation of this version of wgt(). This
  //  implementation simply calls the superclass' version of the same
  //  routine. 
  //  \sa rrel_wls_obj::wgt()
  virtual void wgt( vect_const_iter res_begin, vect_const_iter res_end,
                    vect_const_iter scale_begin,
                    vect_iter wgt_begin ) const
    { rrel_m_est_obj::wgt(res_begin, res_end, scale_begin, wgt_begin); }

  //: Computes the weights for homoscedastic residuals.  Overriding
  //  the overloaded version wgt(u) hides the superclass' implementation
  //  of this version of wgt(). This implementation simply calls the
  //  superclass' version of the same routine.
  //  \sa rrel_wls_obj::wgt()
  virtual void wgt( vect_const_iter begin, vect_const_iter end,
                    double scale,
                    vect_iter wgt_begin ) const
    { rrel_m_est_obj::wgt(begin, end, scale, wgt_begin); }

  //: The weight of the residual. Overriding the overloaded version
  //  wgt(u) hides the superclass' implementation of this version of
  //  wgt(). This implementation simply calls the superclass' version of
  //  the same routine.
  //  \a r is the residual and
  //  \a s is the scale for that residual.
  inline  double wgt( double r, double s ) const
    { return rrel_m_est_obj::wgt(r, s); }

private:
  double C_;
};

#endif
