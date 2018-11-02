#include <iostream>
#include <sstream>
#include "mmn_solver.h"
//:
// \file
// \brief Base for classes which solve Markov Random Field problems
// \author Tim Cootes

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_cloneables_factory.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mmn_solver::mmn_solver() = default;

//=======================================================================
// Destructor
//=======================================================================

mmn_solver::~mmn_solver() = default;


//: Initialise from a string stream
bool mmn_solver::set_from_stream(std::istream &is)
{
  // Cycle through string and produce a map of properties
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  if (props.size()!=0)
  {
    std::cerr<<is_a()<<" does not expect any extra arguments.\n";
    mbl_read_props_look_for_unused_props(
      "mmn_solver::set_from_stream", props, mbl_read_props_type());
  }
  return true;
}

//=======================================================================
// Method: version_no
//=======================================================================

short mmn_solver::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mmn_solver::is_a() const
{
  return std::string("mmn_solver");
}

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mmn_solver& b)
{
  vsl_binary_loader<mmn_solver>::instance().add(b);
}

//: Create a concrete region_model-derived object, from a text specification.
std::unique_ptr<mmn_solver> mmn_solver::
  create_from_stream(std::istream &is)
{
  std::string name;
  is >> name;
  std::unique_ptr<mmn_solver> pair_cost;
  try {
    pair_cost = mbl_cloneables_factory<mmn_solver>::get_clone(name);
  }
  catch (const mbl_exception_no_name_in_factory & e)
  {
    throw (mbl_exception_parse_error( e.what() ));
  }
  pair_cost->set_from_stream(is);
  return pair_cost;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mmn_solver& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mmn_solver& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mmn_solver& b)
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

std::ostream& operator<<(std::ostream& os,const mmn_solver* b)
{
  if (b)
    return os << *b;
  else
    return os << "No mmn_solver defined.";
}
