// This is mul/pdf1d/pdf1d_gaussian_kernel_pdf_builder.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include <vnl/vnl_fwd.h> // Leave this hack here to avoid a MSVC internal compiler error.
#include "pdf1d_gaussian_kernel_pdf_builder.h"

#include <vcl_cassert.h>
#include <vcl_string.h>

#include <pdf1d/pdf1d_gaussian_kernel_pdf.h>

//=======================================================================
// Dflt ctor
//=======================================================================

pdf1d_gaussian_kernel_pdf_builder::pdf1d_gaussian_kernel_pdf_builder()
{
}

//=======================================================================
// Destructor
//=======================================================================

pdf1d_gaussian_kernel_pdf_builder::~pdf1d_gaussian_kernel_pdf_builder()
{
}

//=======================================================================

pdf1d_gaussian_kernel_pdf& pdf1d_gaussian_kernel_pdf_builder::gkpdf(pdf1d_pdf& model) const
{
  // require a pdf1d_gaussian_kernel_pdf
  assert(model.is_class("pdf1d_gaussian_kernel_pdf"));
  return static_cast<pdf1d_gaussian_kernel_pdf&>( model);
}

pdf1d_pdf* pdf1d_gaussian_kernel_pdf_builder::new_model() const
{
  return new pdf1d_gaussian_kernel_pdf;
}


vcl_string pdf1d_gaussian_kernel_pdf_builder::new_model_type() const
{
  return vcl_string("pdf1d_gaussian_kernel_pdf");
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string pdf1d_gaussian_kernel_pdf_builder::is_a() const
{
  return vcl_string("pdf1d_gaussian_kernel_pdf_builder");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_gaussian_kernel_pdf_builder::is_class(vcl_string const& s) const
{
  return pdf1d_kernel_pdf_builder::is_class(s) || s==pdf1d_gaussian_kernel_pdf_builder::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short pdf1d_gaussian_kernel_pdf_builder::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_builder* pdf1d_gaussian_kernel_pdf_builder::clone() const
{
  return new pdf1d_gaussian_kernel_pdf_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void pdf1d_gaussian_kernel_pdf_builder::print_summary(vcl_ostream& /*os*/) const
{
  vcl_cerr << "pdf1d_gaussian_kernel_pdf_builder::print_summary() NYI\n";
}


