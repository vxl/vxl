// This is mul/vil2/vil2_image_data.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott  ISBE Manchester
// \date   20 Sep 2002
//
//-----------------------------------------------------------------------------

#include <vil2/vil2_image_data.h>
#include <vcl_climits.h> // CHAR_BIT
#include <vil2/vil2_image_view_base.h>

//--------------------------------------------------------------------------------

//: the reference count starts at 0.
vil2_image_data::vil2_image_data() : reference_count_(0) { }

vil2_image_data::~vil2_image_data() { }


bool vil2_image_data::get_property(char const *, void *) const
{
  return false;
}


//: Return the name of the class;
vcl_string vil2_image_data::is_a() const
{
  static const vcl_string class_name_="vil2_image_data";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil2_image_data::is_class(vcl_string const& s) const
{
  return s==vil2_image_data::is_a();
}


//: Check that a view will fit into the data at the given offset.
bool vil2_image_data::view_fits(const vil2_image_view_base& im, unsigned x0, unsigned y0,
               unsigned plane0)
{
  if (x0 + im.nx() > nx() || y0 + im. ny() > ny() ||
    plane0 + im.nplanes() > nplanes())
    return false;
  return true;
}
