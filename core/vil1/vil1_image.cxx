// This is vxl/vil/vil_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vil_image.h"
#include <vcl_climits.h> // for CHAR_BIT
#include <vcl_iostream.h>

vcl_ostream& vil_image::print(vcl_ostream& s) const
{
  if (! *this) return s << "[vil_image: empty]";

  s << "[vil_image: size " << width() << " x " << height();
  char const *fmt = file_format();
  s << ", file format " << (fmt ? fmt : "unknown");
  s << ", components " << components();
  s << ", bits per component " << bits_per_component();
#if 1
  s << ", format " << vil_print(component_format());
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
  return planes() * height() * ((width() * components() * bits_per_component() + CHAR_BIT-1) / CHAR_BIT);
}
