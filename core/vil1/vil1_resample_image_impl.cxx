// This is core/vil1/vil1_resample_image_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil1_resample_image_impl.h"
#include <vil1/vil1_resample_image.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_pixel.h>
#include <vxl_config.h>
#include <vcl_climits.h> // CHAR_BIT
#include <vcl_iostream.h>
#include <vcl_cassert.h>

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
  assert(buf!=0);
  //vcl_cerr << "x0 y0 w h = " << x0 << ' ' << y0 << ' ' << w << ' ' << h << vcl_endl;

  if (base.bits_per_component() % CHAR_BIT) {
    vcl_cerr << __FILE__ " : urgh!\n";
    return false; // FIXME
  }

  // compute and check bounds of region requested.
  if (x0 < 0 || y0 < 0 || x0+w > (int)new_width || y0+h > (int)new_height) {
    vcl_cerr << __FILE__ ": invalid section bounds\n";
    return false;
  }

  // switch on pixel type:
  switch ( vil1_pixel_format(base) ) {
  case VIL1_BYTE:
    return vil1_resample_image(base, new_width, new_height,
                               (vxl_byte*)buf, (unsigned*)0,
                               x0, y0, w, h);
  case VIL1_RGB_BYTE:
    return vil1_resample_image(base, new_width, new_height,
                               (vil1_rgb<vxl_byte>*)buf, (vil1_rgb<int>*)0,
                               x0, y0, w, h);

  default:
    vcl_cerr << __FILE__ ": not implemented for this pixel type\n";
    return false;
  }
}
