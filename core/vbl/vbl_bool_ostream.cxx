// This is core/vbl/vbl_bool_ostream.cxx

#include <iostream>
#include "vbl_bool_ostream.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
