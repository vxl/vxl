// This is mul/pdf1d/pdf1d_pdf.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Tim Cootes
// \brief Base class for Univariate Probability Density Function classes.
// \verbatim
//  Modifications
//  IMS 28 Feb 2002 - Added inverse CDF function.
// \endverbatim

#include "pdf1d_pdf.h"
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <vnl/vnl_math.h>
#include <mbl/mbl_priority_bounded_queue.h>
#include <pdf1d/pdf1d_sampler.h>

//=======================================================================

pdf1d_pdf::pdf1d_pdf()
  : mean_(0.0),var_(0.0)
{
}

//=======================================================================

pdf1d_pdf::~pdf1d_pdf()
{
}

//=======================================================================

double pdf1d_pdf::operator()(double x) const
{
  return vcl_exp(log_p(x));
}

//: Cumulative Probability (P(x'<x) for x' drawn from the distribution)
//  By default this can be calculated by drawing random samples from
//  the distribution and computing the number less than x.
double pdf1d_pdf::cdf(double x0) const
{
  pdf1d_sampler *sampler = new_sampler();

  int n_trials = 100;
  int sum = 0;
  for (int i=0;i<n_trials;++i)
    if (sampler->sample()<=x0) sum++;

  delete sampler;

  return double(sum)/n_trials;
}

//: Return true if cdf() uses an analytic implementation
//  Default is false, as the base implementation is to draw samples
//  from the distribution randomly to estimate cdf(x)
bool pdf1d_pdf::cdf_is_analytic() const
{
  return false;
}


double pdf1d_pdf::log_prob_thresh(double pass_proportion) const
{
  assert(pass_proportion >= 0.0);
  assert(pass_proportion < 1.0);

  // The number of samples on the less likely side of the boundary.
  // Increase the number for greater reliabililty
  const unsigned n_stat = 30;

  unsigned nSamples;
  double right;
  double left;

  pdf1d_sampler *sampler = new_sampler();
  if (pass_proportion > 0.5)
  {
    //We want at n_stat samples outside the cut-off.
    pass_proportion = 1 - pass_proportion;
    nSamples = (unsigned)vcl_ceil((double)n_stat / pass_proportion);

    // Find lowest values
    mbl_priority_bounded_queue<double> lowest(n_stat+1);
    for (unsigned i=0; i < nSamples; i++)
      lowest.push(operator()(sampler->sample()));

    // bracket result
    right = lowest.top();
    lowest.pop();
    left = lowest.top();
  }
  else
  {
    //We want at n_stat samples inside the cut-off.
    nSamples = (unsigned)vcl_ceil((double)n_stat / pass_proportion);

    // Find highest values
    mbl_priority_bounded_queue<double, vcl_vector<double>, vcl_greater<double> >
      highest(n_stat+1);
    for (unsigned i=0; i < nSamples; i++)
      highest.push(operator()(sampler->sample()));

    // bracket result
    right = highest.top();
    highest.pop();
    left = highest.top();
  }
  delete sampler;

  // Interpolate left and right to find value
  // check interpolation is not extrapolation.
  assert (0.0 <= pass_proportion*nSamples - n_stat &&
          pass_proportion*nSamples - n_stat <= 1.0);
  return (n_stat + 1.0 - pass_proportion*nSamples) * vcl_log(left)
         + (pass_proportion*nSamples - n_stat) * vcl_log(right);
}

//=======================================================================

bool pdf1d_pdf::is_valid_pdf() const
{
  return true;
}

//: Fill x with samples drawn from distribution
void pdf1d_pdf::get_samples(vnl_vector<double>& x) const
{
  pdf1d_sampler *sampler = new_sampler();
  sampler->get_samples(x);
  delete sampler;
}

//: Write values (x,p(x)) to text file suitable for plotting
//  Evaluate pdf at n points in range [min_x,max_x] and write a text file,
//  each line of which is {x p(x)}, suitable for plotting with many graph packages
bool pdf1d_pdf::write_plot_file(const vcl_string& plot_file, double min_x, double max_x, int n) const
{
  vcl_ofstream ofs(plot_file.c_str(),vcl_ios::out);
  if (!ofs) return false;
  assert(n>1);

  double dx = (max_x-min_x)/(n-1);
  for (int i=0;i<n;++i)
  {
    double x = min_x + i*dx;
    ofs<<x<<' '<<operator()(x)<<'\n';
  }
  ofs.close();

  return true;
}


//=======================================================================

short pdf1d_pdf::version_no() const
{
  return 1;
}

//=======================================================================

void vsl_add_to_binary_loader(const pdf1d_pdf& b)
{
  vsl_binary_loader<pdf1d_pdf>::instance().add(b);
}

//=======================================================================

vcl_string pdf1d_pdf::is_a() const
{
  static vcl_string class_name_ = "pdf1d_pdf";
  return class_name_;
}

//=======================================================================

bool pdf1d_pdf::is_class(vcl_string const& s) const
{
  return s==pdf1d_pdf::is_a();
}

//=======================================================================

  // required if data is present in this base class
void pdf1d_pdf::print_summary(vcl_ostream& os) const
{
  os << "  Mean: " << mean_
     << "  Variance: " << var_;
}

//=======================================================================

  // required if data is present in this base class
void pdf1d_pdf::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no());
  vsl_b_write(bfs, mean_);
  vsl_b_write(bfs, var_);
}

//=======================================================================

  // required if data is present in this base class
void pdf1d_pdf::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,mean_);
      vsl_b_read(bfs,var_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_pdf &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const pdf1d_pdf& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, pdf1d_pdf& b)
{
  b.b_read(bfs);
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const pdf1d_pdf& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const pdf1d_pdf* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No pdf1d_pdf defined.";
}

//=======================================================================

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const pdf1d_pdf& b)
{
  vsl_print_summary(os,b);
  return os;
}

//=======================================================================

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const pdf1d_pdf* b)
{
  vsl_print_summary(os,b);
  return os;
}

//=======================================================================

//: The inverse cumulative distribution function.
// The value of x: P(x'<x) = P for x' drawn from distribution pdf.
double pdf1d_pdf::inverse_cdf(double P) const
{
  if (cdf_is_analytic())
  {
  // Use Chebyshev inequality to get starting point.
  // P[ |x-mean| >= k] <= var/k^2.
    double x_init;
    if (P < 0.5)
      x_init = mean() - vcl_sqrt(variance() / (2*P));
    else
      x_init = mean() + vcl_sqrt(variance() / (2*(1-P)));

    double f_init = cdf(x_init);

    // guess initial step size assuming a rectangular distribution.
    // slope = 1 / sqrt(12 * variance)
    double step = 2.0 * vnl_math_abs(f_init - P)*vcl_sqrt(12 * variance());

    double x_above, x_below;
    if (f_init > P)
    {
      x_above = x_init;
      while (true)
      {
        x_below = x_above - step;
        double f_below = cdf(x_below);
        if (f_below < P) break;
        x_above = x_below;
        step *= 2.0;
      }
    }
    else
    {
      x_below = x_init;
      while (true)
      {
        x_above = x_below + step;
        double f_above = cdf(x_above);
        if (f_above > P) break;
        x_below = x_above;
        step *= 2.0;
      }
    }
#if 0
    double x_middle = (x_above + x_below) / 2;
    while (x_above - x_below > vnl_math::sqrteps)
    {
      double f_middle = pdf.cdf(x_middle) - P;
      if (f_middle < 0) x_below=x_middle;
      else x_above = x_middle;
    }
    return (x_above + x_below) / 2;
#endif
    // bracketed Newton-Raphson.


    double x_middle=0.5*(x_above+x_below); 
    double dxold= x_above-x_below; 
    double dx=dxold;
    double f_middle = cdf(x_middle)-P;
    double df_middle = operator() (x_middle);
    for (unsigned j=100;j>0;j--) 
    {
      if ( !((x_above - x_middle)*df_middle + f_middle > 0.0 &&
             (x_below - x_middle)*df_middle + f_middle < 0.0   ) || 
        (vnl_math_abs((2.0*f_middle)) > vnl_math_abs(dxold*df_middle)))
      { // Bisect if Newton-Raphson isn't working
        x_middle=0.5*(x_above+x_below);
        dxold=dx;
        dx=x_above-x_middle;
      } else // Newton-Raphson step
      {
        dxold=dx;
        dx=f_middle/df_middle;
        x_middle -= dx;
        assert (x_below <= x_middle && x_middle <= x_above);
      }

      if (vnl_math_abs(dx) < vnl_math_abs(x_middle * vnl_math::sqrteps))
      {
        return x_middle; // Converged .
      }

      f_middle = cdf(x_middle)-P;
      df_middle = operator()(x_middle);

      if (f_middle < 0) //bracket on the root.
        x_below=x_middle;
      else
        x_above=x_middle;
    }
    vcl_cerr << "ERROR: pdf1d_pdf::inverse_cdf() failed to converge.\n";
    vcl_abort();
    return 0.0; // dummy return
  }
  else // Use sampling.
  {
    const unsigned n_stat = 20;
    pdf1d_sampler * sampler = new_sampler();
    unsigned n;
    double left, right; // These bracket the result
    if (P < 0.5)
    {
      // we want n_stat samples below P
      n = vnl_math_rnd(vcl_ceil(n_stat / P));

      // find lowest values
      mbl_priority_bounded_queue<double> lowest(n_stat+1);
      for (unsigned i=0; i < n; i++)
        lowest.push(sampler->sample());

      // bracket result
      right = lowest.top();
      lowest.pop();
      left = lowest.top();
    }
    else
    {
      // we want n_stat samples above P
      P = 1.0 - P;
      n = vnl_math_rnd(vcl_ceil(n_stat / P));

      // find highest values
      mbl_priority_bounded_queue<double, vcl_vector<double>, vcl_greater<double> >
        highest(n_stat+1);
      for (unsigned i=0; i < n; i++)
        highest.push(sampler->sample());

      // bracket result
      right = highest.top();
      highest.pop();
      left = highest.top();
    }
    delete sampler;

    // Interpolate left and right to find value
    // check interpolation is not extrapolation.
    assert (0.0 <= P*n - n_stat && P*n - n_stat <= 1.0);
    return (n_stat + 1 - P*n) * left + (P*n - n_stat) * right;
  }
}
