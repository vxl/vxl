//:
// \file
#include <iostream>
#include <cstdlib>
#include "mcal_single_basis_cost.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_exception.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mbl/mbl_parse_block.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

mcal_single_basis_cost::mcal_single_basis_cost() = default;

//=======================================================================

mcal_single_basis_cost::~mcal_single_basis_cost() = default;


//=======================================================================

short mcal_single_basis_cost::version_no() const
{
  return 1;
}

//=======================================================================

void vsl_add_to_binary_loader(const mcal_single_basis_cost& b)
{
  vsl_binary_loader<mcal_single_basis_cost>::instance().add(b);
}

//=======================================================================

std::string  mcal_single_basis_cost::is_a() const
{
  return std::string("mcal_single_basis_cost");
}


//: Create a concrete mcal_single_basis_cost object, from a text specification.
std::unique_ptr<mcal_single_basis_cost>
  mcal_single_basis_cost::create_from_stream(std::istream &is)
{
  std::string name;
  is >> name;

  std::unique_ptr<mcal_single_basis_cost> pvmb;
  try
  {
    pvmb = mbl_cloneables_factory<mcal_single_basis_cost>::get_clone(name);
  }
  catch (const mbl_exception_no_name_in_factory & e)
  {
      std::cerr<<"ERROR in mcal_single_basis_cost::new_vm_builder_from_stream\n"
              <<"\tRequired vector model builder of "<<name<<" is not in the factory. Further exception details follow:\n"
              <<'\t'<<e.what()<<std::endl;
      std::abort();
  }
  pvmb->config_from_stream(is);
  return pvmb;
}

//: Read initialisation settings from a stream.
// The default implementation merely checks that no properties have
// been specified.
void mcal_single_basis_cost::config_from_stream(std::istream& is)
{
  std::string s = mbl_parse_block(is);
  if (s.empty() || s=="{}") return;

  throw mbl_exception_parse_error(
    this->is_a() + " expects no properties in initialisation,\n"
    "But the following properties were given:\n" + s);
}


//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mcal_single_basis_cost& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mcal_single_basis_cost& b)
{
    b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mcal_single_basis_cost& b)
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

std::ostream& operator<<(std::ostream& os,const mcal_single_basis_cost* b)
{
    if (b)
    return os << *b;
    else
    return os << "No mcal_single_basis_cost defined.";
}
