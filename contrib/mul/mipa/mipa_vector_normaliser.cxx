//:
// \file
// \author Martin Roberts
// \brief Base class for normalisation algorithms for image texture vectors

#include "mipa_vector_normaliser.h"
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_exception.h>

//=======================================================================

void vsl_add_to_binary_loader(const mipa_vector_normaliser& b)
{
  vsl_binary_loader<mipa_vector_normaliser>::instance().add(b);
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mipa_vector_normaliser& b)
{
  b.b_write(bfs);
}

//=======================================================================
//: Initialise from a text stream.
// The default implementation is for attribute-less normalisers,
// and throws if it finds any data in the stream.
void mipa_vector_normaliser::config_from_stream(
  std::istream &is, const mbl_read_props_type & /*extra_props*/)
{
  std::string s = mbl_parse_block(is);
  if (s.empty() || s=="{}") return;

  mbl_exception_parse_error x(
    this->is_a() + " expects no properties in initialisation,\n"
    "But the following properties were given:\n" + s);
  mbl_exception_error(x);
}


//=======================================================================
//: Create a concrete mipa_vector_normaliser-derived object, from a text specification.
std::unique_ptr<mipa_vector_normaliser>
  mipa_vector_normaliser::new_normaliser_from_stream(
    std::istream &is, const mbl_read_props_type &extra_props)
{
  std::string name;
  is >> name;

  std::unique_ptr<mipa_vector_normaliser> ps =
    mbl_cloneables_factory<mipa_vector_normaliser>::get_clone(name);

  ps -> config_from_stream(is, extra_props);
  return ps;
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mipa_vector_normaliser& b)
{
  b.b_read(bfs);
}

//=======================================================================

std::ostream& operator<<(std::ostream& os,const mipa_vector_normaliser& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================

std::ostream& operator<<(std::ostream& os,const mipa_vector_normaliser* b)
{
  if (b)
    return os << *b;
  else
    return os << "No mipa_vector_normaliser defined.";
}

//=======================================================================
//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const mipa_vector_normaliser& b)
{
  os << b;
}

//=======================================================================
//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const mipa_vector_normaliser* b)
{
  if (b)
    os << *b;
  else
    os << vsl_indent() << "No mipa_vector_normaliser defined.";
}
