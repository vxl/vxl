// This is gel/gst/gst_edge_2d.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gst_edge_2d.h"
#include <vcl_compiler.h>
#include <iostream>
#include <iostream>

// output
std::ostream &operator<<( std::ostream &os, gst_edge_2d &e)
{
  return os << '<' << e.start_.ptr() << ',' << e.end_.ptr() << '>';
}
