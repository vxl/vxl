#ifndef vpdfl_gamma_h
#define vpdfl_gamma_h

//:
// \file
// \author Tim Cootes
// \date 12-Apr-2001
// \brief Gamma and Incomplete Gamma functions


//: log of Gamma function of a
double mbl_log_gamma(double a);

//: Incomplete gamma function, P(a,x)
double mbl_gamma_p(double a, double x);

//:Incomplete gamma function, Q(a,x)
double mbl_gamma_q(double a, double x);

//: log of Incomplete gamma function, Q(a,x)
double mbl_log_gamma_q(double a, double x);

#endif // vpdfl_gamma_h