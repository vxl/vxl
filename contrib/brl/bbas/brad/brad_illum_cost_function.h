#ifndef brad_illum_cost_function_h_
#define brad_illum_cost_function_h_
//:
// \file
// \brief A cost function for determining surface normal from illumination
// \author J.L. Mundy
// \date December 31, 2009
//
// \verbatim
// \endverbatim
#include <iostream>
#include <cmath>
#include <vnl/vnl_cost_function.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class brad_illum_cost_function : public vnl_cost_function
{
 public:
  brad_illum_cost_function(vnl_vector<double> u, vnl_vector<double> v,
                           double theta_min, double theta_max,
                           double a_uu, double a_uv, double a_vv,
                           double a_uh, double a_vh, double a_hh)
    : vnl_cost_function(1), theta_min_(theta_min),  theta_max_(theta_max),
      u_(u), v_(v), a_uu_(a_uu), a_uv_(a_uv), a_vv_(a_vv),
      a_uh_(a_uh), a_vh_(a_vh), a_hh_(a_hh){}

  ~brad_illum_cost_function() override = default;

  //:  The main function.  Given the parameter vector x, compute the value of f(x).
  double f(vnl_vector<double> const& x) override{
    double theta = x[0];
    double c = std::cos(theta), s = std::sin(theta);
    double temp = a_uu_*c*c + 2.0*a_uv_*s*c + a_vv_*s*s;
    temp -= 2*a_uh_*c;
    temp -= 2*a_vh_*s;
    temp += a_hh_;
    return temp;
  }

  //:setup for brent minimizer
  void determine_brackets(double& a, double& b, double& c, double interval = 0.1)
  {
    double min = 1.0e10, vmin = 0;
    vnl_vector<double> x(1);
    for (double v = theta_min_; v<=theta_max_; v+=interval)
    {
      x[0]=v;
      double temp = f(x);
      if (temp<min){
        min = temp;
        vmin = v;
      }
    }
    a = vmin-interval;
    b = vmin;
    c = vmin+interval;
  }

 private:
  double theta_min_, theta_max_;
  vnl_vector<double> u_, v_;
  double a_uu_, a_uv_, a_vv_, a_uh_, a_vh_, a_hh_;
};

#endif // brad_illum_cost_function_h_
