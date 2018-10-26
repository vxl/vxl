#include "msm_param_limiter.h"
//:
// \file
// \author Tim Cootes
// \brief Base for objects with apply limits to parameters

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_exception.h>
#include <vnl/vnl_gamma.h>


//=======================================================================

void vsl_add_to_binary_loader(const msm_param_limiter& b)
{
  vsl_binary_loader<msm_param_limiter>::instance().add(b);
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const msm_param_limiter& b)
{
  b.b_write(bfs);
}

//=======================================================================
//: Initialise from a text stream.
// The default implementation is for attribute-less normalisers,
// and throws if it finds any data in the stream.
void msm_param_limiter::config_from_stream(std::istream &is)
{
  std::string s = mbl_parse_block(is);
  if (s.empty() || s=="{}") return;

  mbl_exception_parse_error x(
    this->is_a() + " expects no properties in initialisation,\n"
    "But the following properties were given:\n" + s);
  mbl_exception_error(x);
}


//=======================================================================
//: Create a concrete msm_param_limiter-derived object, from a text specification.
std::unique_ptr<msm_param_limiter> msm_param_limiter::create_from_stream(std::istream &is)
{
  std::string name;
  is >> name;

  std::unique_ptr<msm_param_limiter> ps =
    mbl_cloneables_factory<msm_param_limiter>::get_clone(name);

  ps -> config_from_stream(is);
  return ps;
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, msm_param_limiter& b)
{
  b.b_read(bfs);
}

//=======================================================================

std::ostream& operator<<(std::ostream& os,const msm_param_limiter& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================

std::ostream& operator<<(std::ostream& os,const msm_param_limiter* b)
{
  if (b)
    return os << *b;
  else
    return os << "No msm_param_limiter defined.";
}

//=======================================================================
//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_param_limiter& b)
{
  os << b;
}

//=======================================================================
//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_param_limiter* b)
{
  if (b)
    os << *b;
  else
    os << vsl_indent() << "No msm_param_limiter defined.";
}

//: Returns X such that P(chi<X | dof==n)==p
double msm_chi2_for_cum_prob(double p, int n_dof, double tol)
{
  if ((p<0) | (p>=1.0))
    mbl_exception_error(mbl_exception_abort("msm_chi2_for_cum_prob:"
      "Illegal value for probability. (Outside range [0,1) )"));

  if (p==0) return 0;

  double d_step = n_dof; // prob = 50% ish
  double low_chi = 0;
  double high_chi = d_step;

  double p_high = vnl_cum_prob_chi2(n_dof,high_chi);

  // Perform binary search for solution

  // First step along till p_high >= p
  while (p_high<p)
  {
    low_chi = high_chi;
    high_chi += d_step;
    p_high = vnl_cum_prob_chi2(n_dof,high_chi);
  }

  // p_low and p_high now straddle answer
  double mid_chi = 0.5 * (low_chi+high_chi);
  double p_mid;

  while ((mid_chi-low_chi)>tol)
  {
    p_mid = vnl_cum_prob_chi2(n_dof,mid_chi);
    if (p_mid>p)
    {
      // Use low & mid as limits
      high_chi = mid_chi;
    }
    else
    {
      // Use mid and high as limits
      low_chi = mid_chi;
    }

    mid_chi = 0.5 * (low_chi+high_chi);
  }

  return mid_chi;
}
