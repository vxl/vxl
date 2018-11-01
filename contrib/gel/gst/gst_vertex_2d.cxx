// This is gel/gst/gst_vertex_2d.cxx
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include "gst_vertex_2d.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// output
std::ostream &operator <<( std::ostream &os, const gst_vertex_2d &v)
{
  return os << "[gst_vertex_2d " << v.get_x() << ' ' << v.get_y() << ']';
}
