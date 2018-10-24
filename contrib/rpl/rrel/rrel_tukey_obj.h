#ifndef rrel_tukey_obj_h_
#define rrel_tukey_obj_h_
//:
// \file
// \author Amitha Perera (perera@cs.rpi.edu)
// \brief Beaton-Tukey loss function.

#include <rrel/rrel_m_est_obj.h>
#include <vnl/vnl_math.h>

//: Beaton-Tukey biweight.
//  The cost function for the Beaton-Tukey biweight is
//  \f[
//    \rho(u) =
//    \left\{
//      \begin{array}{ll}
//        \frac{B^2}{6} ( 1 - (1-(u/B)^2)^3 ), & |u| \le B \\ B^2/6, & |u| > B
//      \end{array}
//    \right.
//  \f]
//  The associated weight function is
//  \f[
//    w(u) =
//    \left\{
//      \begin{array}{ll}
//        (1-(u/B)^2)^2, & |u| \le B \\ 0, & |u| > B
//      \end{array}
//    \right.
//  \f]
//  In this implementation, the constant \f$ \frac{B^2}{6} \f$ in \f$
//  \rho \f$ is ignored, since it doesn't affect the minimum.

class rrel_tukey_obj : public rrel_m_est_obj
{
 public:
  //: Constructor.
  //  \a B is the normalised residual value at which the cost becomes
  //  constant.
  rrel_tukey_obj( double B );

  //: Destructor.
  ~rrel_tukey_obj() override;

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
  //  Overriding the overloaded version wgt(u) hides the  superclass'
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
  double B_;
};

inline double
rrel_tukey_obj::rho_fast( double u ) const
{
  if ( u < -B_ || u > B_ )
    return 1.0;
  else
    return 1.0 - vnl_math::cube(1.0 - vnl_math::sqr(u/B_));
}

inline double
rrel_tukey_obj::wgt_fast( double u ) const
{
  if ( u < -B_ || u > B_ )
    return 0.0;
  else
    return vnl_math::sqr(1.0 - vnl_math::sqr(u/B_));
}

#endif // rrel_tukey_obj_h_
