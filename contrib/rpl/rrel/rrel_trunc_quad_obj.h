#ifndef rrel_trunc_quad_obj_h_
#define rrel_trunc_quad_obj_h_

//:
// \file
// \author Chuck Stewaty (stewart@cs.rpi.edu)
// \author Amitha Perera (perera@cs.rpi.edu)
// Truncated quadratic loss function.

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

//  where u is a scale-normalized residual (\f$ r/\sigma \f$) and \f$
//  T \f$ is a threshold. This has been used in a number of vision
//  applications despite the fact that the wgt function is
//  discontinuous. This is the same cost function used in the MSAC
//  generalisation of RANSAC.

class rrel_trunc_quad_obj : public rrel_m_est_obj {
public:
  //: Constructor.
  //  \a scale_mult * scale is the truncation threshold.
  rrel_trunc_quad_obj( double scale_mult = 2.0 );

  //: Destructor.
  virtual ~rrel_trunc_quad_obj();

  //: The robust loss function for the M-estimator.
  virtual double rho( double u ) const;

  //: The weight of the residual.
  virtual double wgt( double u ) const;

private:
  //: Squared threshold.
  double T_sqr_;
};

#endif
