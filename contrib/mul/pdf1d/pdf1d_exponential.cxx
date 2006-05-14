// This is mul/pdf1d/pdf1d_exponential.cxx

//:
// \file
// \brief Univariate exponential PDF.
// \author Tim Cootes

#include "pdf1d_exponential.h"

#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_cmath.h>

#include <pdf1d/pdf1d_exponential_sampler.h>
#include <pdf1d/pdf1d_sampler.h>

//=======================================================================

pdf1d_exponential::pdf1d_exponential()
{
  set_lambda(1);
}

pdf1d_exponential::pdf1d_exponential(double lambda)
{
  set_lambda(lambda);
}

//=======================================================================

pdf1d_exponential::~pdf1d_exponential()
{
}

//=======================================================================

//: Initialise
void pdf1d_exponential::set_lambda(double lambda)
{
  assert(lambda>0);
  lambda_ = lambda;
  log_lambda_ = vcl_log(lambda_);

  pdf1d_pdf::set_mean(1.0/lambda);
  pdf1d_pdf::set_variance(1.0/(lambda*lambda));
}

//=======================================================================


pdf1d_sampler* pdf1d_exponential::new_sampler() const
{
  pdf1d_exponential_sampler *i = new pdf1d_exponential_sampler;
  i->set_model(*this);
  return i;
}

//=======================================================================
//: Probability density at x
double pdf1d_exponential::operator()(double x) const
{
  if (x<0) return 0;
  return lambda_ * vcl_exp(-1*lambda_*x);
}


  // Probability densities:
double pdf1d_exponential::log_p(double x) const
{
  if (x<0) return -9999;  // Very unlikely!

  return log_lambda_ - lambda_*x;
}
//: Cumulative Probability (P(x'<x) for x' drawn from the distribution)
double pdf1d_exponential::cdf(double x) const
{
  return 1.0 - vcl_exp(-1*lambda_*x);
}

//: Return true if cdf() uses an analytic implementation
bool pdf1d_exponential::cdf_is_analytic() const
{
  return true;
}

//=======================================================================


double pdf1d_exponential::gradient(double x,
                                   double& p) const
{
  if (x<0)
  {
    p=0;
    return 0;
  }

  p = lambda_ * vcl_exp(-1*lambda_*x);
  return -1.0*lambda_*p;
}

// ====================================================================


double pdf1d_exponential::log_prob_thresh(double pass_proportion) const
{
  if (pass_proportion<=0) return 0.0;
  if (pass_proportion>=0) return lambda_;

  // CFD = 1-exp(-Lx)
  // So exp(-Lx)=(1-pp)
  return lambda_ * (1-pass_proportion);
}

//=======================================================================

double pdf1d_exponential::nearest_plausible(double x, double log_p_min) const
{
  if (x<=0) return 0;
  double x_lim = (log_lambda_-log_p_min)/lambda_;
  if (x>x_lim) return x_lim;
  return x;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string pdf1d_exponential::is_a() const
{
  static vcl_string class_name_ = "pdf1d_exponential";
  return class_name_;
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_exponential::is_class(vcl_string const& s) const
{
  return pdf1d_pdf::is_class(s) || s==pdf1d_exponential::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short pdf1d_exponential::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_pdf* pdf1d_exponential::clone() const
{
  return new pdf1d_exponential(*this);
}

//=======================================================================
// Method: print
//=======================================================================


void pdf1d_exponential::print_summary(vcl_ostream& os) const
{
  os<<"Lambda="<<lambda_;
}

//=======================================================================
// Method: save
//=======================================================================

void pdf1d_exponential::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,lambda_);
}

//=======================================================================
// Method: load
//=======================================================================

void pdf1d_exponential::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  vcl_string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_exponential &)\n";
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
      vsl_b_read(bfs,lambda_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_exponential &)\n";
      vcl_cerr << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }

  set_lambda(lambda_);
}

//==================< end of pdf1d_exponential.cxx >====================
