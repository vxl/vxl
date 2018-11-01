// This is core/vil1/vil1_resample_image_impl.cxx
//:
// \file
// \author fsm

#include <iostream>
#include "vil1_resample_image_impl.h"
#include <vil1/vil1_resample_image.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_pixel.h>
#include <vxl_config.h>
#include <climits>// CHAR_BIT
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

vil1_resample_image_impl::vil1_resample_image_impl(vil1_image const &underlying, unsigned nw, unsigned nh)
  : base(underlying)
  , new_width(nw)
  , new_height(nh)
{
  assert(base);
  assert(new_width>0);
  assert(new_height>0);
}

//--------------------------------------------------------------------------------

vil1_image vil1_resample_image_impl::get_plane(unsigned int p) const
{
  vil1_image_impl *i = new vil1_resample_image_impl(base.get_plane(p), new_width, new_height);
  return vil1_image(i);
}

bool vil1_resample_image_impl::put_section(void const * , int, int, int, int)
{
  return false;
}

bool vil1_resample_image_impl::get_property(char const *, void *) const
{
  return false;
}

//--------------------------------------------------------------------------------

bool vil1_resample_image_impl::get_section(void *buf, int x0, int y0, int w, int h) const
{
  assert(buf!=nullptr);
#ifdef DEBUG
  std::cerr << "get_section() x0 y0 w h = " << x0 << ' ' << y0 << ' ' << w << ' ' << h << '\n';
#endif

  if (base.bits_per_component() % CHAR_BIT) {
    std::cerr << __FILE__ " : urgh!\n";
    return false; // FIXME
  }

  // compute and check bounds of region requested.
  if (x0 < 0 || y0 < 0 || x0+w > (int)new_width || y0+h > (int)new_height) {
    std::cerr << __FILE__ ": invalid section bounds\n";
    return false;
  }

  // switch on pixel type:
  switch ( vil1_pixel_format(base) ) {
  case VIL1_BYTE:
    return vil1_resample_image(base, new_width, new_height,
                               (vxl_byte*)buf, (unsigned*)nullptr,
                               x0, y0, w, h);
  case VIL1_RGB_BYTE:
    return vil1_resample_image(base, new_width, new_height,
                               (vil1_rgb<vxl_byte>*)buf, (vil1_rgb<int>*)nullptr,
                               x0, y0, w, h);

  default:
    std::cerr << __FILE__ ": not implemented for this pixel type\n";
    return false;
  }
}
