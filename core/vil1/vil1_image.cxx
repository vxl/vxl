/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_image.h"

#include <vcl/vcl_climits.h>
#include <vcl/vcl_iostream.h>

//   VIL_BYTE,       /*!< 1 x W x H x 1 of UINT x 8*/
//   VIL_RGB_BYTE,   /*!< 1 x W x H x 3 of UINT x 8*/
//   VIL_UINT16,     /*!< 1 x W x H x 1 of UINT x 16*/
//   VIL_UINT32,     /*!< 1 x W x H x 1 of UINT x 32*/
//   VIL_FLOAT,      /*!< 1 x W x H x 1 of IEEE_FLOAT x 32*/
//   VIL_DOUBLE,     /*!< 1 x W x H x 1 of IEEE_DOUBLE x 64*/
//   VIL_COMPLEX     /*!< 1 x W x H x 1 of COMPLEX x 64*/

vil_pixel_format vil_image::pixel_type() const
{
  int c = components();
  int b = bits_per_component();
  int f = component_format();

  if (c == 1 && b == 8 && f == VIL_COMPONENT_FORMAT_UNSIGNED_INT) return VIL_BYTE;
  if (c == 3 && b == 8 && f == VIL_COMPONENT_FORMAT_UNSIGNED_INT) return VIL_RGB_BYTE;
  if (c == 1 && b == 16 && f == VIL_COMPONENT_FORMAT_UNSIGNED_INT) return VIL_UINT16;
  if (c == 1 && b == 32 && f == VIL_COMPONENT_FORMAT_UNSIGNED_INT) return VIL_UINT32;
  if (c == 1 && b == 32 && f == VIL_COMPONENT_FORMAT_IEEE_FLOAT) return VIL_FLOAT;
  if (c == 1 && b == 64 && f == VIL_COMPONENT_FORMAT_IEEE_FLOAT) return VIL_DOUBLE;
  if (c == 1 && b == 64 && f == VIL_COMPONENT_FORMAT_COMPLEX) return VIL_COMPLEX;

  return VIL_PIXEL_FORMAT_UNKNOWN;
}

ostream& vil_image::print(ostream& s) const
{
  s << "[vil_image: size " << width() << " x " << height();
  s << ", components " << components();
  s << ", bits per component " << bits_per_component();
  s << ", type " << component_format() << " -- " << pixel_type() << "]";
  return s;
}

int vil_image::get_size_bytes() const 
{
  return planes() * width() * height() * components() * bits_per_component() / CHAR_BIT;
}

#if VIL_IMAGE_USE_SAFETY_NET
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_algorithm.h>
static vcl_vector<void const *> all;

void vil_image::xx_register(vil_image const *p) {
  all.push_back(p);
}
void vil_image::xx_unregister(vil_image const *p) {
  for (unsigned i=0; i<all.size(); ++i)
    if (all[i] == p) {
      all.erase(all.begin() + i);
      return;
    }
  assert(false);
}
//: return true if some vil_image refers to pi.
bool vil_image::is_reffed(vil_image_impl const *pi) {
  for (unsigned i=0; i<all.size(); ++i)
    if (static_cast<vil_image const*>(all[i])->impl() == pi)
      return true;
  return false;
}
#endif
