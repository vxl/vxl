/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vil_image"
#endif

#include "vil_image.h"

#include <vcl/vcl_climits.h>
#include <vcl/vcl_iostream.h>

ostream& vil_image::print(ostream& s) const
{
  s << "[vil_image: size " << width() << " x " << height();
  char const *fmt = file_format();
  s << ", file format " << (fmt ? fmt : "unknown");
  s << ", components " << components();
  s << ", bits per component " << bits_per_component();
#if 0
  s << ", type " << component_format() << " -- " << pixel_type();
#else
  s << ", format "; switch (component_format()) {
  case VIL_COMPONENT_FORMAT_UNSIGNED_INT: s << "unsigned"; break;
  case VIL_COMPONENT_FORMAT_SIGNED_INT: s << "signed"; break;
  case VIL_COMPONENT_FORMAT_IEEE_FLOAT: s<< "float"; break;
  case VIL_COMPONENT_FORMAT_COMPLEX: s << "complex"; break;
  case VIL_COMPONENT_FORMAT_UNKNOWN: s << "unknown"; break;
  default: s << "??"; break;
  }
#endif
  return s << "]";
}

int vil_image::get_size_bytes() const 
{
  return planes() * width() * height() * components() * bits_per_component() / CHAR_BIT;
}
