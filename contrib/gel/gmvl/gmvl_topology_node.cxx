// This is gel/gmvl/gmvl_topology_node.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gmvl_topology_node.h"

gmvl_topology_node::gmvl_topology_node( vtol_topology_object_sptr topology)
  : topology_(topology)
{
  type_= "gmvl_topology_node";
}

gmvl_topology_node::~gmvl_topology_node()
{
}

