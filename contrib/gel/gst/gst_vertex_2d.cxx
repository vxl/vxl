// This is gel/gst/gst_vertex_2d.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gst_vertex_2d.h"
#include <vcl_compiler.h>
#include <iostream>
#include <iostream>

// output
std::ostream &operator <<( std::ostream &os, const gst_vertex_2d &v)
{
  return os << "[gst_vertex_2d " << v.get_x() << ' ' << v.get_y() << ']';
}
