#include "mfpf_vec_cost.h"
//:
// \file
// \brief Cost functions to evaluate vector of pixel samples
// \author Tim Cootes

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_vec_cost::mfpf_vec_cost() = default;

//=======================================================================
// Destructor
//=======================================================================

mfpf_vec_cost::~mfpf_vec_cost() = default;

//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_vec_cost::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_vec_cost::is_a() const
{
  return std::string("mfpf_vec_cost");
}

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_vec_cost& b)
{
  vsl_binary_loader<mfpf_vec_cost>::instance().add(b);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mfpf_vec_cost& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mfpf_vec_cost& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mfpf_vec_cost& b)
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

std::ostream& operator<<(std::ostream& os,const mfpf_vec_cost* b)
{
  if (b)
    return os << *b;
  else
    return os << "No mfpf_vec_cost defined.";
}
