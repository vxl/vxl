#ifndef gmvl_topology_node_h_
#define gmvl_topology_node_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl_string.h>
#include <vtol/vtol_topology_object_ref.h>
#include <gmvl/gmvl_node.h>

class gmvl_topology_node : public gmvl_node
{
public:

  gmvl_topology_node( vtol_topology_object_ref topology);
  ~gmvl_topology_node();

  vtol_topology_object_ref get() const { return topology_; }

protected:

  vtol_topology_object_ref topology_;

};

#endif
