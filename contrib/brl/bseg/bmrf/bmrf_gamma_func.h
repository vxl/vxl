// This is contrib/brl/bseg/bmrf/bmrf_gamma_func.h
#ifndef bmrf_gamma_func_h_
#define bmrf_gamma_func_h_
//:
// \file
// \brief Gamma function object
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 02/24/04
//
// This file contains a base class for the gamma function object
// and several specific functions derived from it.  These functions are used
// in bmrf_epi_transform.h to compute tranformations of curves along epipolar
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
  //: Return the gamma value for any alpha \palpha and time \pt
  // calls a pure virtual function  
  double operator () (double alpha, double t=1.0) const 
  { return this->value(alpha, t); }

  //: Returns the average gamma value at time \pt
  virtual double mean(double t=1.0) const = 0;

 protected:
  //: Return the gamma value for any alpha \pa and time \pt
  virtual double value(double alpha, double t) const = 0;

};


//: A constant gamma function
class bmrf_const_gamma_func : public bmrf_gamma_func
{
 public:
  //: Constructor
  explicit bmrf_const_gamma_func(double gamma = 0.0);

  //: Set the constant gamma value
  void set_gamma(double gamma);

  //: Returns the average gamma value at time \pt
  virtual double mean(double t=1.0) const;

 protected:
  //: Return the gamma value for any alpha \pa and time \pt
  virtual double value(double alpha, double t) const;
  
 private:
  //: The constant gamma value
  double gamma_;
};


//: A linear gamma function
class bmrf_linear_gamma_func : public bmrf_gamma_func
{
 public:
  //: Constructor
  bmrf_linear_gamma_func(double m = 0.0, double b = 0.0);

  //: Set the linear function paramaters
  void set_params(double m, double b);

  //: Returns the average gamma value at time \pt
  virtual double mean(double t=1.0) const;

 protected:
  //: Return the gamma value for any alpha \pa and time \pt
  virtual double value(double alpha, double t) const;

 private:
  //: The function parameters
  double m_, b_;
};

#endif // bmrf_gamma_func_h_
