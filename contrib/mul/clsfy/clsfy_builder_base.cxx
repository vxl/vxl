// This is mul/clsfy/clsfy_builder_base.cxx
// Copyright: (C) 2000 Britsh Telecommunications plc
#include "clsfy_builder_base.h"
//:
// \file
// \brief Implement bits of an abstract classifier builder
// \author Ian Scott
// \date 2000/05/10
// \verbatim
//  Modifications
//  2 May 2001 IMS Converted to VXL
// \endverbatim


//=======================================================================

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

clsfy_builder_base::clsfy_builder_base()
{
}

//=======================================================================

clsfy_builder_base::~clsfy_builder_base()
{
}


//=======================================================================

void vsl_add_to_binary_loader(const clsfy_builder_base& b)
{
  vsl_binary_loader<clsfy_builder_base>::instance().add(b);
}

//=======================================================================

vcl_string clsfy_builder_base::is_a() const
{
  return vcl_string("clsfy_builder_base");
}

//=======================================================================

bool clsfy_builder_base::is_class(vcl_string const& s) const
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

vcl_ostream& operator<<(vcl_ostream& os,const clsfy_builder_base& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const clsfy_builder_base* b)
{
  if (b)
    os << *b;
  else
    os << "No clsfy_builder_base defined.";
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const clsfy_builder_base* b)
{
  vsl_print_summary(os, b);
  return os;
}

