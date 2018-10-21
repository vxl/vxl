// This is mul/pdf1d/pdf1d_builder.cxx

//:
// \file
// \author Tim Cootes
// \brief Base for classes to build pdf1d_pdf objects.

#include "pdf1d_builder.h"
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_data_array_wrapper.h>

//=======================================================================

pdf1d_builder::pdf1d_builder() = default;

//=======================================================================

pdf1d_builder::~pdf1d_builder() = default;

//: Build model from data
void pdf1d_builder::build_from_array(pdf1d_pdf& model, const double* data, int n) const
{
  mbl_data_array_wrapper<double> wrapper(data,n);
  build(model,wrapper);
}


//=======================================================================

short pdf1d_builder::version_no() const
{
  return 1;
}

//=======================================================================

void vsl_add_to_binary_loader(const pdf1d_builder& b)
{
  vsl_binary_loader<pdf1d_builder>::instance().add(b);
}

//=======================================================================

std::string pdf1d_builder::is_a() const
{
  return std::string("pdf1d_builder");
}

//=======================================================================

bool pdf1d_builder::is_class(std::string const& s) const
{
  return s==pdf1d_builder::is_a();
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const pdf1d_builder& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, pdf1d_builder& b)
{
  b.b_read(bfs);
}

//=======================================================================

void vsl_print_summary(std::ostream& os,const pdf1d_builder& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

void vsl_print_summary(std::ostream& os,const pdf1d_builder* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No pdf1d_builder defined.";
}

//=======================================================================

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const pdf1d_builder& b)
{
  vsl_print_summary(os,b);
  return os;
}

//=======================================================================

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const pdf1d_builder* b)
{
  vsl_print_summary(os,b);
  return os;
}
