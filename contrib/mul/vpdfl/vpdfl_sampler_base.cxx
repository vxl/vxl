#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file
// \author Ian Scott
// \date 19-Apr-2001
// \brief Base class for Multi-Variate Probability Sampler classes.

#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vpdfl/vpdfl_sampler_base.h>
#include <vpdfl/vpdfl_pdf_base.h>
#include <vsl/vsl_indent.h>

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_sampler_base::vpdfl_sampler_base()
{
}

//=======================================================================
// Destructor
//=======================================================================

vpdfl_sampler_base::~vpdfl_sampler_base()
{
}


//: Set model for which this is an instance
void vpdfl_sampler_base::set_model(const vpdfl_pdf_base& model)
{
  pdf_model_ = &model;
}

//: PDF of which this is an instance
const vpdfl_pdf_base& vpdfl_sampler_base::model() const
{
  assert (pdf_model_);
  return *pdf_model_;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string vpdfl_sampler_base::is_a() const
{
  return vcl_string("vpdfl_sampler_base");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_sampler_base::is_class(vcl_string const& s) const
{
  return s==vcl_string("vpdfl_sampler_base");
}

//=======================================================================
// Method: print
//=======================================================================

  // required if data is present in this base class
void vpdfl_sampler_base::print_summary(vcl_ostream& os) const
{
  os << vsl_indent() << "PDF: ";
  vsl_print_summary(os, model());
}


void vsl_print_summary(vcl_ostream& os,const vpdfl_sampler_base& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const vpdfl_sampler_base* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No vpdfl_sampler_base defined.";
}

