// This is gel/gmvl/gmvl_corner_node.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gmvl_corner_node.h"

gmvl_corner_node::gmvl_corner_node( double x, double y)
  : x_(x), y_(y)
{
  type_= "gmvl_corner_node";
}

gmvl_corner_node::~gmvl_corner_node()
{
}
