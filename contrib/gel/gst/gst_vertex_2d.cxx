// This is gel/gst/gst_vertex_2d.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gst_vertex_2d.h"
#include <vcl_iostream.h>

// output
vcl_ostream &operator <<( vcl_ostream &os, const gst_vertex_2d &v)
{
  return os << "[gst_vertex_2d " << v.get_x() << ' ' << v.get_y() << ']';
}
