// This is mul/vimt/vimt_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vimt_image.h"
#if VCL_CXX_HAS_HEADER_TYPEINFO
# include <vcl_typeinfo.h>
#endif
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <vcl_iostream.h>

//=======================================================================

short vimt_image::version_no() const
{
    return 1;
}

//=======================================================================
//: Shallow equality.
// tests if the two images are the same type, have equal transforms, and point
// to the same image data with equal step sizes, etc.
bool vimt_image::operator==(const vimt_image &im) const
{
#if VCL_HAS_RTTI && VCL_CXX_HAS_HEADER_TYPEINFO
  if (typeid(*this) == typeid(im))
#else
  if (this->is_class(im.is_a()))
#endif
    return equals(im);
  else
    return false;
}

//=======================================================================

vcl_string vimt_image::is_a() const
{
  static const vcl_string c_ = "vimt_image";
  return c_;
}

//=======================================================================

bool vimt_image::is_class(vcl_string const& s) const
{
  return s==vimt_image::is_a();
}


//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const vimt_image& b)
{
    b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, vimt_image& b)
{
    b.b_read(bfs);
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const vimt_image& b)
{
    os << b.is_a() << ": ";
    vsl_indent_inc(os);
    b.print_summary(os);
    vsl_indent_dec(os);
    return os;
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const vimt_image* b)
{
    if (b)
        return os << *b;
    else
        return os << "No vimt_image defined.";
}

//: Print class to os
void vsl_print_summary(vcl_ostream& os, const vimt_image& im)
{
  im.print_summary(os);
}

//=======================================================================

void vsl_add_to_binary_loader(const vimt_image& b)
{
    vsl_binary_loader<vimt_image>::instance().add(b);
}
