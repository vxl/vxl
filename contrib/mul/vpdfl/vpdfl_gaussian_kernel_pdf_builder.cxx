// This is mul/vpdfl/vpdfl_gaussian_kernel_pdf_builder.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vpdfl_gaussian_kernel_pdf_builder.h"

#include <vcl_string.h>
#include <vcl_cstdlib.h> // vcl_abort()

#include <vpdfl/vpdfl_gaussian_kernel_pdf.h>

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_gaussian_kernel_pdf_builder::vpdfl_gaussian_kernel_pdf_builder()
{
}

//=======================================================================
// Destructor
//=======================================================================

vpdfl_gaussian_kernel_pdf_builder::~vpdfl_gaussian_kernel_pdf_builder()
{
}


vpdfl_pdf_base* vpdfl_gaussian_kernel_pdf_builder::new_model() const
{
  return new vpdfl_gaussian_kernel_pdf;
}


//=======================================================================
// Method: is_a
//=======================================================================

vcl_string vpdfl_gaussian_kernel_pdf_builder::is_a() const
{
  return vcl_string("vpdfl_gaussian_kernel_pdf_builder");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_gaussian_kernel_pdf_builder::is_class(vcl_string const& s) const
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


