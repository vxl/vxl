// This is gel/gmvl/gmvl_tag_node.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gmvl_tag_node.h"

gmvl_tag_node::gmvl_tag_node( const vcl_string name)
  : name_( name)
{
  type_= "gmvl_tag_node";
}

gmvl_tag_node::~gmvl_tag_node()
{
}

