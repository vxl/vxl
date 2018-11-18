// This is mul/vpdfl/vpdfl_builder_base.cxx
//:
// \file
// \author Tim Cootes
// \date 12-Apr-2001
// \brief Base for classes to build vpdfl_pdf_base objects.

#include "vpdfl_builder_base.h"

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

#include <vpdfl/vpdfl_axis_gaussian_builder.h>
#include <vpdfl/vpdfl_gaussian_kernel_pdf_builder.h>
#include <vpdfl/vpdfl_gaussian_builder.h>

#include <mbl/mbl_read_props.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_exception.h>

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

std::string vpdfl_builder_base::is_a() const
{
  return std::string("vpdfl_builder_base");
}

//=======================================================================

bool vpdfl_builder_base::is_class(std::string const& s) const
{
  return s==vpdfl_builder_base::is_a();
}

//: Create a vpdfl_builder_base object given a config stream
// \throw vcl_runtime_exception if parse error.
std::unique_ptr<vpdfl_builder_base> vpdfl_builder_base::new_builder_from_stream(std::istream &is)
{
  // This function should really be replaced by a general loader scheme
  // Ask Ian for examples from Manchester's private code base.

  //: This will store the constructed builder.
  std::unique_ptr<vpdfl_builder_base> builder;

  std::string type;
  is >> type;

  if (type == "vpdfl_axis_gaussian_builder")
  {
    builder = std::unique_ptr<vpdfl_builder_base>(new vpdfl_axis_gaussian_builder());
  }
  else if (type == "vpdfl_gaussian_kernel_pdf_builder")
  {
    builder = std::unique_ptr<vpdfl_builder_base>(new vpdfl_gaussian_kernel_pdf_builder());
  }
  else if (type == "vpdfl_gaussian_builder")
  {
    builder = std::unique_ptr<vpdfl_builder_base>(new vpdfl_gaussian_builder());
  }
  else
    mbl_exception_error(mbl_exception_no_name_in_factory(type,
      "vpdfl_axis_gaussian_builder, vpdfl_gaussian_kernel_pdf_builder, vpdfl_gaussian_builder"));

  //: Incoming properties
  mbl_read_props_type props(mbl_read_props(is));
  // which should be empty (until we add some relevant parameters)
  mbl_read_props_look_for_unused_props("vpdfl_builder_base::new_builder_from_stream", props, mbl_read_props_type());
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

void vsl_print_summary(std::ostream& os,const vpdfl_builder_base& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

void vsl_print_summary(std::ostream& os,const vpdfl_builder_base* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No vpdfl_builder_base defined.";
}

//=======================================================================

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const vpdfl_builder_base& b)
{
  vsl_print_summary(os,b);
  return os;
}

//=======================================================================

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const vpdfl_builder_base* b)
{
  vsl_print_summary(os,b);
  return os;
}

//=======================================================================
//: Create a vpdfl_builder_base object given a config stream (recursive style)
//  Creates object, then uses config_from_stream(is) to set up internals
std::unique_ptr<vpdfl_builder_base> vpdfl_builder_base::
  new_pdf_builder_from_stream(std::istream &is)
{
  std::string name;
  is >> name;
  std::unique_ptr<vpdfl_builder_base> builder;
  try
  {
    builder = mbl_cloneables_factory<vpdfl_builder_base>::get_clone(name);
  }
  catch (const mbl_exception_no_name_in_factory & e)
  {
    throw (mbl_exception_parse_error( e.what() ));
  }
  builder->config_from_stream(is);
  return builder;
}

//=======================================================================
//: Read initialisation settings from a stream.
// The default implementation merely checks that no properties have
// been specified.
void vpdfl_builder_base::config_from_stream(
  std::istream & is)
{
  std::string s = mbl_parse_block(is);
  if (s.empty() || s=="{}") return;

  throw mbl_exception_parse_error(
    this->is_a() + " expects no properties in initialisation,\n"
    "But the following properties were given:\n" + s);
}
