// This is ./vxl/vil/vil_image_impl.cxx
#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   16 Feb 00
//
//-----------------------------------------------------------------------------

#include "vil_image_impl.h"

#include <vcl_climits.h> // CHAR_BIT
#include <vcl_iostream.h>
#include <vil/vil_image.h>

//--------------------------------------------------------------------------------

//: the reference count starts at 0.
vil_image_impl::vil_image_impl() : reference_count(0) { }

vil_image_impl::~vil_image_impl() { }

vil_image vil_image_impl::get_plane(int ) const
{
  vcl_cerr << __FILE__ ":" << __LINE__ << ": get_plane()" << vcl_endl; // probably remove this.
  return 0;
}

bool vil_image_impl::get_property(char const *, void *) const
{
  return false;
}

bool vil_image_impl::set_property(char const *, void const *) const
{
  return false;
}

bool vil_image_impl::get_section(void* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) const
{
  return false;
}

bool vil_image_impl::put_section(void const* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/)
{
  return false;
}

/* START_MANCHESTER_BINARY_IO_CODE */

//: Return the name of the class;
vcl_string vil_image_impl::is_a() const
{
  static const vcl_string class_name_="vil_image_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil_image_impl::is_class(vcl_string const& s) const
{
  return s==vil_image_impl::is_a();
}

/* END_MANCHESTER_BINARY_IO_CODE */
