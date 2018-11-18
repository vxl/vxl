// This is mul/vpdfl/vpdfl_gaussian_kernel_pdf_builder.cxx

#include <iostream>
#include <string>
#include "vpdfl_gaussian_kernel_pdf_builder.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpdfl/vpdfl_gaussian_kernel_pdf.h>

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_gaussian_kernel_pdf_builder::vpdfl_gaussian_kernel_pdf_builder() = default;

//=======================================================================
// Destructor
//=======================================================================

vpdfl_gaussian_kernel_pdf_builder::~vpdfl_gaussian_kernel_pdf_builder() = default;


vpdfl_pdf_base* vpdfl_gaussian_kernel_pdf_builder::new_model() const
{
  return new vpdfl_gaussian_kernel_pdf;
}


//=======================================================================
// Method: is_a
//=======================================================================

std::string vpdfl_gaussian_kernel_pdf_builder::is_a() const
{
  return std::string("vpdfl_gaussian_kernel_pdf_builder");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_gaussian_kernel_pdf_builder::is_class(std::string const& s) const
{
  return vpdfl_kernel_pdf_builder::is_class(s) || s==vpdfl_gaussian_kernel_pdf_builder::is_a();
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_builder_base* vpdfl_gaussian_kernel_pdf_builder::clone() const
{
  return new vpdfl_gaussian_kernel_pdf_builder(*this);
}
