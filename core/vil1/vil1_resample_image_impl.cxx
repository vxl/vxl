// This is ./vxl/vil/vil_resample_image_impl.cxx
#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file

/*
  fsm@robots.ox.ac.uk
*/
#include "vil_resample_image_impl.h"
#include "vil_resample_image.h"
#include <vil/vil_byte.h>
#include <vil/vil_pixel.h>
#include <vcl_climits.h> // CHAR_BIT
#include <vcl_iostream.h>
#include <vcl_cassert.h>

vil_resample_image_impl::vil_resample_image_impl(vil_image const &underlying, unsigned nw, unsigned nh)
  : base(underlying)
  , new_width(nw)
  , new_height(nh)
{
  assert(base);
  assert(new_width>0);
  assert(new_height>0);
}

vil_resample_image_impl::~vil_resample_image_impl()
{
}

//--------------------------------------------------------------------------------

vil_image vil_resample_image_impl::get_plane(int p) const
{
  vil_image_impl *i = new vil_resample_image_impl(base.get_plane(p), new_width, new_height);
  return vil_image(i);
}

bool vil_resample_image_impl::put_section(void const * , int, int, int, int)
{
  return false;
}

bool vil_resample_image_impl::get_property(char const *, void *) const
{
  return false;
}

//--------------------------------------------------------------------------------

#include <vil/vil_rgb.h>
#include <vil/vil_byte.h>

bool vil_resample_image_impl::get_section(void *buf, int x0, int y0, int w, int h) const
{
  assert(buf!=0);
  //vcl_cerr << "x0 y0 w h = " << x0 << ' ' << y0 << ' ' << w << ' ' << h << vcl_endl;

  if (base.bits_per_component() % CHAR_BIT) {
    vcl_cerr << __FILE__ " : urgh!" << vcl_endl;
    return false; // FIXME
  }

  // compute and check bounds of region requested.
  if (x0 < 0 || y0 < 0 || x0+w > (int)new_width || y0+h > (int)new_height) {
    vcl_cerr << __FILE__ ": invalid section bounds" << vcl_endl;
    return false;
  }

  // switch on pixel type:
  switch ( vil_pixel_format(base) ) {
  case VIL_BYTE:
    return vil_resample_image(base, new_width, new_height,
                        (vil_byte*)buf, (unsigned*)0,
                        x0, y0, w, h);
  case VIL_RGB_BYTE:
    return vil_resample_image(base, new_width, new_height,
                        (vil_rgb<vil_byte>*)buf, (vil_rgb<int>*)0,
                        x0, y0, w, h);

  default:
    vcl_cerr << __FILE__ ": not implemented for this pixel type" << vcl_endl;
    return false;
  }
}

/* START_MANCHESTER_BINARY_IO_CODE */

//: Return the name of the class;
vcl_string vil_resample_image_impl::is_a() const
{
  static const vcl_string class_name_="vil_resample_image_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil_resample_image_impl::is_class(vcl_string const& s) const
{
  return s==vil_resample_image_impl::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */
