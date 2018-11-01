// This is mul/vpdfl/vpdfl_kernel_pdf.cxx
//:
// \file
// \brief Multi-variate kernel PDF
// \author Tim Cootes

#include <iostream>
#include <string>
#include "vpdfl_kernel_pdf.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_kernel_pdf::vpdfl_kernel_pdf() = default;

//=======================================================================
// Destructor
//=======================================================================

vpdfl_kernel_pdf::~vpdfl_kernel_pdf() = default;

//: Compute mean/variance given current centres and widths
void vpdfl_kernel_pdf::calc_mean_var()
{
  int n = x_.size();
  assert(n>0);
  int t = x_[0].size();
  vnl_vector<double> m(t),v2(t);
  m.fill(0); v2.fill(0);
  double* v2_data = v2.data_block();

  for (int i=0;i<n;++i)
  {
    m += x_[i];
    double* x_data = x_[i].data_block();
    for (int j=0;j<t;++j)
      v2_data[j]+= x_data[j]*x_data[j];
  }
  m/=n;

  double sd=width_.rms();
  double extra_var = sd*sd;
  for (int j=0;j<t;++j)
  {
    v2_data[j]/=n;
    v2_data[j]+= extra_var - m[j]*m[j];
  }


  set_mean(m);
  set_variance(v2);
}

//: Initialise so all kernels have the same width
void vpdfl_kernel_pdf::set_centres(const vnl_vector<double>* x, int n, double width)
{
  x_.resize(n);
  for (int i=0;i<n;++i) x_[i] = x[i];

  width_.set_size(n);
  width_.fill(width);
  all_same_width_ = true;

  calc_mean_var();
}

//: Initialise so all kernels have given width
void vpdfl_kernel_pdf::set_centres(const vnl_vector<double>* x, int n,
                                   const vnl_vector<double>& width)
{
  assert((unsigned int)n==width.size());
  x_.resize(n);
  for (int i=0;i<n;++i) x_[i] = x[i];
  width_= width;
  all_same_width_ = false;

  calc_mean_var();
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string vpdfl_kernel_pdf::is_a() const
{
  static const std::string s_ = "vpdfl_kernel_pdf";
  return s_;
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_kernel_pdf::is_class(std::string const& s) const
{
  return s==vpdfl_kernel_pdf::is_a() || vpdfl_pdf_base::is_class(s);
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_kernel_pdf::version_no() const
{
  return 1;
}

//=======================================================================
// Method: print
//=======================================================================

void vpdfl_kernel_pdf::print_summary(std::ostream& os) const
{
  vpdfl_pdf_base::print_summary(os);
}

//=======================================================================
// Method: save
//=======================================================================

void vpdfl_kernel_pdf::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vpdfl_pdf_base::b_write(bfs);
  vsl_b_write(bfs,x_);
  vsl_b_write(bfs,width_);
}

//=======================================================================
// Method: load
//=======================================================================

void vpdfl_kernel_pdf::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vpdfl_pdf_base::b_read(bfs);
      vsl_b_read(bfs,x_);
      vsl_b_read(bfs,width_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_kernel_pdf &)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
