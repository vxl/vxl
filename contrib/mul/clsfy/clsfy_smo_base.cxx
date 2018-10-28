// This is mul/clsfy/clsfy_smo_base.cxx
#include "clsfy_smo_base.h"
//:
// \file
// \author Ian Scott
// \date 14-Nov-2001
// \brief Sequential Minimum Optimisation algorithm
// This code is based on the C++ code of
// Xianping Ge, ( http://www.ics.uci.edu/~xge ) which he kindly
// put in the public domain.
// That code was in turn based on the algorithms of
// John Platt, ( http://research.microsoft.com/~jplatt ) described in
// Platt, J. C. (1998). Fast Training of Support Vector Machines Using Sequential
// Minimal Optimisation. In Advances in Kernel Methods - Support Vector Learning.
// B. Scholkopf, C. Burges and A. Smola, MIT Press: 185-208. and other papers.

#include <cassert>

// ----------------------------------------------------------------

double clsfy_smo_base::error()
{
  return error_;
}

// ----------------------------------------------------------------

//: Access the data points
const vnl_vector<double> & clsfy_smo_base::data_point(unsigned long l)
{
  data_->set_index(l);
  return data_->current();
}

// ----------------------------------------------------------------

double clsfy_smo_base::learned_func(int k)
{
  double s = -b_;
  const unsigned long N = data_->size();
  for (unsigned int i=0; i<N; i++)
    if (alph_[i] > 0)
      s += alph_[i]*target_[i]*kernel(i,k);

  return s;
}

// ----------------------------------------------------------------

//: Get the optimised parameters
const vnl_vector<double>& clsfy_smo_base::lagrange_mults() const
{
  return alph_;
}

// ----------------------------------------------------------------

//: Set the initial values of the parameters to be optimised.
// The caller is responsible for ensuring that the initial values
// fulfill the constraints;
void clsfy_smo_base::set_lagrange_mults(const vnl_vector<double>& lagrange_mults)
{
  alph_ = lagrange_mults;
}

// ----------------------------------------------------------------

double clsfy_smo_base::bias()
{
  return b_;
}

// ----------------------------------------------------------------

//: Reseeds the internal random number generator.
// To achieve quasi-random initialisation use;
// \code
// #include <vcl_compiler.h>
// #include <iostream>
// #include <ctime>
// ..
// sampler.reseed(std::time(0));
// \endcode
void clsfy_smo_base::reseed(unsigned long seed)
{
  rng_.reseed(seed);
}

// ----------------------------------------------------------------

//: Amount by which a sample can violate the KKT conditions
const double& clsfy_smo_base::tolerance() const
{
  return tolerance_;
}

// ----------------------------------------------------------------

//: Set the amount by which a sample can violate the KKT conditions.
// Default value is 0.001
void clsfy_smo_base::set_tolerance(double tolerance)
{
  assert(tolerance >= 0.0);
  tolerance_ = tolerance;
}

// ----------------------------------------------------------------

//: Tolerance on several equalities.
// Including testing if a Lagrange multiplier is at one of the bounds.
double clsfy_smo_base::eps() const
{
  return eps_;
}

// ----------------------------------------------------------------

//: Set the tolerance on several equalities.
// Including testing if a Lagrange multiplier is at one of the bounds.
// Default value is 0.001;
void clsfy_smo_base::set_eps(double eps)
{
  assert(eps >= 0.0);
  eps_ = eps;
}

// ----------------------------------------------------------------

clsfy_smo_base::clsfy_smo_base():
  error_(0.0), data_(nullptr), tolerance_(0.001), eps_(0.001), b_(0.0), rng_(9667566)
{
}

// ----------------------------------------------------------------

clsfy_smo_base::~clsfy_smo_base()
{
  delete data_;
}

// ----------------------------------------------------------------

double clsfy_smo_base::error_rate()
{
    int n_total = 0;
    int n_error = 0;
    for (unsigned int i=0; i<data_->size(); ++i) {
      if ((learned_func(i) > 0) != (target_[i] > 0)) // meaning: signs are different
        ++n_error;
      ++n_total;
    }
    return double(n_error)/double(n_total);
}
