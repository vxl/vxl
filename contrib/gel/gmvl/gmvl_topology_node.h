#ifndef gmvl_topology_node_h_
#define gmvl_topology_node_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  crossge@crd.ge.com
*/

#include <vtol/vtol_topology_object_sptr.h>
#include <gmvl/gmvl_node.h>

class gmvl_topology_node : public gmvl_node
{
public:

  gmvl_topology_node( vtol_topology_object_sptr topology);
  ~gmvl_topology_node();

  vtol_topology_object_sptr get() const { return topology_; }

protected:

  vtol_topology_object_sptr topology_;
};

#endif // gmvl_topology_node_h_
