#include "mfpf_region_definer.h"
//:
// \file
// \brief Base for objects which generate regions from sets of points
// \author Tim Cootes

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_cloneables_factory.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_region_definer::mfpf_region_definer()
{
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_region_definer::~mfpf_region_definer()
{
}

//: Initialise from a string stream
bool mfpf_region_definer::set_from_stream(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  if (props.size()!=0)
  {
    vcl_cerr<<is_a()<<" does not expect any extra arguments.\n";
    mbl_read_props_look_for_unused_props(
      "mfpf_region_definer::set_from_stream", props, mbl_read_props_type());
  }
  return true;
}

//: Create a concrete object, from a text specification.
vcl_auto_ptr<mfpf_region_definer> mfpf_region_definer::
  create_from_stream(vcl_istream &is)
{
  vcl_string name;
  is >> name;
  vcl_auto_ptr<mfpf_region_definer> vcb;
  try {
    vcb = mbl_cloneables_factory<mfpf_region_definer>::get_clone(name);
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

short mfpf_region_definer::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_region_definer::is_a() const
{
  return vcl_string("mfpf_region_definer");
}

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_region_definer& b)
{
  vsl_binary_loader<mfpf_region_definer>::instance().add(b);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mfpf_region_definer& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mfpf_region_definer& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mfpf_region_definer& b)
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

vcl_ostream& operator<<(vcl_ostream& os,const mfpf_region_definer* b)
{
  if (b)
    return os << *b;
  else
    return os << "No mfpf_region_definer defined.";
}
