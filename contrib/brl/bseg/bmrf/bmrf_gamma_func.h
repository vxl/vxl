// This is brl/bseg/bmrf/bmrf_gamma_func.h
#ifndef bmrf_gamma_func_h_
#define bmrf_gamma_func_h_
//:
// \file
// \brief Gamma function object
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 03/11/04
//
// This file contains a base class for the gamma function object
// and several specific functions derived from it.  These functions are used
// in bmrf_epi_transform.h to compute transformations of curves along epipolar
// lines.  The function returns a gamma value give alpha and time.
//
// \verbatim
//  Modifications
// \endverbatim

#include <vbl/vbl_ref_count.h>

//: Gamma function base class
class bmrf_gamma_func : public vbl_ref_count
{
 public:
  bmrf_gamma_func() : vbl_ref_count() {}
  bmrf_gamma_func(bmrf_gamma_func const& ) : vbl_ref_count() {}

  //: Return the gamma value for any \p alpha and time \p t.
  // Calls a pure virtual function
  double operator () (double alpha, double t=1.0) const
  { return this->value(alpha, t); }

  //: Returns the average gamma value at time \p t
  virtual double mean(double t=1.0) const = 0;

 protected:
  //: Return the gamma value for any \p alpha and time \p t
  virtual double value(double alpha, double t) const = 0;
};


//: A constant gamma function
class bmrf_const_gamma_func : public bmrf_gamma_func
{
 public:
  //: Constructor
  explicit bmrf_const_gamma_func(double gamma = 0.0) : gamma_(gamma) {}

  //: Set the constant gamma value
  void set_gamma(double gamma) { gamma_ = gamma; }

  //: Returns the constant average gamma value at time \p t.
  //  Here, this value is independent of \t t
  double mean(double /*t*/=1.0) const { return gamma_; }

 protected:
  //: Return the constant gamma value for any \p alpha and time \p t.
  //  Here, gamma is independent of \p alpha and of \p t
  virtual double value(double /*alpha*/, double /*t*/) const { return gamma_; }

 private:
  //: The constant gamma value
  double gamma_;
};


//: A linear gamma function
class bmrf_linear_gamma_func : public bmrf_gamma_func
{
 public:
  // Constructor
  bmrf_linear_gamma_func(double m = 0.0, double b = 0.0) : m_(m), b_(b) {}

  //: Set the linear function parameters
  void set_params(double m, double b) { m_ = m; b_ = b; }

  //: Return the average gamma value at time \p t
  //  Here, this value is independent of t
  virtual double mean(double /*t*/=1.0) const { return b_; }

 protected:
  //: Return the gamma value for any \p alpha and time \p t
  //  Here, this value is independent of t
  virtual double value(double alpha, double /*t*/) const { return m_*alpha+b_; }

 private:
  //: The function parameters
  double m_, b_;
};

#endif // bmrf_gamma_func_h_
