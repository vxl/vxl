// This is brl/bseg/bmrf/bmrf_gamma_func.cxx
#include "bmrf_gamma_func.h"
//:
// \file

//----------------bmrf_const_gamma_func functions --------------------

//: Constructor
bmrf_const_gamma_func::bmrf_const_gamma_func(double gamma)
 : gamma_(gamma)
{
}


//: Set the constant gamma value
void
bmrf_const_gamma_func::set_gamma(double gamma)
{
  gamma_ = gamma;
}


//: Returns the constant gamma value
// Here, gamma is independent of alpha and of t
double
bmrf_const_gamma_func::value(double /*alpha*/, double /*t*/) const
{
  return gamma_;
}


//: Returns the constant gamma value
// Here, this mean gamma is independent of t
double
bmrf_const_gamma_func::mean(double /*t*/) const
{
  return gamma_;
}

//----------------bmrf_linear_gamma_func functions --------------------

//: Constructor
bmrf_linear_gamma_func::bmrf_linear_gamma_func(double m, double b)
 : m_(m), b_(b)
{
}


//: Set the parameters
void
bmrf_linear_gamma_func::set_params(double m, double b)
{
  m_ = m;
  b_ = b;
}


//: Returns the gamma value
// Here, gamma is independent of t
double
bmrf_linear_gamma_func::value(double alpha, double /*t*/) const
{
  return m_*alpha + b_;
}

//: Returns the mean gamma value
// Here, this mean gamma is independent of t
double
bmrf_linear_gamma_func::mean(double /*t*/) const
{
  return b_;
}
