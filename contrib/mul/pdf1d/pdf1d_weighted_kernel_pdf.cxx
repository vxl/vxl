// This is mul/pdf1d/pdf1d_weighted_kernel_pdf.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief ...
// \author Ian Scott
// \date   Tue Apr  9 14:00:27 2002


#include "pdf1d_weighted_kernel_pdf.h"
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vsl/vsl_indent.h>
#include <vnl/vnl_math.h>
#include <vnl/io/vnl_io_vector.h>
#include <vsl/vsl_binary_loader.h>


//:calc the weighted mean and var of kernels.
// w is expected to sum to n.
void pdf1d_weighted_kernel_pdf::pdf1d_weighted_kernel_mean_var(double& mean, double& var,
                                                               const vnl_vector<double>& centres,
                                                               const vnl_vector<double>& widths,
                                                               const vnl_vector<double>& weights)
{
  const unsigned n = centres.size();
  assert(n > 1 && widths.size() == n && weights.size() ==n);

  double sum=0;
  double sum2 = 0;
  double sum3 = 0;
  double sum_weights = 0.0;
  for (unsigned i=0;i<n;++i)
  {
    sum+=weights(i) * centres(i);
    sum2+=weights(i) * vnl_math_sqr(centres(i));
    sum3+=weights(i) * vnl_math_sqr(widths(i));
    sum_weights += weights(i);
  }

  mean = sum/sum_weights;
  //variance = weighted variance of centres + weighted mean square of widths
  var  = (sum2 - n*mean*mean)/(n-1) + sum3/sum_weights;
}

  //: Initialise so all kernels have the same width
void pdf1d_weighted_kernel_pdf::set_centres(const vnl_vector<double>& x, double width)
{
  pdf1d_kernel_pdf::set_centres(x, width);
  weight_.set_size(x.size());
  weight_.fill(1.0);
  sum_weights_ = x.size();
}

  //: Initialise so all kernels have given width
void pdf1d_weighted_kernel_pdf::set_centres(const vnl_vector<double>& x,
                                            const vnl_vector<double>& width)
{
  pdf1d_kernel_pdf::set_centres(x, width);
  weight_.set_size(x.size());
  weight_.fill(1.0);
  sum_weights_ = x.size();
}


//=======================================================================

  //: Set the weights
void pdf1d_weighted_kernel_pdf::set_weight(const vnl_vector<double>& weights)
{
  const unsigned n  = x_.size();
  assert(weights.size() == n);

  weight_ = weights;
  sum_weights_ = vnl_c_vector<double>::sum(weight_.data_block(), weight_.size());

  double m,v;
  pdf1d_weighted_kernel_mean_var(m,v,x_, width_, weight_);
  set_mean(m);
  set_variance(v);
}

//=======================================================================

  //: Swap in the weights values.
  // This function is fast when you are changing the weights regularly
  // The weights will be scaled to sum to n
void pdf1d_weighted_kernel_pdf::swap_weight(vnl_vector<double>& weights)
{
  const unsigned n  = x_.size();
  assert(weights.size() == n);

  swap(weights, weight_);
  sum_weights_ = vnl_c_vector<double>::sum(weight_.data_block(), weight_.size());
}

//=======================================================================

short pdf1d_weighted_kernel_pdf::version_no() const
{
  return 1;
}

//=======================================================================

bool pdf1d_weighted_kernel_pdf::is_class(vcl_string const& s) const
{
  return s == pdf1d_weighted_kernel_pdf::is_a() || pdf1d_kernel_pdf::is_class(s);
}

//=======================================================================

vcl_string pdf1d_weighted_kernel_pdf::is_a() const
{
  return vcl_string("pdf1d_weighted_kernel_pdf");
}


//=======================================================================

    // required if data is present in this base class
void pdf1d_weighted_kernel_pdf::print_summary(vcl_ostream& os) const
{
  pdf1d_kernel_pdf::print_summary(os);
  os << vsl_indent() << "Weights: "; vsl_print_summary(os, weight_) ; os << '\n';
}

//=======================================================================

  // required if data is present in this base class
void pdf1d_weighted_kernel_pdf::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no());
  pdf1d_kernel_pdf::b_write(bfs);
  vsl_b_write(bfs, weight_);
}

//=======================================================================

  // required if data is present in this base class
void pdf1d_weighted_kernel_pdf::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    pdf1d_kernel_pdf::b_read(bfs);
    vsl_b_read(bfs,weight_);
    sum_weights_ = vnl_c_vector<double>::sum(weight_.data_block(), weight_.size());

    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_weighted_kernel_pdf&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
