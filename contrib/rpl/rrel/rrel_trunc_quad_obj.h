#ifndef rrel_trunc_quad_obj_h_
#define rrel_trunc_quad_obj_h_

//:
// \file
// \author Chuck Stewart (stewart@cs.rpi.edu)
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
  ~rrel_trunc_quad_obj() override;

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
  //: Squared threshold.
  double T_sqr_;
};


inline double
rrel_trunc_quad_obj::rho_fast( double u ) const
{
  double u_sqr = u*u;
  return ( u_sqr < T_sqr_ ) ? u_sqr : T_sqr_;
}

inline double
rrel_trunc_quad_obj::wgt_fast( double u ) const
{
  return ( u*u < T_sqr_ ) ? 1 : 0.0;
}

#endif // rrel_trunc_quad_obj_h_
