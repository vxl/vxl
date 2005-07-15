// This is mul/vpdfl/vpdfl_builder_base.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Tim Cootes
// \date 12-Apr-2001
// \brief Base for classes to build vpdfl_pdf_base objects.


#include "vpdfl_builder_base.h"
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

#include <mbl/mbl_exception.h>
#include <mbl/mbl_read_props.h>
#include <vpdfl/vpdfl_axis_gaussian_builder.h>
#include <vpdfl/vpdfl_gaussian_kernel_pdf_builder.h>
#include <vpdfl/vpdfl_gaussian_builder.h>
//=======================================================================

short vpdfl_builder_base::version_no() const
{
  return 1;
}

//=======================================================================

void vsl_add_to_binary_loader(const vpdfl_builder_base& b)
{
  vsl_binary_loader<vpdfl_builder_base>::instance().add(b);
}

//=======================================================================

vcl_string vpdfl_builder_base::is_a() const
{
  return vcl_string("vpdfl_builder_base");
}

//=======================================================================

bool vpdfl_builder_base::is_class(vcl_string const& s) const
{
  return s==vpdfl_builder_base::is_a();
}

//: Create a vpdfl_builder_base object given a config steram
// \throw vcl_runtime_exception if parse error.
vcl_auto_ptr<vpdfl_builder_base> vpdfl_builder_base::new_builder_from_stream(vcl_istream &is)
{
  // This function should really be replaced by a general loader scheme
  // Ask Ian for examples from Manchester's private code base.

  //: This will store the constructed limiter.
  vcl_auto_ptr<vpdfl_builder_base> builder;

  vcl_string type;
  is >> type;

  if (type == "vpdfl_axis_gaussian_builder")
  {
    builder = vcl_auto_ptr<vpdfl_builder_base>(new vpdfl_axis_gaussian_builder());
  }
  else if (type == "vpdfl_gaussian_kernel_pdf_builder")
  {
    builder = vcl_auto_ptr<vpdfl_builder_base>(new vpdfl_gaussian_kernel_pdf_builder());
  }
  else if (type == "vpdfl_gaussian_builder")
  {
    builder = vcl_auto_ptr<vpdfl_builder_base>(new vpdfl_gaussian_builder());
  }
  else
    mbl_exception_error(mbl_exception_no_name_in_factory(type,
      "vpdfl_axis_gaussian_builder, vpdfl_gaussian_kernel_pdf_builder, vpdfl_gaussian_builder"));

  //: Incoming properties
  mbl_read_props_type props(mbl_read_props(is));
  // which should be empty (until we add some relevent parameters)
  mbl_exception_look_for_unused_props("vpdfl_builder_base::new_builder_from_stream", props, mbl_read_props_type());
  return builder;

}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const vpdfl_builder_base& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, vpdfl_builder_base& b)
{
  b.b_read(bfs);
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const vpdfl_builder_base& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const vpdfl_builder_base* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No vpdfl_builder_base defined.";
}

//=======================================================================

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const vpdfl_builder_base& b)
{
  vsl_print_summary(os,b);
  return os;
}

//=======================================================================

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const vpdfl_builder_base* b)
{
  vsl_print_summary(os,b);
  return os;
}
