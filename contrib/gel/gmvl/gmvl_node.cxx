// This is gel/gmvl/gmvl_node.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gmvl_node.h"

gmvl_node::gmvl_node()
  : type_( "gmvl_node"), ref_( -1)
{
}

gmvl_node::~gmvl_node()
{
}

vcl_ostream &operator<<( vcl_ostream &os, gmvl_node &node)
{
  return os << "<Node " << &node << " ref=" << node.ref_ << " type=\"" << node.type_ << "\">";
}
