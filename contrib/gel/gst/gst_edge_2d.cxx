// This is gel/gst/gst_edge_2d.cxx
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include "gst_edge_2d.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// output
std::ostream &operator<<( std::ostream &os, gst_edge_2d &e)
{
  return os << '<' << e.start_.ptr() << ',' << e.end_.ptr() << '>';
}
