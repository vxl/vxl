// This is mul/pdf1d/pdf1d_gaussian_kernel_pdf.cxx

//:
// \file
// \brief Univariate gaussian_kernel_pdf kernel PDF
// \author Tim Cootes

#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include "pdf1d_gaussian_kernel_pdf.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_math.h>
#include <vnl/vnl_erf.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf_sampler.h>
#include <pdf1d/pdf1d_sampler.h>
#include <mbl/mbl_index_sort.h>

//=======================================================================

pdf1d_gaussian_kernel_pdf::pdf1d_gaussian_kernel_pdf() = default;

//: Define n kernels centred at i*sep (i=0..n-1)
pdf1d_gaussian_kernel_pdf::pdf1d_gaussian_kernel_pdf(
                            int n, double sep, double width)
{
  vnl_vector<double> x(n);
  for (int i=0;i<n;++i) x[i]=i*sep;
  set_centres(x,width);
}

//=======================================================================

pdf1d_gaussian_kernel_pdf::~pdf1d_gaussian_kernel_pdf() = default;

//=======================================================================


pdf1d_sampler* pdf1d_gaussian_kernel_pdf::new_sampler() const
{
  auto *i = new pdf1d_gaussian_kernel_pdf_sampler;
  i->set_model(*this);
  return i;
}

//=======================================================================
//: Probability density at x
double pdf1d_gaussian_kernel_pdf::operator()(double x0) const
{
  double p;
  const double* x = x_.data_block();
  const double* w = width_.data_block();
  int n = x_.size();
  double k = vnl_math::one_over_sqrt2pi/n;
  double sum = 0;

  for (int i=0;i<n;++i)
  {
    double dx = (x[i]-x0)/w[i];
    sum += std::exp(-0.5*dx*dx)/w[i];
  }

  p = k*sum;

  return p;
}

  // Probability densities:
double pdf1d_gaussian_kernel_pdf::log_p(double x) const
{
  return std::log(pdf1d_gaussian_kernel_pdf::operator()(x));
}

//: Cumulative Probability (P(x'<x) for x' drawn from the distribution)
double pdf1d_gaussian_kernel_pdf::cdf(double x0) const
{
  const double* x = x_.data_block();
  const double* w = width_.data_block();
  int n = x_.size();
  double sum = 0;
#if 1

  for (int i=0;i<n;++i)
    sum += vnl_erfc( (x[i]-x0)/(vnl_math::sqrt2*w[i]) );
    // CDF for gaussian = 0.5*(erfc(-x/sqrt2))

  return 0.5*(sum/n);

#else // a slower but clearer and easier to debug version of above

  if (index_.empty())
    mbl_index_sort(x_.data_block(), x_.size(), index_);
  for (int i=0;i<n;++i)
    sum += 0.5 * (vnl_erfc( -(x0-x[index_[i]])/(vnl_math::sqrt2*w[index_[i]]) ) )/n;

  return sum;
#endif
}

//: Return true if cdf() uses an analytic implementation
bool pdf1d_gaussian_kernel_pdf::cdf_is_analytic() const
{
  return true;
}

//=======================================================================


double pdf1d_gaussian_kernel_pdf::gradient(double x0,
                                           double& p) const
{
  const double* x = x_.data_block();
  const double* w = width_.data_block();
  int n = x_.size();
  double k = vnl_math::one_over_sqrt2pi/n;
  double sum_p = 0;
  double sum_g = 0;

  for (int i=0;i<n;++i)
  {
    double wi = w[i];
    double dx = (x[i]-x0)/wi;
    double p_i = std::exp(-0.5*dx*dx)/wi;
    sum_p += p_i;
    sum_g -= p_i*dx/wi;
  }

  p = k*sum_p;

  return k*sum_g;
}

//=======================================================================

double pdf1d_gaussian_kernel_pdf::nearest_plausible(double /*x*/, double /*log_p_min*/) const
{
  std::cerr<<"pdf1d_gaussian_kernel_pdf::nearest_plausible() not yet implemented.\n";
  std::abort();
  return 0.0;
}

//=======================================================================

std::string pdf1d_gaussian_kernel_pdf::is_a() const
{
  static std::string class_name_ = "pdf1d_gaussian_kernel_pdf";
  return class_name_;
}

//=======================================================================

bool pdf1d_gaussian_kernel_pdf::is_class(std::string const& s) const
{
  return pdf1d_kernel_pdf::is_class(s) || s==pdf1d_gaussian_kernel_pdf::is_a();
}

//=======================================================================

short pdf1d_gaussian_kernel_pdf::version_no() const
{
  return 1;
}

//=======================================================================

pdf1d_pdf* pdf1d_gaussian_kernel_pdf::clone() const
{
  return new pdf1d_gaussian_kernel_pdf(*this);
}

//=======================================================================

void pdf1d_gaussian_kernel_pdf::print_summary(std::ostream& os) const
{
  pdf1d_pdf::print_summary(os);
  os << '\n';
}

//=======================================================================

void pdf1d_gaussian_kernel_pdf::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  pdf1d_kernel_pdf::b_write(bfs);
}

//=======================================================================

void pdf1d_gaussian_kernel_pdf::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  std::string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_gaussian_kernel_pdf &)\n"
             << "           Attempted to load object of type "
             << name <<" into object of type " << is_a() << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      pdf1d_kernel_pdf::b_read(bfs);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_gaussian_kernel_pdf &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
