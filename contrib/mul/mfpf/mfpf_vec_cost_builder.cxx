#include <iostream>
#include <sstream>
#include "mfpf_vec_cost_builder.h"
//:
// \file
// \brief Base for classes which build mfpf_vec_cost objects.
// \author Tim Cootes

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_cloneables_factory.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_vec_cost_builder::mfpf_vec_cost_builder() = default;

//=======================================================================
// Destructor
//=======================================================================

mfpf_vec_cost_builder::~mfpf_vec_cost_builder() = default;


//: Initialise from a string stream
bool mfpf_vec_cost_builder::set_from_stream(std::istream &is)
{
  // Cycle through string and produce a map of properties
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  if (props.size()!=0)
  {
    std::cerr<<is_a()<<" does not expect any extra arguments.\n";
    mbl_read_props_look_for_unused_props(
      "mfpf_vec_cost_builder::set_from_stream", props, mbl_read_props_type());
  }
  return true;
}

//: Create a concrete object, from a text specification.
std::unique_ptr<mfpf_vec_cost_builder> mfpf_vec_cost_builder::
  create_from_stream(std::istream &is)
{
  std::string name;
  is >> name;
  std::unique_ptr<mfpf_vec_cost_builder> vcb;
  try {
    vcb = mbl_cloneables_factory<mfpf_vec_cost_builder>::get_clone(name);
  }
  catch (const mbl_exception_no_name_in_factory & e)
  {
    throw (mbl_exception_parse_error( e.what() ));
  }
  vcb->set_from_stream(is);
  return vcb;
}


//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_vec_cost_builder::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_vec_cost_builder::is_a() const
{
  return std::string("mfpf_vec_cost_builder");
}


//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_vec_cost_builder& b)
{
  vsl_binary_loader<mfpf_vec_cost_builder>::instance().add(b);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mfpf_vec_cost_builder& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mfpf_vec_cost_builder& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mfpf_vec_cost_builder& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mfpf_vec_cost_builder* b)
{
  if (b)
    return os << *b;
  else
    return os << "No mfpf_vec_cost_builder defined.";
}
