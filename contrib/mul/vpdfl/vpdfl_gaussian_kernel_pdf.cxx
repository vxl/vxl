// This is mul/vpdfl/vpdfl_gaussian_kernel_pdf.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Multi-variate spherical gaussian_kernel_pdf kernel PDF.
// \author Tim Cootes

#include "vpdfl_gaussian_kernel_pdf.h"

#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_cmath.h>

#include <vnl/vnl_math.h>
#include <vpdfl/vpdfl_gaussian_kernel_pdf_sampler.h>
#include <vpdfl/vpdfl_sampler_base.h>

//=======================================================================

vpdfl_gaussian_kernel_pdf::vpdfl_gaussian_kernel_pdf()
{
}

//=======================================================================

vpdfl_gaussian_kernel_pdf::~vpdfl_gaussian_kernel_pdf()
{
}


//=======================================================================
//: Probability density at x
double vpdfl_gaussian_kernel_pdf::operator()(const vnl_vector<double>& x0) const
{
  int n = x_.size();
  assert(n>0);
  int dim = x_[0].size();
  double p;
  const vnl_vector<double>* x = &x_[0];
  const double* w = width_.data_block();
  double k = 1.0/(n*vcl_pow(2*vnl_math::pi,0.5*dim));
  double sum = 0;

  for (int i=0;i<n;++i)
  {
    double M = vnl_vector_ssd(x[i],x0)/(w[i]*w[i]);
    if (M<20)
      sum += vcl_exp(-0.5*M)/vcl_pow(w[i],dim);
  }

  p = k*sum;

  return p;
}

  // Probability densities:
double vpdfl_gaussian_kernel_pdf::log_p(const vnl_vector<double>& x) const
{
  return vcl_log(vpdfl_gaussian_kernel_pdf::operator()(x));
}

//=======================================================================


vpdfl_sampler_base* vpdfl_gaussian_kernel_pdf::new_sampler() const
{
  vpdfl_gaussian_kernel_pdf_sampler *i = new vpdfl_gaussian_kernel_pdf_sampler;
  i->set_model(*this);
  return i;
}

//=======================================================================


void vpdfl_gaussian_kernel_pdf::gradient(vnl_vector<double>& /*g*/,
                                         vnl_vector<double>const& /*x*/,
                                         double& /*p*/) const
{
  vcl_cerr<<"vpdfl_gaussian_kernel_pdf::gradient() Not yet implemented.\n";
  vcl_abort();
}

//=======================================================================

void vpdfl_gaussian_kernel_pdf::nearest_plausible(vnl_vector<double>& /*x*/, double /*log_p_min*/) const
{
  vcl_cerr<<"vpdfl_gaussian_kernel_pdf::nearest_plausible() Not yet implemented.\n";
  vcl_abort();
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string vpdfl_gaussian_kernel_pdf::is_a() const
{
  static vcl_string class_name_ = "vpdfl_gaussian_kernel_pdf";
  return class_name_;
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_gaussian_kernel_pdf::is_class(vcl_string const& s) const
{
  return vpdfl_kernel_pdf::is_class(s) || s==vpdfl_gaussian_kernel_pdf::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_gaussian_kernel_pdf::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_pdf_base* vpdfl_gaussian_kernel_pdf::clone() const
{
  return new vpdfl_gaussian_kernel_pdf(*this);
}

//=======================================================================
// Method: print
//=======================================================================


void vpdfl_gaussian_kernel_pdf::print_summary(vcl_ostream& os) const
{
  vpdfl_kernel_pdf::print_summary(os);
}

//=======================================================================
// Method: save
//=======================================================================

void vpdfl_gaussian_kernel_pdf::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  vpdfl_kernel_pdf::b_write(bfs);
}

//=======================================================================
// Method: load
//=======================================================================

void vpdfl_gaussian_kernel_pdf::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  vcl_string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_gaussian_kernel_pdf &)\n"
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
      vpdfl_kernel_pdf::b_read(bfs);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_gaussian_kernel_pdf &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//==================< end of vpdfl_gaussian_kernel_pdf.cxx >====================
