/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "gmvl_topology_node.h"


gmvl_topology_node::gmvl_topology_node( vtol_topology_object_sptr topology)
  : topology_(topology)
{
  type_= "gmvl_topology_node";
}

gmvl_topology_node::~gmvl_topology_node()
{
}

