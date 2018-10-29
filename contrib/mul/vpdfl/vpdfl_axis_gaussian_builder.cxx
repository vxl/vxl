// This is mul/vpdfl/vpdfl_axis_gaussian_builder.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "vpdfl_axis_gaussian_builder.h"
//
#include <cassert>
#include <vcl_compiler.h>

#include <mbl/mbl_data_wrapper.h>
#include <vpdfl/vpdfl_axis_gaussian.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <vul/vul_string.h>
#include <mbl/mbl_exception.h>

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_axis_gaussian_builder::vpdfl_axis_gaussian_builder()
    : min_var_(1.0e-6)
{
}

//=======================================================================
// Destructor
//=======================================================================

vpdfl_axis_gaussian_builder::~vpdfl_axis_gaussian_builder() = default;

//=======================================================================

vpdfl_axis_gaussian& vpdfl_axis_gaussian_builder::gaussian(vpdfl_pdf_base& model) const
{
  // require a vpdfl_axis_gaussian
  assert(model.is_class("vpdfl_axis_gaussian"));
  return static_cast<vpdfl_axis_gaussian&>(model);
}

vpdfl_pdf_base* vpdfl_axis_gaussian_builder::new_model() const
{
  return new vpdfl_axis_gaussian;
}

//=======================================================================
//: Define lower threshold on variance for built models
//=======================================================================
void vpdfl_axis_gaussian_builder::set_min_var(double min_var)
{
  min_var_ = min_var;
}

//=======================================================================
//: Get lower threshold on variance for built models
//=======================================================================
double vpdfl_axis_gaussian_builder::min_var() const
{
  return min_var_;
}

void vpdfl_axis_gaussian_builder::build(vpdfl_pdf_base& model,
                                        const vnl_vector<double>& mean) const
{
  vpdfl_axis_gaussian& g = gaussian(model);

  vnl_vector<double> var(mean.size());
  for (unsigned int i=0;i<mean.size();i++)
    var(i)=min_var_;

  g.set(mean,var);
}

void vpdfl_axis_gaussian_builder::build(vpdfl_pdf_base& model,
                                        mbl_data_wrapper<vnl_vector<double> >& data) const
{
  vpdfl_axis_gaussian& g = gaussian(model);

  unsigned long n_samples = data.size();

  if (n_samples<1L)
  {
    std::cerr<<"vpdfl_axis_gaussian_builder::build() Too few examples available.\n";
    std::abort();
  }

  data.reset();
  if (n_samples==1)
  {
    // Build using the single example as mean
    build(model,data.current());
    return;
  }

  unsigned long n_dims = data.current().size();
  vnl_vector<double> sum(n_dims),sum_sq(n_dims),var(n_dims);

  double* var_data = var.data_block();
  double* sum_data = sum.data_block();
  double* sum_sq_data = sum_sq.data_block();
  for (unsigned long j=0;j<n_dims;j++)
  {
    sum_data[j]=0.0;
    sum_sq_data[j]=0.0;
  }

  data.reset();
  for (unsigned long i=0;i<n_samples;i++)
  {
    const double *v = data.current().data_block();
    for (unsigned long j=0;j<n_dims;j++)
    {
      sum_data[j] += v[j];
      sum_sq_data[j] += v[j]*v[j];
    }

    data.next();
  }

  sum/=n_samples;
  for (unsigned long j=0;j<n_dims;j++)
  {
    var_data[j] = sum_sq_data[j]/n_samples - sum_data[j]*sum_data[j];
    if (var_data[j]<min_var_) var_data[j]=min_var_;
  }

  g.set(sum,var);
}

void vpdfl_axis_gaussian_builder::weighted_build(vpdfl_pdf_base& model,
                                                 mbl_data_wrapper<vnl_vector<double> >& data,
                                                 const std::vector<double>& wts) const
{
  vpdfl_axis_gaussian& g = gaussian(model);

  unsigned long n_samples = data.size();

  if (n_samples<2L)
  {
    std::cerr<<"vpdfl_axis_gaussian_builder::build() Too few examples available.\n";
    std::abort();
  }

  if (wts.size()!=n_samples)
  {
    std::cerr<<"vpdfl_axis_gaussian_builder::build() Weight array must contain "
            <<n_samples<<" not "<<wts.size()<<std::endl;
    std::abort();
  }
  data.reset();
  unsigned long n_dims = data.current().size();

  double w_sum = wts[0];
  vnl_vector<double> sum = data.current(); sum *= w_sum;

  for (unsigned long i=1;i<n_samples;i++)
  {
    data.next();
    double wt = wts[i];
    sum += wt * data.current();
    w_sum += wts[i];
  }

  vnl_vector<double> mean = sum/w_sum;


  vnl_vector<double> sum_sq(n_dims), var(n_dims);
  double* var_data = var.data_block();
  double* m_data = mean.data_block();
  double* sum_sq_data = sum_sq.data_block();
  for (unsigned long j=0;j<n_dims;j++)
  {
    sum_sq_data[j]=0.0;
  }

  data.reset();
  for (unsigned long i=0;i<n_samples;i++)
  {
    const double *v = data.current().data_block();
    double w = wts[i];
    for (unsigned long j=0;j<n_dims;j++)
    {
      double dx = v[j]-m_data[j];
      sum_sq_data[j] += w * dx*dx;
    }

    data.next();
  }

  for (unsigned long j=0;j<n_dims;j++)
  {
    var_data[j] = sum_sq_data[j]/w_sum;
    if (var_data[j]<min_var_) var_data[j]=min_var_;
  }

  g.set(mean,var);
}
//=======================================================================
// Method: is_a
//=======================================================================

std::string vpdfl_axis_gaussian_builder::is_a() const
{
  return std::string("vpdfl_axis_gaussian_builder");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_axis_gaussian_builder::is_class(std::string const& s) const
{
  return vpdfl_builder_base::is_class(s) || s==vpdfl_axis_gaussian_builder::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_axis_gaussian_builder::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_builder_base* vpdfl_axis_gaussian_builder::clone() const
{
  return new vpdfl_axis_gaussian_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void vpdfl_axis_gaussian_builder::print_summary(std::ostream& os) const
{
  os << "Min. var.: "<< min_var_;
}

//=======================================================================
// Method: save
//=======================================================================

void vpdfl_axis_gaussian_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,min_var_);
}

//=======================================================================
// Method: load
//=======================================================================

void vpdfl_axis_gaussian_builder::b_read(vsl_b_istream& bfs)
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
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_axis_gaussian_builder &)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Read initialisation settings from a stream.
// Parameters:
// \verbatim
// {
//   min_var: 1.0e-6
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.
void vpdfl_axis_gaussian_builder::config_from_stream(std::istream & is)
{
  std::string s = mbl_parse_block(is);

  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  double mv=1.0e-6;

  if (props.find("min_var")!=props.end())
  {
    mv=vul_string_atof(props["min_var"]);
    props.erase("min_var");
  }
  set_min_var(mv);

  try
  {
    mbl_read_props_look_for_unused_props(
        "vpdfl_axis_gaussian_builder::config_from_stream", props);
  }
  catch(mbl_exception_unused_props &e)
  {
    throw mbl_exception_parse_error(e.what());
  }
}

