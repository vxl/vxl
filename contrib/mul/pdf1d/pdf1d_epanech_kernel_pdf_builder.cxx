// This is mul/pdf1d/pdf1d_epanech_kernel_pdf_builder.cxx
#include <iostream>
#include <string>
#include "pdf1d_epanech_kernel_pdf_builder.h"
//:
// \file
// \author Tim Cootes
// \brief Builds Epanechnikov kernel pdfs

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <pdf1d/pdf1d_epanech_kernel_pdf.h>

//=======================================================================
// Dflt ctor
//=======================================================================

pdf1d_epanech_kernel_pdf_builder::pdf1d_epanech_kernel_pdf_builder() = default;

//=======================================================================
// Destructor
//=======================================================================

pdf1d_epanech_kernel_pdf_builder::~pdf1d_epanech_kernel_pdf_builder() = default;

//=======================================================================

pdf1d_epanech_kernel_pdf& pdf1d_epanech_kernel_pdf_builder::gkpdf(pdf1d_pdf& model) const
{
  // require a pdf1d_epanech_kernel_pdf
  assert(model.is_class("pdf1d_epanech_kernel_pdf"));
  return static_cast<pdf1d_epanech_kernel_pdf&>( model);
}

pdf1d_pdf* pdf1d_epanech_kernel_pdf_builder::new_model() const
{
  return new pdf1d_epanech_kernel_pdf;
}


std::string pdf1d_epanech_kernel_pdf_builder::new_model_type() const
{
  return std::string("pdf1d_epanech_kernel_pdf");
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string pdf1d_epanech_kernel_pdf_builder::is_a() const
{
  return std::string("pdf1d_epanech_kernel_pdf_builder");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_epanech_kernel_pdf_builder::is_class(std::string const& s) const
{
  return pdf1d_kernel_pdf_builder::is_class(s) || s==pdf1d_epanech_kernel_pdf_builder::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short pdf1d_epanech_kernel_pdf_builder::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_builder* pdf1d_epanech_kernel_pdf_builder::clone() const
{
  return new pdf1d_epanech_kernel_pdf_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void pdf1d_epanech_kernel_pdf_builder::print_summary(std::ostream& /*os*/) const
{
  std::cerr << "pdf1d_epanech_kernel_pdf_builder::print_summary() NYI\n";
}
