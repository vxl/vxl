// This is mul/pdf1d/pdf1d_gaussian.cxx

//:
// \file
// \brief Univariate Gaussian PDF.
// \author Tim Cootes

#include "pdf1d_gaussian.h"

#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_cmath.h>

#include <vnl/vnl_math.h>
#include <pdf1d/pdf1d_gaussian_sampler.h>
#include <pdf1d/pdf1d_sampler.h>
#include <pdf1d/pdf1d_prob_chi2.h>
#include <vnl/vnl_erf.h>


//=======================================================================

pdf1d_gaussian::pdf1d_gaussian()
{
  set(0,1);
}

pdf1d_gaussian::pdf1d_gaussian(double mean, double variance)
{
  set(mean,variance);
}

//=======================================================================

pdf1d_gaussian::~pdf1d_gaussian()
{
}

//=======================================================================

void pdf1d_gaussian::calc_log_k()
{
  k_ = 1.0/vcl_sqrt(2*vnl_math::pi*variance());
  log_k_ = vcl_log(k_);
}

//: Initialise with mean and variance (NOT standard deviation)
void pdf1d_gaussian::set(double mean, double var)
{
  pdf1d_pdf::set_mean(mean);
  pdf1d_pdf::set_variance(var);
  sd_ = vcl_sqrt(var);
  calc_log_k();
}

//: Modify just the mean of the distribution
// This functions should only be used by builders.
void pdf1d_gaussian::set_mean(double mean)
{
  pdf1d_pdf::set_mean(mean);
}


//=======================================================================


pdf1d_sampler* pdf1d_gaussian::new_sampler() const
{
  pdf1d_gaussian_sampler *i = new pdf1d_gaussian_sampler;
  i->set_model(*this);
  return i;
}

//=======================================================================


  // Probability densities:
double pdf1d_gaussian::log_p(double x) const
{
  double v = variance();
  assert(v>0);

  double dx = x-mean();
  return log_k_ - 0.5*dx*dx/v;
}

//: Cumulative Probability (P(x'<x) for x' drawn from the distribution)
double pdf1d_gaussian::cdf(double x) const
{
  double dx = (x-mean())/(vnl_math::sqrt2*sd_);
  return 0.5*(vnl_erfc(-dx));
}

//: Return true if cdf() uses an analytic implementation
bool pdf1d_gaussian::cdf_is_analytic() const
{
  return true;
}


//=======================================================================


double pdf1d_gaussian::gradient(double x,
                                double& p) const
{
  double v = variance();
  assert(v>0);

  double dx = x-mean();
  p = k_ * vcl_exp( -0.5*dx*dx/v);

  return -1.0*dx*p/v;
}

// ====================================================================


double pdf1d_gaussian::log_prob_thresh(double pass_proportion) const
{
  // The Mahalanobis distance of n-D Gaussian is distributed as Chi^2(n),
  // by definition, Chi^2 is the sum of independent Normal RVs.
  return log_k() - 0.5 * pdf1d_chi2_for_cum_prob (pass_proportion, 1);
}

//=======================================================================

double pdf1d_gaussian::nearest_plausible(double x, double log_p_min) const
{
  // calculate radius of plausible region in standard deviations.
  log_p_min -= log_k();
  assert(log_p_min <0); // Check sd_limit is positive and real.
  const double sd_limit = vcl_sqrt(-2.0*log_p_min);

  double dx = x-mean();

  double limit = sd_limit * sd_;
  double lo = -1.0 * limit;
  double hi = limit;

  if (dx<lo) dx=lo;
  else
    if (dx>hi) dx=hi;

  return mean()+dx;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string pdf1d_gaussian::is_a() const
{
  static vcl_string class_name_ = "pdf1d_gaussian";
  return class_name_;
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_gaussian::is_class(vcl_string const& s) const
{
  return pdf1d_pdf::is_class(s) || s==pdf1d_gaussian::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short pdf1d_gaussian::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_pdf* pdf1d_gaussian::clone() const
{
  return new pdf1d_gaussian(*this);
}

//=======================================================================
// Method: print
//=======================================================================


void pdf1d_gaussian::print_summary(vcl_ostream& os) const
{
  pdf1d_pdf::print_summary(os);
  os << vcl_endl;
}

//=======================================================================
// Method: save
//=======================================================================

void pdf1d_gaussian::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  pdf1d_pdf::b_write(bfs);
  vsl_b_write(bfs,log_k_);
}

//=======================================================================
// Method: load
//=======================================================================

void pdf1d_gaussian::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  vcl_string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_gaussian &)\n";
    vcl_cerr << "           Attempted to load object of type ";
    vcl_cerr << name <<" into object of type " << is_a() << vcl_endl;
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      pdf1d_pdf::b_read(bfs);
      vsl_b_read(bfs,log_k_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_gaussian &)\n";
      vcl_cerr << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }

  k_ = vcl_exp(log_k_);
  sd_ = vcl_sqrt(variance());
}

//==================< end of pdf1d_gaussian.cxx >====================
