#ifndef top_hold_H
#define top_hold_H

// temperary holding class to test topology hierarch node

#include <vtol/vtol_topology_hierarchy_node.h>

class top_hold : public vtol_topology_hierarchy_node
{
 public:
  
  top_hold();
  ~top_hold();

  // must be added to topology_object 

  inline bool link_inferior(top_hold* child)
    { return vtol_topology_hierarchy_node::link_inferior(this, child); }
  inline bool link_superior(top_hold* parent)
    { return vtol_topology_hierarchy_node::link_superior(parent, this); }
  inline bool unlink_inferior(top_hold* child)
    { return vtol_topology_hierarchy_node::unlink_inferior(this, child); }
  inline bool unlink_superior(top_hold* parent)
    { return vtol_topology_hierarchy_node::unlink_superior(parent, this); }


};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vtol_topology_hierarchy_node.




