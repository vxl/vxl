// This is gel/gmvl/gmvl_topology_node.h
#ifndef gmvl_topology_node_h_
#define gmvl_topology_node_h_
//:
// \file
// \author crossge@crd.ge.com
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vtol/vtol_topology_object_sptr.h>
#include <gmvl/gmvl_node.h>

class gmvl_topology_node : public gmvl_node
{
 public:
  gmvl_topology_node( vtol_topology_object_sptr t) : topology_(t) { type_="gmvl_topology_node"; }
  ~gmvl_topology_node() {}

  vtol_topology_object_sptr get() const { return topology_; }

 protected:
  vtol_topology_object_sptr topology_;
};

#endif // gmvl_topology_node_h_
