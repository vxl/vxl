#ifndef rrel_tukey_obj_h_
#define rrel_tukey_obj_h_

//:
// \file
// \author Amitha Perera (perera@cs.rpi.edu)
// Beaton-Tukey loss functon.

#include <rrel/rrel_m_est_obj.h>

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

class rrel_tukey_obj : public rrel_m_est_obj {
public:
  //: Constructor.
  //  \a B is the normalised residual value at which the cost becomes
  //  constant.
  rrel_tukey_obj( double B );

  //: Destructor.
  virtual ~rrel_tukey_obj();

  //: The robust loss function for the M-estimator.
  virtual double rho( double u ) const;

  //: The weight of the residual.
  virtual double wgt( double u ) const;

private:
  double B_;
};

#endif
