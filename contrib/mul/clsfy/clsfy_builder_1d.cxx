// This is mul/clsfy/clsfy_builder_1d.cxx
#include "clsfy_builder_1d.h"
//:
// \file
// \brief Describe an abstract classifier builder for scalar data
// \author Tim Cootes

//=======================================================================

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

clsfy_builder_1d::clsfy_builder_1d()
{
}

//=======================================================================

clsfy_builder_1d::~clsfy_builder_1d()
{
}


//=======================================================================

void vsl_add_to_binary_loader(const clsfy_builder_1d& b)
{
  vsl_binary_loader<clsfy_builder_1d>::instance().add(b);
}

//=======================================================================

vcl_string clsfy_builder_1d::is_a() const
{
  return vcl_string("clsfy_builder_1d");
}

bool clsfy_builder_1d::is_class(vcl_string const& s) const
{
  return s == clsfy_builder_1d::is_a();
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& os, const clsfy_builder_1d& b)
{
  b.b_write(os);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, clsfy_builder_1d& b)
{
  b.b_read(bfs);
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const clsfy_builder_1d& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const clsfy_builder_1d* b)
{
  if (b)
    return os << *b;
  else
    return os << "No clsfy_builder_1d defined.";
}

