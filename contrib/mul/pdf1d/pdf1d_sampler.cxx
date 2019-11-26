// This is mul/pdf1d/pdf1d_sampler.cxx

//:
// \file
// \author Tim Cootes
// \date 19-Apr-2001
// \brief Base class for Univariate Probability Sampler classes.

#include "pdf1d_sampler.h"
#include <pdf1d/pdf1d_pdf.h>
#include "vsl/vsl_indent.h"
#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

//=======================================================================
// Dflt ctor
//=======================================================================

pdf1d_sampler::pdf1d_sampler() = default;

//=======================================================================
// Destructor
//=======================================================================

pdf1d_sampler::~pdf1d_sampler() = default;


//: Set model for which this is an instance
void pdf1d_sampler::set_model(const pdf1d_pdf& model)
{
  pdf_model_ = &model;
}

//: PDF of which this is an instance
const pdf1d_pdf& pdf1d_sampler::model() const
{
  assert (pdf_model_ != nullptr);
  return *pdf_model_;
}

//: Fill x with samples drawn from distribution
void pdf1d_sampler::get_samples(vnl_vector<double>& x)
{
  int n = x.size();
  double* x_data = x.data_block();
  for (int i=0;i<n;++i)
    x_data[i]=sample();
}

//: Fill x with samples possibly chosen so as to represent the distribution
//  By default simply randomly sample from distribution
void pdf1d_sampler::regular_samples(vnl_vector<double>& x)
{
  int n = x.size();
  double* x_data = x.data_block();
  for (int i=0;i<n;++i)
    x_data[i]=sample();
}

//: Fill x with samples possibly chosen so as to represent the distribution
//  By default simply randomly sample from distribution
void pdf1d_sampler::regular_samples_and_prob(vnl_vector<double>& x, vnl_vector<double>& p)
{
  regular_samples(x);
  int n = x.size();
  p.set_size(n);
  double* p_data = p.data_block();
  double* x_data = x.data_block();

  for (int i=0;i<n;++i)
    p_data[i] = pdf_model_->operator()(x_data[i]);
}


//=======================================================================
// Method: is_a
//=======================================================================

std::string pdf1d_sampler::is_a() const
{
  return std::string("pdf1d_sampler");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_sampler::is_class(std::string const& s) const
{
  return s==pdf1d_sampler::is_a();
}

//=======================================================================
// Method: print
//=======================================================================

  // required if data is present in this base class
void pdf1d_sampler::print_summary(std::ostream& os) const
{
  os << vsl_indent() << "PDF: ";
  vsl_print_summary(os, model());
}


void vsl_print_summary(std::ostream& os,const pdf1d_sampler& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

void vsl_print_summary(std::ostream& os,const pdf1d_sampler* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No pdf1d_sampler defined.";
}
