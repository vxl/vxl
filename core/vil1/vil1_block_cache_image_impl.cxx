// This is ./vxl/vil/vil_block_cache_image_impl.cxx
#ifdef __GNUC__
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vil_block_cache_image_impl.h"
#include <vcl_cstring.h>  // strcmp()

// Constructors/Destructors--------------------------------------------------

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vil_block_cache_image_impl::vil_block_cache_image_impl(vil_image i_, unsigned bx, unsigned by)
  : base(i_),block_size_x(bx),block_size_y(by)
{
}

vil_block_cache_image_impl::~vil_block_cache_image_impl() {
}

vil_image vil_block_cache_image_impl::get_plane(int ) const {
  return 0;
}

bool vil_block_cache_image_impl::get_section(void *buf, int x0, int y0, int w, int h) const {
  // implement this, please.
  return base.get_section(buf, x0, y0, w, h);
}

bool vil_block_cache_image_impl::put_section(void const *buf, int x0, int y0, int w, int h) {
  return base.put_section(buf, x0, y0, w, h);
}

//--------------------------------------------------------------------------------

bool vil_block_cache_image_impl::get_property(char const *tag,
                                              void *out) const
{
  if (vcl_strcmp(tag, "is_blocked") == 0)
    return true;

  if (vcl_strcmp(tag, "block_size_x") == 0) {
    if (out) *(unsigned *)out = block_size_x;
    return true;
  }

  if (vcl_strcmp(tag, "block_size_y") == 0) {
    if (out) *(unsigned *)out = block_size_y;
    return true;
  }

  return false;
}


/* START_MANCHESTER_BINARY_IO_CODE */

//: Return the name of the class;
vcl_string vil_block_cache_image_impl::is_a() const
{
  static const vcl_string class_name_="vil_block_cache_image_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil_block_cache_image_impl::is_class(vcl_string const& s) const
{
  return s==vil_block_cache_image_impl::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */
