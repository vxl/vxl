// This is mul/pdf1d/pdf1d_exponential_builder.cxx

//:
// \file

#include <string>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "pdf1d_exponential_builder.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_data_wrapper.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <pdf1d/pdf1d_exponential.h>
#include <pdf1d/pdf1d_calc_mean_var.h>

//=======================================================================
// Dflt ctor
//=======================================================================

pdf1d_exponential_builder::pdf1d_exponential_builder()
    : min_var_(1.0e-6)
{
}

//=======================================================================
// Destructor
//=======================================================================

pdf1d_exponential_builder::~pdf1d_exponential_builder() = default;

//=======================================================================

pdf1d_exponential& pdf1d_exponential_builder::exponential(pdf1d_pdf& model) const
{
  // require a pdf1d_exponential
  assert(model.is_class("pdf1d_exponential"));
  return static_cast<pdf1d_exponential&>(model);
}

pdf1d_pdf* pdf1d_exponential_builder::new_model() const
{
  return new pdf1d_exponential;
}

std::string pdf1d_exponential_builder::new_model_type() const
{
  return std::string("pdf1d_exponential");
}


//=======================================================================
//: Define lower threshold on variance for built models
//=======================================================================
void pdf1d_exponential_builder::set_min_var(double min_var)
{
  min_var_ = min_var;
}

//=======================================================================
//: Get lower threshold on variance for built models
//=======================================================================
double pdf1d_exponential_builder::min_var() const
{
  return min_var_;
}

void pdf1d_exponential_builder::build(pdf1d_pdf& model, double mean) const
{
  pdf1d_exponential& g = exponential(model);
  g.set_lambda(1.0/mean);
}

//: Build exponential from n elements in data[i]
void pdf1d_exponential_builder::build_from_array(pdf1d_pdf& model, const double* data, int n) const
{
  if (n<2)
  {
    std::cerr<<"pdf1d_exponential_builder::build_from_array()"
            <<" Too few examples available.\n";
    std::abort();
  }

  double m,v;
  pdf1d_calc_mean_var(m,v,data,n);

  double min_m = std::sqrt(min_var_);
  if (m<min_m) m=min_m;

  pdf1d_exponential& g = exponential(model);
  g.set_lambda(1.0/m);
}

void pdf1d_exponential_builder::build(pdf1d_pdf& model, mbl_data_wrapper<double>& data) const
{
  int n_samples = data.size();

  if (n_samples<2)
  {
    std::cerr<<"pdf1d_exponential_builder::build() Too few examples available.\n";
    std::abort();
  }

  if (data.is_class("mbl_data_array_wrapper<T>"))
  {
    // Use more efficient build_from_array algorithm
    auto& data_array =
                     static_cast<mbl_data_array_wrapper<double>&>(data);
    build_from_array(model,data_array.data(),n_samples);
    return;
  }

  double sum = 0;

  data.reset();
  for (int i=0;i<n_samples;i++)
  {
    double x = data.current();
    sum += x;
    data.next();
  }

  double m = sum/n_samples;
  double min_m = std::sqrt(min_var_);
  if (m<min_m) m=min_m;

  pdf1d_exponential& g = exponential(model);
  g.set_lambda(1.0/m);
}

void pdf1d_exponential_builder::weighted_build(pdf1d_pdf& model,
                                               mbl_data_wrapper<double>& data,
                                               const std::vector<double>& wts) const
{
  int n_samples = data.size();

  if (n_samples<2)
  {
    std::cerr<<"pdf1d_exponential_builder::build() Too few examples available.\n";
    std::abort();
  }

  double sum = 0;
  double w_sum = 0;
  const double* w = &(wts.front()); // cannot use wts.begin() since that's an iterator, not a pointer


  // Inefficient to go through twice.
  // Fix this one day.

  data.reset();
  for (int i=0;i<n_samples;i++)
  {
    double x = data.current();
    double wi = w[i];
    sum += wi*x;
    w_sum += wi;
    data.next();
  }

  double m = sum/w_sum;
  double min_m = std::sqrt(min_var_);
  if (m<min_m) m=min_m;

  pdf1d_exponential& g = exponential(model);
  g.set_lambda(1.0/m);
}
//=======================================================================
// Method: is_a
//=======================================================================

std::string pdf1d_exponential_builder::is_a() const
{
  return std::string("pdf1d_exponential_builder");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_exponential_builder::is_class(std::string const& s) const
{
  return pdf1d_builder::is_class(s) || s==pdf1d_exponential_builder::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short pdf1d_exponential_builder::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_builder* pdf1d_exponential_builder::clone() const
{
  return new pdf1d_exponential_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void pdf1d_exponential_builder::print_summary(std::ostream& os) const
{
  os << "Min. var.: "<< min_var_;
}

//=======================================================================
// Method: save
//=======================================================================

void pdf1d_exponential_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,min_var_);
}

//=======================================================================
// Method: load
//=======================================================================

void pdf1d_exponential_builder::b_read(vsl_b_istream& bfs)
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
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_exponential_builder &)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
