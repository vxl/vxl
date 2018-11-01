// This is core/vil1/vil1_image_impl.cxx
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   16 Feb 00
//
//-----------------------------------------------------------------------------

#include "vil1_image_impl.h"
#include <vil1/vil1_image.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//--------------------------------------------------------------------------------

vil1_image vil1_image_impl::get_plane(unsigned int p) const
{
  assert((int)p < planes());
  return nullptr; // since this function has to be overloaded
}

bool vil1_image_impl::get_property(char const *, void *) const
{
  return false;
}

bool vil1_image_impl::set_property(char const *, void const *) const
{
  return false;
}

bool vil1_image_impl::get_section(void* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) const
{
  return false;
}

bool vil1_image_impl::put_section(void const* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/)
{
  return false;
}
