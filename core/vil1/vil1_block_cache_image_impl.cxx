// This is core/vil1/vil1_block_cache_image_impl.cxx
//:
// \file
// \author fsm

#include <cstring>
#include "vil1_block_cache_image_impl.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

// Constructors/Destructors--------------------------------------------------

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vil1_block_cache_image_impl::vil1_block_cache_image_impl(const vil1_image& i_, unsigned bx, unsigned by)
  : base(i_),block_size_x(bx),block_size_y(by)
{
}

vil1_block_cache_image_impl::~vil1_block_cache_image_impl() = default;

vil1_image vil1_block_cache_image_impl::get_plane(unsigned int p) const {
  assert((int)p < base.planes());
  return base.get_plane(p);
}

bool vil1_block_cache_image_impl::get_section(void *buf, int x0, int y0, int w, int h) const {
  // implement this, please.
  return base.get_section(buf, x0, y0, w, h);
}

bool vil1_block_cache_image_impl::put_section(void const *buf, int x0, int y0, int w, int h) {
  return base.put_section(buf, x0, y0, w, h);
}

//--------------------------------------------------------------------------------

bool vil1_block_cache_image_impl::get_property(char const *tag,
                                               void *out) const
{
  if (std::strcmp(tag, "is_blocked") == 0)
    return true;

  if (std::strcmp(tag, "block_size_x") == 0) {
    if (out) *(unsigned *)out = block_size_x;
    return true;
  }

  if (std::strcmp(tag, "block_size_y") == 0) {
    if (out) *(unsigned *)out = block_size_y;
    return true;
  }

  return false;
}


//: Return the name of the class;
std::string vil1_block_cache_image_impl::is_a() const
{
  static const std::string class_name_="vil1_block_cache_image_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil1_block_cache_image_impl::is_class(std::string const& s) const
{
  return s==vil1_block_cache_image_impl::is_a() || vil1_image_impl::is_class(s);
}
