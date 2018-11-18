// This is mul/clsfy/clsfy_builder_base.cxx
// Copyright: (C) 2000 British Telecommunications plc
#include "clsfy_builder_base.h"
//:
// \file
// \brief Implement bits of an abstract classifier builder
// \author Ian Scott
// \date 2000-05-10
// \verbatim
//  Modifications
//   2 May 2001 IMS Converted to VXL
// \endverbatim


//=======================================================================

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mbl/mbl_read_props.h>



//: Initialise the parameters from a text stream.
// Default case accepts no parameters.
void clsfy_builder_base::config(std::istream &as)
{
  mbl_read_props_type props = mbl_read_props_ws(as);

  // Check there are no unused properties
  mbl_read_props_look_for_unused_props("clsfy_builder_base::config",
                                       props, mbl_read_props_type());
}

//=======================================================================
//: Load description from a text stream
// The stream should contain the name of the feature extractor
// class that will be used, followed by a brace-enclosed list of
// parameters for the builder. This function will construct
// the appropriate clsfy_builder_base derivative and return that.
// \throws if the parse fails.
std::unique_ptr<clsfy_builder_base> clsfy_builder_base::new_builder(
  std::istream &as)
{
  std::string name;
  as >> name;

  std::unique_ptr<clsfy_builder_base> ps;
  try
  {
    ps = mbl_cloneables_factory<clsfy_builder_base>::get_clone(name);
  }
  catch (const mbl_exception_no_name_in_factory & e)
  {
    throw (mbl_exception_parse_error( e.what() ));
  }

  ps->config(as);

  return ps;
}


//=======================================================================

void vsl_add_to_binary_loader(const clsfy_builder_base& b)
{
  vsl_binary_loader<clsfy_builder_base>::instance().add(b);
}

//=======================================================================

std::string clsfy_builder_base::is_a() const
{
  return std::string("clsfy_builder_base");
}

//=======================================================================

bool clsfy_builder_base::is_class(std::string const& s) const
{
  return s == clsfy_builder_base::is_a();
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& os, const clsfy_builder_base& b)
{
  b.b_write(os);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, clsfy_builder_base& b)
{
  b.b_read(bfs);
}

//=======================================================================

std::ostream& operator<<(std::ostream& os,const clsfy_builder_base& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================

void vsl_print_summary(std::ostream& os,const clsfy_builder_base* b)
{
  if (b)
    os << *b;
  else
    os << "No clsfy_builder_base defined.";
}

//=======================================================================

std::ostream& operator<<(std::ostream& os,const clsfy_builder_base* b)
{
  vsl_print_summary(os, b);
  return os;
}
