// This is mul/vpdfl/vpdfl_axis_gaussian.cxx
//:
// \file
// \author Tim Cootes
// \date 12-Apr-2001
// \brief Multi-variate Gaussian PDF, with a diagonal covariance matrix

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include "vpdfl_axis_gaussian.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <cassert>
#include <vsl/vsl_indent.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>
#include <vpdfl/vpdfl_sampler_base.h>
#include <vpdfl/vpdfl_prob_chi2.h>


//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_axis_gaussian::vpdfl_axis_gaussian() = default;

//=======================================================================
// Destructor
//=======================================================================

vpdfl_axis_gaussian::~vpdfl_axis_gaussian() = default;

//=======================================================================

void vpdfl_axis_gaussian::calcLogK()
{
  const double *v_data = variance().data_block();
  int n = n_dims();
  double log_v_sum = 0.0;
  for (int i=0;i<n;i++)
    log_v_sum+=std::log(v_data[i]);

  log_k_ = -0.5 * (n*std::log(vnl_math::twopi) + log_v_sum);
}

void vpdfl_axis_gaussian::calcSD()
{
  sd_ = variance();
  int n = sd_.size();
  for (int i=0;i<n;i++) sd_[i] = std::sqrt(sd_[i]);
}


void vpdfl_axis_gaussian::set(const vnl_vector<double>& m,
                              const vnl_vector<double>& v)
{
  set_mean(m);
  set_variance(v);

  calcLogK();
  calcSD();
}

// ====================================================================

//: Mahalanobis distance squared from the mean.
double vpdfl_axis_gaussian::dx_sigma_dx(const vnl_vector<double> &x) const
{
  int n = x.size();
#ifndef NDEBUG
  if (n!=n_dims())
  {
    std::cerr<<"ERROR: vpdfl_axis_gaussian::log_p: Target vector has "
            <<n<<" dimensions, not the required "<<n_dims()<<'\n';
    std::abort();
  }
#endif

  const double* x_data = x.data_block();
  const double* m_data = mean().data_block();
  const double* v_data = variance().data_block();

  double sum=0.0;
  for (int i=0;i<n;++i)
  {
    double dx=x_data[i]-m_data[i];
    sum+=(dx*dx)/v_data[i];
  }
  return sum;
}

  // Probability densities:
double vpdfl_axis_gaussian::log_p(const vnl_vector<double>& x) const
{
  return log_k() - 0.5*dx_sigma_dx(x);
}

void vpdfl_axis_gaussian::gradient(vnl_vector<double>& g,
                                   const vnl_vector<double>& x,
                                   double& p) const
{
  unsigned int n = n_dims();
  assert(x.size() == n);

  if (g.size()!=n) g.set_size(n);

  double* g_data = g.data_block();
  const double* x_data = x.data_block();
  const double* m_data = mean().data_block();
  const double* v_data = variance().data_block();

  double sum=0.0;

  for (unsigned int i=0;i<n;++i)
  {
    double dx=x_data[i]-m_data[i];
    sum+=(dx*dx)/v_data[i];
    g_data[i]= -dx/v_data[i];
  }

  p = std::exp(log_k() - 0.5*sum);

  g*=p;
}

//: Gradient and value of log(p(x)) at x
//  Computes gradient df/dx of f(x)=log(p(x)) at x.
void vpdfl_axis_gaussian::gradient_logp(vnl_vector<double>& g,
                                        const vnl_vector<double>& x) const
{
  unsigned int n = n_dims();
  assert(x.size() == n);

  if (g.size()!=n) g.set_size(n);

  double* g_data = g.data_block();
  const double* x_data = x.data_block();
  const double* m_data = mean().data_block();
  const double* v_data = variance().data_block();

  for (unsigned int i=0;i<n;++i)
  {
    g_data[i]= (m_data[i]-x_data[i])/v_data[i];
  }
}

// ====================================================================

vpdfl_sampler_base* vpdfl_axis_gaussian::new_sampler() const
{
  auto *i = new vpdfl_axis_gaussian_sampler;
  i->set_model(*this);
  return i;
}


double vpdfl_axis_gaussian::log_prob_thresh(double pass_proportion) const
{
  // The Mahalanobis distance of n-D Gaussian is distributed as Chi^2(n),
  // by definition, Chi^2 is the sum of independent Normal RVs.
  return log_k() - 0.5 * vpdfl_chi2_for_cum_prob (pass_proportion, n_dims());
}


void vpdfl_axis_gaussian::nearest_plausible(vnl_vector<double>& x,
                                            double log_p_min) const
{
  unsigned n = x.size();

  // calculate radius of plausible region in standard deviations.
  log_p_min -= log_k();
  assert(log_p_min <0); // Check sd_limit is positive and real.

  const double sd_limit_sqr = -2.0*log_p_min;
  // distance to x in standard deviations
  const double x_dist_sqr = dx_sigma_dx(x);

  if (sd_limit_sqr >= x_dist_sqr) return;

  const double corrective_factor = std::sqrt(sd_limit_sqr / x_dist_sqr);

  for (unsigned i=0;i<n;++i)
    x(i) = ((x(i)-mean()(i)) * corrective_factor) + mean()(i);
}

//=======================================================================

std::string vpdfl_axis_gaussian::is_a() const
{
  static const std::string s_ = "vpdfl_axis_gaussian";
  return s_;
}

//=======================================================================

bool vpdfl_axis_gaussian::is_class(std::string const& s) const
{
  return s==vpdfl_axis_gaussian::is_a() || vpdfl_pdf_base::is_class(s);
}

//=======================================================================

short vpdfl_axis_gaussian::version_no() const
{
  return 1;
}

//=======================================================================

vpdfl_pdf_base* vpdfl_axis_gaussian::clone() const
{
  return new vpdfl_axis_gaussian(*this);
}

//=======================================================================

void vpdfl_axis_gaussian::print_summary(std::ostream& os) const
{
  vpdfl_pdf_base::print_summary(os);
}

//=======================================================================

void vpdfl_axis_gaussian::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vpdfl_pdf_base::b_write(bfs);
}

//=======================================================================

void vpdfl_axis_gaussian::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vpdfl_pdf_base::b_read(bfs);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_axis_gaussian &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }

  calcLogK();
  calcSD();
}
