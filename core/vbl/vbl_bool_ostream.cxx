// This is core/vbl/vbl_bool_ostream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vbl_bool_ostream.h"
#include <vcl_compiler.h>
#include <iostream>

std::ostream& operator<<(std::ostream& s, const vbl_bool_ostream::on_off& proxy)
{
  if (*(proxy.truth))
    s << "on";
  else
    s << "off";
  return s;
}

std::ostream& operator<<(std::ostream& s, const vbl_bool_ostream::high_low& proxy)
{
  if (*(proxy.truth))
    s << "high";
  else
    s << "low";
  return s;
}

std::ostream& operator<<(std::ostream& s, const vbl_bool_ostream::true_false& proxy)
{
  if (*(proxy.truth))
    s << "true";
  else
    s << "false";
  return s;
}

