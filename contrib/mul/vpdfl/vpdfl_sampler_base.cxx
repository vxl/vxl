// This is mul/vpdfl/vpdfl_sampler_base.cxx
//:
// \file
// \author Ian Scott
// \date 19-Apr-2001
// \brief Base class for Multi-Variate Probability Sampler classes.

#include "vpdfl_sampler_base.h"
#include <vpdfl/vpdfl_pdf_base.h>
#include <vsl/vsl_indent.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_sampler_base::vpdfl_sampler_base() = default;

//=======================================================================
// Destructor
//=======================================================================

vpdfl_sampler_base::~vpdfl_sampler_base() = default;


//: Set model for which this is an instance
void vpdfl_sampler_base::set_model(const vpdfl_pdf_base& model)
{
  pdf_model_ = &model;
}

//: PDF of which this is an instance
const vpdfl_pdf_base& vpdfl_sampler_base::model() const
{
  assert (pdf_model_ != nullptr);
  return *pdf_model_;
}


//: Fill x with samples drawn from distribution
void vpdfl_sampler_base::get_samples(std::vector<vnl_vector<double> >& x)
{
  int n = x.size();
  vnl_vector<double>* x_data = &x[0];
  for (int i=0;i<n;++i)
    sample(x_data[i]);
}

//: Fill x with samples possibly chosen so as to represent the distribution
void vpdfl_sampler_base::regular_samples(std::vector<vnl_vector<double> >& x)
{
  int n = x.size();
  vnl_vector<double>* x_data = &x[0];
  for (int i=0;i<n;++i)
    sample(x_data[i]);
}

//: Fill x with samples possibly chosen so as to represent the distribution
//  As regular_samples(x), but p[i] is set to p(x[i])
void vpdfl_sampler_base::regular_samples_and_prob(
                              std::vector<vnl_vector<double> >& x,
                              vnl_vector<double>& p)
{
  regular_samples(x);
  int n = x.size();
  p.set_size(n);
  double* p_data = p.data_block();
  vnl_vector<double>* x_data = &x[0];

  for (int i=0;i<n;++i)
    p_data[i] = pdf_model_->operator()(x_data[i]);
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string vpdfl_sampler_base::is_a() const
{
  return std::string("vpdfl_sampler_base");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_sampler_base::is_class(std::string const& s) const
{
  return s==vpdfl_sampler_base::is_a();
}

//=======================================================================
// Method: print
//=======================================================================

  // required if data is present in this base class
void vpdfl_sampler_base::print_summary(std::ostream& os) const
{
  os << vsl_indent() << "PDF: ";
  vsl_print_summary(os, model());
}


void vsl_print_summary(std::ostream& os,const vpdfl_sampler_base& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

void vsl_print_summary(std::ostream& os,const vpdfl_sampler_base* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No vpdfl_sampler_base defined.";
}
