#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file
// \author Tim Cootes
// \date 12-Apr-2001
// \brief Base for classes to build vpdfl_pdf_base objects.


#include <vpdfl/vpdfl_builder_base.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_builder_base::vpdfl_builder_base()
{
}

//=======================================================================
// Destructor
//=======================================================================

vpdfl_builder_base::~vpdfl_builder_base()
{
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_builder_base::version_no() const
{
  return 1;
}

//=======================================================================
// Method: vxl_add_to_binary_loader
//=======================================================================

void vsl_add_to_binary_loader(const vpdfl_builder_base& b)
{
  vsl_binary_loader<vpdfl_builder_base>::instance().add(b);
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string vpdfl_builder_base::is_a() const
{
  return vcl_string("vpdfl_builder_base");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_builder_base::is_class(vcl_string const& s) const
{
  return s==vcl_string("vpdfl_builder_base");
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const vpdfl_builder_base* b)
{
  if (b)
  {
    vsl_b_write(bfs,b->is_a());
    b->b_write(bfs);
  }
  else
    vsl_b_write(bfs,vcl_string("VSL_NULL_PTR"));
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const vpdfl_builder_base& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, vpdfl_builder_base& b)
{
  b.b_read(bfs);
}


void vsl_print_summary(vcl_ostream& os,const vpdfl_builder_base& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

void vsl_print_summary(vcl_ostream& os,const vpdfl_builder_base* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No vpdfl_builder_base defined.";
}
