#ifdef __GNUC__
#pragma implementation
#endif

#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h> // vcl_abort()

#include <vsl/vsl_indent.h>
#include <mbl/mbl_data_wrapper.h>
#include <vpdfl/vpdfl_gaussian_kernel_pdf.h>
#include <vpdfl/vpdfl_gaussian_kernel_pdf_builder.h>

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


