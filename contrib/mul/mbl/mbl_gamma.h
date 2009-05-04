#ifndef mbl_gamma_h
#define mbl_gamma_h
//:
// \file
// \author Tim Cootes
// \date 18-Apr-2001
// \brief Gamma and Incomplete Gamma functions

//: log of Gamma function of a
double mbl_log_gamma(double a);

//: Incomplete gamma function, P(a,x)
double mbl_gamma_p(double a, double x);

//:Incomplete gamma function, Q(a,x)
double mbl_gamma_q(double a, double x);

//: log of Incomplete gamma function, Q(a,x)
double mbl_log_gamma_q(double a, double x);

//: The Error function
// erf(x) = (2/sqrt(pi)) Integral from 0 to x (exp(-t^2) dt)
inline double mbl_erf(double x)
{ return (x<0)?-mbl_gamma_p(0.5,x*x):mbl_gamma_p(0.5,x*x); }

#endif // mbl_gamma_h
