// This is mul/pdf1d/pdf1d_flat_builder.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "pdf1d_flat_builder.h"

#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h> // vcl_abort()

#include <mbl/mbl_data_wrapper.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <pdf1d/pdf1d_flat.h>

//=======================================================================
// Dflt ctor
//=======================================================================

pdf1d_flat_builder::pdf1d_flat_builder()
    : min_var_(1.0e-6)
{
}

//=======================================================================
// Destructor
//=======================================================================

pdf1d_flat_builder::~pdf1d_flat_builder()
{
}

//=======================================================================

pdf1d_flat& pdf1d_flat_builder::flat(pdf1d_pdf& model) const
{
  // require a pdf1d_flat
  assert(model.is_class("pdf1d_flat"));
  return static_cast<pdf1d_flat&>(model);
}

pdf1d_pdf* pdf1d_flat_builder::new_model() const
{
  return new pdf1d_flat;
}

vcl_string pdf1d_flat_builder::new_model_type() const
{
  return vcl_string("pdf1d_flat");
}

//=======================================================================
//: Define lower threshold on variance for built models
//=======================================================================
void pdf1d_flat_builder::set_min_var(double min_var)
{
  min_var_ = min_var;
}

//=======================================================================
//: Get lower threshold on variance for built models
//=======================================================================
double pdf1d_flat_builder::min_var() const
{
  return min_var_;
}

void pdf1d_flat_builder::build(pdf1d_pdf& model, double mean) const
{
  pdf1d_flat& f = flat(model);
  f.set(mean,min_var_);
}

//: Build flat from n elements in data[i]
void pdf1d_flat_builder::build_from_array(pdf1d_pdf& model, const double* data, int n) const
{
  if (n<2)
  {
    vcl_cerr<<"pdf1d_flat_builder::build_from_array(): too few examples available.\n";
    vcl_abort();
  }

  double lo = data[0];
  double hi = lo;
  for (int i=1;i<n;++i)
  {
    if (data[i]<lo) lo=data[i];
    else
      if (data[i]>hi) hi=data[i];
  }

  double min_w = vcl_sqrt(12*min_var_);
  if (hi-lo<min_w)
  {
    double c = 0.5*(lo+hi);
    lo = c-0.5*min_w;
    hi = c+0.5*min_w;
  }

  pdf1d_flat& f = flat(model);
  f.set(lo,hi);
}

void pdf1d_flat_builder::build(pdf1d_pdf& model, mbl_data_wrapper<double>& data) const
{
  int n_samples = data.size();

  if (n_samples<2)
  {
    vcl_cerr<<"pdf1d_flat_builder::build(): too few examples available.\n";
    vcl_abort();
  }

  if (data.is_class("mbl_data_array_wrapper<T>"))
  {
    // Use more efficient build_from_array algorithm
    mbl_data_array_wrapper<double>& data_array =
                       static_cast<mbl_data_array_wrapper<double>&>(data);
    build_from_array(model,data_array.data(),n_samples);
    return;
  }

  data.reset();
  double lo = data.current();
  double hi = lo;
  for (int i=0;i<n_samples;i++)
  {
    double x = data.current();
    if (x<lo) lo=x;
    else
      if (x>hi) hi=x;
    data.next();
  }

  double min_w = vcl_sqrt(12*min_var_);
  if (hi-lo<min_w)
  {
    double c = 0.5*(lo+hi);
    lo = c-0.5*min_w;
    hi = c+0.5*min_w;
  }

  pdf1d_flat& f = flat(model);
  f.set(lo,hi);
}

void pdf1d_flat_builder::weighted_build(pdf1d_pdf& model,
                                        mbl_data_wrapper<double>& data,
                                        const vcl_vector<double>& /*wts*/) const
{
  // TODO - Currently ignore weights
  build(model,data);
}
//=======================================================================
// Method: is_a
//=======================================================================

vcl_string pdf1d_flat_builder::is_a() const
{
  return vcl_string("pdf1d_flat_builder");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_flat_builder::is_class(vcl_string const& s) const
{
  return pdf1d_builder::is_class(s) || s==pdf1d_flat_builder::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short pdf1d_flat_builder::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_builder* pdf1d_flat_builder::clone() const
{
  return new pdf1d_flat_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void pdf1d_flat_builder::print_summary(vcl_ostream& os) const
{
  os << "Min. var.: "<< min_var_;
}

//=======================================================================
// Method: save
//=======================================================================

void pdf1d_flat_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,min_var_);
}

//=======================================================================
// Method: load
//=======================================================================

void pdf1d_flat_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,min_var_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_flat_builder &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

