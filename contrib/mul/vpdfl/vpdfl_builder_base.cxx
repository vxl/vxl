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

vpdfl_builder_base::vpdfl_builder_base()
{
}

//=======================================================================

vpdfl_builder_base::~vpdfl_builder_base()
{
}

//=======================================================================

short vpdfl_builder_base::version_no() const
{
  return 1;
}

//=======================================================================

void vsl_add_to_binary_loader(const vpdfl_builder_base& b)
{
  vsl_binary_loader<vpdfl_builder_base>::instance().add(b);
}

//=======================================================================

vcl_string vpdfl_builder_base::is_a() const
{
  return vcl_string("vpdfl_builder_base");
}

//=======================================================================

bool vpdfl_builder_base::is_class(vcl_string const& s) const
{
  return s==vpdfl_builder_base::is_a();
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const vpdfl_builder_base& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, vpdfl_builder_base& b)
{
  b.b_read(bfs);
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const vpdfl_builder_base& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const vpdfl_builder_base* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No vpdfl_builder_base defined.";
}

//=======================================================================

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const vpdfl_builder_base& b)
{
  vsl_print_summary(os,b);
  return os;
}

//=======================================================================

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const vpdfl_builder_base* b)
{
  vsl_print_summary(os,b);
  return os;
}
