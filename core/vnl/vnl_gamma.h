// This is vxl/vnl/vnl_gamma.h
#ifndef vnl_gamma_h_
#define vnl_gamma_h_

#include <vcl_cmath.h>

//:
//  \file
//  \brief Complete and incomplete gamma function approximations
//  \author Tim Cootes

//: Approximate log of gamma function
//  Uses 6 parameter Lanczos approximation as described by Toth
//  (http://www.rskey.org/gamma.htm)
//  Accurate to about one part in 3e-11.
double vnl_log_gamma(double x);

//: Approximate gamma function
//  Uses 6 parameter Lanczos approximation as described by Toth
//  (http://www.rskey.org/gamma.htm)
//  Accurate to about one part in 3e-11.
double vnl_gamma(double x) { return vcl_exp(vnl_log_gamma(x)); }

//: Normalised Incomplete gamma function, P(a,x)
// $P(a,x)=\frac{1}{\Gamma(a)}\int_0^x e^{-t}t^{a-1}dt
double vnl_gamma_p(double a, double x);

//:Normalised Incomplete gamma function, Q(a,x)
// $Q(a,x)=\frac{1}{\Gamma(a)}\int_x^{\infty}e^{-t}t^{a-1}dt
double vnl_gamma_q(double a, double x);

//: The Error function
// erf(x) = (2/sqrt(pi)) Integral from 0 to x (exp(-t^2) dt)
inline double vnl_erf(double x)
{ return (x<0)?-vnl_gamma_p(0.5,x*x):vnl_gamma_p(0.5,x*x); };



#endif