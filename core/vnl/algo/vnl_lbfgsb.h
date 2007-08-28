// This is core/vnl/algo/vnl_lbfgsb.h
#ifndef vnl_lbfgsb_h_
#define vnl_lbfgsb_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Limited memory Broyden Fletcher Goldfarb Shannon constrained opt
// \author Brad King, Kitware Inc.
// \date   28 Aug 07
//
// \verbatim
// Modifications
//  070828 BJK Initial version.
// \endverbatim
//

#include <vnl/vnl_cost_function.h>
#include <vnl/vnl_nonlinear_minimizer.h>
#include <vnl/vnl_vector.h>

//: Limited memory Broyden Fletcher Goldfarb Shannon minimization with
// constraints.  Lower and upper bounds may be specified for the
// variables to be optimized.  The algorithm miminizes a nonlinear
// function f(x) of n variables subject to simple bound constraints of
// l <= x <= u.

class vnl_lbfgsb : public vnl_nonlinear_minimizer
{
 public:
  vnl_lbfgsb();
  vnl_lbfgsb(vnl_cost_function& f);

  bool minimize(vnl_vector<double>& x);

  void set_bound_selection(vnl_vector<long> const& nbd)
    { this->bound_selection_ = nbd; }
  void get_bound_selection(vnl_vector<long>& nbd)
    { nbd = this->bound_selection_; }

  void set_lower_bound(vnl_vector<double> const& l)
    { this->lower_bound_ = l; }
  void get_lower_bound(vnl_vector<double>& l)
    { l = this->lower_bound_; }

  void set_upper_bound(vnl_vector<double> const& u)
    { this->upper_bound_ = u; }
  void get_upper_bound(vnl_vector<double>& u)
    { u = this->upper_bound_; }

protected:

  vnl_vector<double> lower_bound_;
  vnl_vector<double> upper_bound_;
  vnl_vector<long> bound_selection_;
  long max_corrections_;
  double convergence_factor_;
  double projected_gradient_tolerance_;

private:
  void init_parameters();
  vnl_cost_function* f_;
};

#endif // vnl_lbfgsb_h_
