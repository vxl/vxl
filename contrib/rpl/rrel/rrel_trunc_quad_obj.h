#ifndef rrel_trunc_quad_obj_h_
#define rrel_trunc_quad_obj_h_

//:
// \file
// \author Chuck Stewaty (stewart@cs.rpi.edu)
// \author Amitha Perera (perera@cs.rpi.edu)
// \brief Truncated quadratic loss function

#include <rrel/rrel_m_est_obj.h>

//: Truncated quadratic robust loss function.
//  The objective function for the truncated quadratic M-estimator is
//  \f[
//    \rho(u) =
//    \left\{
//      \begin{array}{ll}
//        u^2, & |u| \le T \\ T^2, & |u| > T
//      \end{array}
//    \right.
//  \f]
//
//  where u is a scale-normalized residual (\f$ r/\sigma \f$) and \f$
//  T \f$ is a threshold. This has been used in a number of vision
//  applications despite the fact that the wgt function is
//  discontinuous. This is the same cost function used in the MSAC
//  generalisation of RANSAC.

class rrel_trunc_quad_obj : public rrel_m_est_obj
{
 public:
  //: Constructor.
  //  \a scale_mult * scale is the truncation threshold.
  rrel_trunc_quad_obj( double scale_mult = 2.0 );

  //: Destructor.
  virtual ~rrel_trunc_quad_obj();

  //: The robust loss function for the M-estimator.
  virtual double rho( double u ) const;

  //: The robust loss function for the M-estimator.
  //  Overriding the overloaded version rho(u) hides the superclass'
  //  implementation of this version of rho(). This implementation simply
  //  calls the superclass' version of the same routine.
  //  \a r is the residual and
  //  \a s is the scale for that residual.
  virtual double rho( double r, double s ) const
    { return rrel_m_est_obj::rho(r, s); }

  //: The weight of the residual.
  virtual double wgt( double u ) const;

  //: Evaluate the objective function on heteroscedastic residuals.
  //  Overriding the overloaded version wgt(u) hides the superclass'
  //  implementation of this version of wgt(). This implementation simply
  //  calls the superclass' version of the same routine.
  //  \sa rrel_wls_obj::wgt()
  virtual void wgt( vect_const_iter res_begin, vect_const_iter res_end,
                    vect_const_iter scale_begin,
                    vect_iter wgt_begin ) const
    { rrel_m_est_obj::wgt(res_begin, res_end, scale_begin, wgt_begin); }

  //: Computes the weights for homoscedastic residuals.
  //  Overriding the overloaded version wgt(u) hides the superclass'
  //  implementation of this version of wgt(). This implementation simply
  //  calls the superclass' version of the same routine.
  //  \sa rrel_wls_obj::wgt()
  virtual void wgt( vect_const_iter begin, vect_const_iter end,
                    double scale,
                    vect_iter wgt_begin ) const
    { rrel_m_est_obj::wgt(begin, end, scale, wgt_begin); }

  //: The weight of the residual.
  //  Overriding the overloaded version wgt(u) hides the superclass'
  //  implementation of this version of wgt(). This implementation simply
  //  calls the superclass' version of the same routine.
  //  \a r is the residual and
  //  \a s is the scale for that residual.
  virtual double wgt( double r, double s ) const
    { return rrel_m_est_obj::wgt(r, s); }

 private:
  //: Squared threshold.
  double T_sqr_;
};

#endif // rrel_trunc_quad_obj_h_
