#ifndef rrel_cauchy_obj_h_
#define rrel_cauchy_obj_h_

//:
// \file
// \author Amitha Perera (perera@cs.rpi.edu)
// Cauchy loss function.

#include <rrel/rrel_m_est_obj.h>
#include <vnl/vnl_math.h>

//: Cauchy robust loss function.
//  The objective function for the Cauchy M-estimator is
//  \f[
//      \rho(u) = 0.5 \log\bigl( 1 + \frac{u^2}{c^2} \bigr),
//  \f]
//  The associated weight function is
//  \f[
//       w(u) = \frac{1}{1 + \frac{u^2}{c^2} },
//  \f]
//  where u is a scale-normalized residual ($r/\sigma$) and $c$ is a
//  tuning constant.  This should be used when a more gradual
//  rejection of outliers is desired than something like the
//  Beaton-Tukey biweight.

class rrel_cauchy_obj : public rrel_m_est_obj
{
 public:
  //: Constructor.
  rrel_cauchy_obj( double C );

  //: Destructor.
  ~rrel_cauchy_obj() override;

  //: The robust loss function for the M-estimator.
  double rho( double u ) const override;

  //: The robust loss function for the M-estimator.
  //  Overriding the overloaded version rho(u) hides the superclass'
  //  implementation of this version of rho(). This implementation simply
  //  calls the superclass' version of the same routine.
  //  \a r is the residual and
  //  \a s is the scale for that residual.
  virtual double rho( double r, double s ) const
    { return rrel_m_est_obj::rho(r, s); }

  //: The weight of the residual.
  double wgt( double u ) const override;

  //: Evaluate the objective function on heteroscedastic residuals.
  //  Overriding the overloaded version wgt(u) hides the superclass'
  //  implementation of this version of wgt(). This implementation simply
  //  calls the superclass' version of the same routine.
  //  \sa rrel_wls_obj::wgt()
  void wgt( vect_const_iter res_begin, vect_const_iter res_end,
                    vect_const_iter scale_begin,
                    vect_iter wgt_begin ) const override
    { rrel_m_est_obj::wgt(res_begin, res_end, scale_begin, wgt_begin); }

  //: Computes the weights for homoscedastic residuals.
  //  Overriding the overloaded version wgt(u) hides the superclass'
  //  implementation of this version of wgt(). This implementation simply
  //  calls the superclass' version of the same routine.
  //  \sa rrel_wls_obj::wgt()
  void wgt( vect_const_iter begin, vect_const_iter end,
                    double scale,
                    vect_iter wgt_begin ) const override
    { rrel_m_est_obj::wgt(begin, end, scale, wgt_begin); }

  //: The weight of the residual.
  //  Overriding the overloaded version wgt(u) hides the superclass'
  //  implementation of this version of wgt(). This implementation simply
  //  calls the superclass' version of the same routine.
  //  \a r is the residual and
  //  \a s is the scale for that residual.
  virtual double wgt( double r, double s ) const
    { return rrel_m_est_obj::wgt(r, s); }

  //: Fast version of the wgt(u) computation.
  inline double wgt_fast( double u ) const;

  //: Fast version of the rho(u) computation.
  inline double rho_fast( double u ) const;


 private:
  double C_;
};

inline double
rrel_cauchy_obj::rho_fast( double u ) const
{
  return 0.5 * std::log( 1 + vnl_math::sqr( u/C_ ) );
}

inline double
rrel_cauchy_obj::wgt_fast( double u ) const
{
  return 1.0 / ( 1 + vnl_math::sqr(u/C_) );
}

#endif // rrel_cauchy_obj_h_
