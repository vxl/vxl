#ifndef top_H
#define top_H

// temperary holding class to test topology hierarch node

// #include <vtol/vtol_topology_hierarchy_node.h>


#include <vcl/vcl_vector.h>

class top_hold;
typedef vcl_vector<top_hold*> topology_list;

// class top_hold : public vtol_topology_hierarchy_node
class top 
{
 public:
  
  top();
  ~top();

 private:
  
  // vcl_vector<top_hold*> _list;
   topology_list _list;

};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vtol_topology_hierarchy_node.




