// This is brl/bbas/bsta/algo/bsta_fit_weibull.h
#ifndef bsta_fit_weibull_h_
#define bsta_fit_weibull_h_
//:
// \file
// \brief Fit a Weibull distribution given sufficient statistics
// \author J. L. Mundy
// \date November 8, 2008
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <bsta/bsta_weibull.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/algo/vnl_brent_minimizer.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bsta_weibull_cost_function : public vnl_cost_function
{
 public:
   //! Default constructor
  bsta_weibull_cost_function(): vnl_cost_function(1),
    mean_(1), std_dev_(1){}

  //! Construct with a specified number of unknowns
  bsta_weibull_cost_function(double mean, double std_dev):
    vnl_cost_function(1), mean_(mean), std_dev_(std_dev){}

  ~bsta_weibull_cost_function() override = default;

  //:  The main function.  Given the parameter vector x, compute the value of f(x).
  double f(vnl_vector<double> const& x) override;

  //:  Calculate the gradient of f at parameter vector x.
  void gradf(vnl_vector<double> const& x, vnl_vector<double>& gradient) override;
  //: sample mean
  double mean() const {return mean_;}

  //: sample standard deviation
  double std_dev() const {return std_dev_;}

  //: Weibull scale parameter from sample mean and k
  double lambda(double k) const;

 private:
  double mean_;
  double std_dev_;
};

template <class T>
class bsta_fit_weibull
{
 public:
  bsta_fit_weibull() : wcf_(nullptr), residual_(T(0)){}
  bsta_fit_weibull(bsta_weibull_cost_function* wcf) : wcf_(wcf), residual_(T(0)){}

  void set_cost_function(bsta_weibull_cost_function* wcf)
    {wcf_ = wcf;}

  //:provides an initial guess for k
  bool init(T& k)
  {
    if (!wcf_) return false;
    double m = wcf_->mean();
    double sd = wcf_->std_dev();
    if (!sd) return false;
    double ki = 1.0 + 1.21*((m/sd)-1.0);
    if (ki>0) {
      if (ki < 1.0)
        ki = 1.0001;  // we need it to be ki > 1.0
      k = static_cast<T>(ki);
      return true;
    } else
      return false;
  }

  //:solves for k that matches the sample mean and variance ratio
  void solve(T& k)
  {
    double kinit = static_cast<double>(k);
    vnl_brent_minimizer bmin(*wcf_);
    double kmin = bmin.minimize_given_bounds(1,kinit,10.0*kinit);
    double res = bmin.f_at_last_minimum();
    k = static_cast<T>(kmin);
    residual_ = static_cast<T>(res);
  }
  //: the residual after solving
  T residual() const {return residual_;}

  //: the Weibull scale parameter
  T lambda(T const& k) const
  {
    if (wcf_)
      return static_cast<T>(wcf_->lambda(k));
    return T(0);
  }

 private:
  bsta_weibull_cost_function* wcf_;
  T residual_;
};

#endif // bsta_fit_weibull_h_
