// This is mul/pdf1d/pdf1d_epanech_kernel_pdf.cxx

//:
// \file
// \brief Univariate Epanechnikov kernel PDF
// \author Tim Cootes

#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include "pdf1d_epanech_kernel_pdf.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <pdf1d/pdf1d_epanech_kernel_pdf_sampler.h>
#include <pdf1d/pdf1d_sampler.h>

//=======================================================================

pdf1d_epanech_kernel_pdf::pdf1d_epanech_kernel_pdf() = default;

//: Define n kernels centred at i*sep (i=0..n-1)
pdf1d_epanech_kernel_pdf::pdf1d_epanech_kernel_pdf(
                            int n, double sep, double width)
{
  vnl_vector<double> x(n);
  for (int i=0;i<n;++i) x[i]=i*sep;
  set_centres(x,width);
}

//=======================================================================

pdf1d_epanech_kernel_pdf::~pdf1d_epanech_kernel_pdf() = default;

//=======================================================================


pdf1d_sampler* pdf1d_epanech_kernel_pdf::new_sampler() const
{
  auto *i = new pdf1d_epanech_kernel_pdf_sampler;
  i->set_model(*this);
  return i;
}

static const double root5 = 2.23606797749978970; //std::sqrt(5);

//=======================================================================
//: Probability density at x
double pdf1d_epanech_kernel_pdf::operator()(double x0) const
{
  double p;
  const double* x = x_.data_block();
  const double* w = width_.data_block();
  int n = x_.size();
  double k = 0.75/(n*root5);
  double sum = 0;

  for (int i=0;i<n;++i)
  {
    double dx = (x[i]-x0)/w[i];
    double dx2=dx*dx;
    if (dx2<5) sum += (1.0-0.2*dx2)/w[i];
  }

  p = k*sum;

  return p;
}

  // Probability densities:
double pdf1d_epanech_kernel_pdf::log_p(double x) const
{
  return std::log(pdf1d_epanech_kernel_pdf::operator()(x));
}

//: Cumulative Probability (P(x'<x) for x' drawn from the distribution)
// CDF of $k(x) = 0.75x(1-x^2/15)/\sqrt{5} + 0.5$ if $x^2<5$
double pdf1d_epanech_kernel_pdf::cdf(double x0) const
{
  const double* x = x_.data_block();
  const double* w = width_.data_block();
  int n = x_.size();
  double k = 0.75/(root5);

  double sum = 0;
  for (int i=0;i<n;++i)
  {
    double dx = (x0-x[i])/w[i];
    if (dx>=root5) sum+=1;
    else if (dx > -root5)
    {
      const double dx2 = dx*dx;
      sum += (k*dx*(1-dx2/15)+0.5);
    }
  }

  return sum/n;
}

//: Return true if cdf() uses an analytic implementation
bool pdf1d_epanech_kernel_pdf::cdf_is_analytic() const
{
  return true;
}

//=======================================================================


double pdf1d_epanech_kernel_pdf::gradient(double x0,
                                          double& p) const
{
  const double* x = x_.data_block();
  const double* w = width_.data_block();
  int n = x_.size();
  double sum_p = 0;
  double sum_g = 0;

  for (int i=0;i<n;++i)
  {
    double wi = w[i];
    double dx = (x[i]-x0)/wi;
    double dx2 = dx*dx;
    if (dx2<5)
    {
      sum_p += (1.0-0.2*dx2)/wi;
      sum_g += dx/wi;
    }
  }

  double k = 1.0/(n*root5);
  p = sum_p*0.75*k;

  return -0.4*k*sum_g;
}

//=======================================================================

double pdf1d_epanech_kernel_pdf::nearest_plausible(double /*x*/, double /*log_p_min*/) const
{
  std::cerr<<"pdf1d_epanech_kernel_pdf::nearest_plausible() not yet implemented.\n";
  std::abort();
  return 0.0;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string pdf1d_epanech_kernel_pdf::is_a() const
{
  static std::string class_name_ = "pdf1d_epanech_kernel_pdf";
  return class_name_;
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_epanech_kernel_pdf::is_class(std::string const& s) const
{
  return pdf1d_kernel_pdf::is_class(s) || s==pdf1d_epanech_kernel_pdf::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short pdf1d_epanech_kernel_pdf::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_pdf* pdf1d_epanech_kernel_pdf::clone() const
{
  return new pdf1d_epanech_kernel_pdf(*this);
}

//=======================================================================
// Method: print
//=======================================================================


void pdf1d_epanech_kernel_pdf::print_summary(std::ostream& os) const
{
  pdf1d_pdf::print_summary(os);
  os << '\n';
}

//=======================================================================
// Method: save
//=======================================================================

void pdf1d_epanech_kernel_pdf::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  pdf1d_kernel_pdf::b_write(bfs);
}

//=======================================================================
// Method: load
//=======================================================================

void pdf1d_epanech_kernel_pdf::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  std::string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_epanech_kernel_pdf &)\n"
             << "           Attempted to load object of type "
             << name <<" into object of type " << is_a() << std::endl;
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
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_epanech_kernel_pdf &)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//==================< end of pdf1d_epanech_kernel_pdf_pdf.cxx >====================
