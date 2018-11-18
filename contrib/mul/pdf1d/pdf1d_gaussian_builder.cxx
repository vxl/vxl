// This is mul/pdf1d/pdf1d_gaussian_builder.cxx
#include <string>
#include <iostream>
#include <cstdlib>
#include "pdf1d_gaussian_builder.h"
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_data_wrapper.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_calc_mean_var.h>

//=======================================================================
// Dflt ctor
//=======================================================================

pdf1d_gaussian_builder::pdf1d_gaussian_builder()
    : min_var_(1.0e-6)
{
}

//=======================================================================
// Destructor
//=======================================================================

pdf1d_gaussian_builder::~pdf1d_gaussian_builder() = default;

//=======================================================================

pdf1d_gaussian& pdf1d_gaussian_builder::gaussian(pdf1d_pdf& model) const
{
  // require a pdf1d_gaussian
  assert(model.is_class("pdf1d_gaussian"));
  return static_cast<pdf1d_gaussian&>(model);
}

pdf1d_pdf* pdf1d_gaussian_builder::new_model() const
{
  return new pdf1d_gaussian;
}

std::string pdf1d_gaussian_builder::new_model_type() const
{
  return std::string("pdf1d_gaussian");
}

//=======================================================================
//: Define lower threshold on variance for built models
//=======================================================================
void pdf1d_gaussian_builder::set_min_var(double min_var)
{
  min_var_ = min_var;
}

//=======================================================================
//: Get lower threshold on variance for built models
//=======================================================================
double pdf1d_gaussian_builder::min_var() const
{
  return min_var_;
}

void pdf1d_gaussian_builder::build(pdf1d_pdf& model, double mean) const
{
  pdf1d_gaussian& g = gaussian(model);
  g.set(mean,min_var_);
}

//: Build gaussian from n elements in data[i]
void pdf1d_gaussian_builder::build_from_array(pdf1d_pdf& model, const double* data, int n) const
{
  if (n<2)
  {
    std::cerr<<"pdf1d_gaussian_builder::build_from_array() Too few samples available.\n";
    std::abort();
  }

  double m,v;
  pdf1d_calc_mean_var(m,v,data,n);

  if (v<min_var_) v=min_var_;

  pdf1d_gaussian& g = gaussian(model);
  g.set(m,v);
}

void pdf1d_gaussian_builder::build(pdf1d_pdf& model, mbl_data_wrapper<double>& data) const
{
  pdf1d_gaussian& g = gaussian(model);

  int n_samples = data.size();

  if (n_samples<2)
  {
    std::cerr<<"pdf1d_gaussian_builder::build() Too few samples available.\n";
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
  double sum_sq = 0;

  data.reset();
  for (int i=0; i++<n_samples; data.next())
  {
    double x = data.current();
    sum += x;
    sum_sq += x*x;
  }

  double m = sum/n_samples;
  double v = (sum_sq - m*sum)/(n_samples-1);
  if (v<min_var_) v=min_var_;

  g.set(m,v);
}

void pdf1d_gaussian_builder::weighted_build(pdf1d_pdf& model,
                                            mbl_data_wrapper<double>& data,
                                            const std::vector<double>& wts) const
{
  pdf1d_gaussian& g = gaussian(model);

  int n_samples = data.size();

  if (n_samples<2)
  {
    std::cerr<<"pdf1d_gaussian_builder::build() Too few samples available.\n";
    std::abort();
  }

  double sum = 0;
  double w_sum = 0;
  double sum_sq = 0;
  const double* w = &(wts.front()); // cannot use wts.begin() since that's an iterator, not a pointer


  // Inefficient to go through twice.
  // Fix this one day.

  data.reset();
  for (int i=0;i<n_samples;++i,data.next())
  {
    double x = data.current();
    double wi = w[i];
    sum += wi*x;
    sum_sq += wi*x*x;
    w_sum += wi;
  }

  double m = sum/w_sum;
  double v = sum_sq/w_sum -m*m;
  v *= n_samples/double(n_samples-1);
  if (v<min_var_) v=min_var_;

  g.set(m,v);
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string pdf1d_gaussian_builder::is_a() const
{
  return std::string("pdf1d_gaussian_builder");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_gaussian_builder::is_class(std::string const& s) const
{
  return pdf1d_builder::is_class(s) || s==pdf1d_gaussian_builder::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short pdf1d_gaussian_builder::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_builder* pdf1d_gaussian_builder::clone() const
{
  return new pdf1d_gaussian_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void pdf1d_gaussian_builder::print_summary(std::ostream& os) const
{
  os << "Min. var.: "<< min_var_;
}

//=======================================================================
// Method: save
//=======================================================================

void pdf1d_gaussian_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,min_var_);
}

//=======================================================================
// Method: load
//=======================================================================

void pdf1d_gaussian_builder::b_read(vsl_b_istream& bfs)
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
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_gaussian_builder &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
