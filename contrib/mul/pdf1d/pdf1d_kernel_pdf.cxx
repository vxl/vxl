// This is mul/pdf1d/pdf1d_kernel_pdf.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Base class for kernel PDFs.
// \author Tim Cootes
// \verbatim
//  Modifications
//  IMS 28 Feb 2002 Added inverse CDF.
// \endverbatim

#include "pdf1d_kernel_pdf.h"
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vnl/vnl_math.h>
#include <vnl/io/vnl_io_vector.h>
#include <mbl/mbl_index_sort.h>
#include <pdf1d/pdf1d_calc_mean_var.h>

//=======================================================================

pdf1d_kernel_pdf::pdf1d_kernel_pdf()
{
}

//=======================================================================

pdf1d_kernel_pdf::~pdf1d_kernel_pdf()
{
}

//=======================================================================
//: Initialise so all kernels have the same width
void pdf1d_kernel_pdf::set_centres(const vnl_vector<double>& x, double width)
{
  x_ = x;
  width_.set_size(x.size());
  width_.fill(width);
  all_same_width_ = true;

  double m,v;
  pdf1d_calc_mean_var(m,v,x);
  set_mean(m);
  set_variance(v+width*width);
}

//=======================================================================
//: Initialise so all kernels have given width
void pdf1d_kernel_pdf::set_centres(const vnl_vector<double>& x,
                                   const vnl_vector<double>& width)
{
  assert(x.size()==width.size());
  x_ = x;
  width_= width;
  all_same_width_ = false;

  double m,v;
  pdf1d_calc_mean_var(m,v,x);
  double w_sd = width.rms();
  set_mean(m);
  set_variance(v+w_sd*w_sd);
  index_.clear();
}

//=======================================================================
//: The inverse cdf.
// The value of x: P(x'<x) = P for x' drawn from distribution pdf.
double pdf1d_kernel_pdf::inverse_cdf(double P) const
{
  assert (cdf_is_analytic()); // if it isn't then we would be better off using sampling
  assert (0.0 < P && P < 1.0);

  if (index_.empty())
    mbl_index_sort(x_.data_block(), x_.size(), index_);

  // Assume kernels are deltas to get the starting point.
  double x_init=x_(index_[int(P * x_.size())]);

  double f_init = cdf(x_init);

  // guess initial step size. assuming a triangular kernel.
  double step = width_(index_[int(P * x_.size())]);

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
  vcl_cerr << "ERROR: pdf1d_kernel_pdf::inverse_cdf() failed to converge.\n";
  vcl_abort();
  return 0.0; // dummy return
}

//=======================================================================

vcl_string pdf1d_kernel_pdf::is_a() const
{
  static vcl_string class_name_ = "pdf1d_kernel_pdf";
  return class_name_;
}

//=======================================================================

bool pdf1d_kernel_pdf::is_class(vcl_string const& s) const
{
  return pdf1d_pdf::is_class(s) || s==pdf1d_kernel_pdf::is_a();
}

//=======================================================================

short pdf1d_kernel_pdf::version_no() const
{
  return 1;
}

//=======================================================================

void pdf1d_kernel_pdf::print_summary(vcl_ostream& os) const
{
  pdf1d_pdf::print_summary(os);
  os << '\n';
}

//=======================================================================

void pdf1d_kernel_pdf::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  pdf1d_pdf::b_write(bfs);
  vsl_b_write(bfs,x_);
  vsl_b_write(bfs,width_);
}

//=======================================================================

void pdf1d_kernel_pdf::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  vcl_string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_kernel_pdf &)\n"
             << "           Attempted to load object of type "
             << name <<" into object of type " << is_a() << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      pdf1d_pdf::b_read(bfs);
      vsl_b_read(bfs,x_);
      vsl_b_read(bfs,width_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_kernel_pdf &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

