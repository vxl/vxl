// This is core/vil1/vil1_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil1_image.h"
#include <vcl_climits.h> // for CHAR_BIT
#include <vcl_iostream.h>

vcl_ostream& vil1_image::print(vcl_ostream& s) const
{
  if (! *this) return s << "[vil1_image: empty]";

  s << "[vil1_image: size " << width() << " x " << height();
  char const *fmt = file_format();
  s << ", file format " << (fmt ? fmt : "unknown")
    << ", components " << components()
    << ", bits per component " << bits_per_component();
#if 1
  s << ", format " << vil1_print(component_format());
#else
  s << ", format "; switch (component_format()) {
  case VIL1_COMPONENT_FORMAT_UNSIGNED_INT: s << "unsigned"; break;
  case VIL1_COMPONENT_FORMAT_SIGNED_INT: s << "signed"; break;
  case VIL1_COMPONENT_FORMAT_IEEE_FLOAT: s<< "float"; break;
  case VIL1_COMPONENT_FORMAT_COMPLEX: s << "complex"; break;
  case VIL1_COMPONENT_FORMAT_UNKNOWN: s << "unknown"; break;
  default: s << "??"; break;
  }
#endif
  return s << "]";
}

int vil1_image::get_size_bytes() const
{
  return planes() * height() * ((width() * components() * bits_per_component() + CHAR_BIT-1) / CHAR_BIT);
}
