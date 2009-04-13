#ifndef boxm_cell_vis_graph_node_h_
#define boxm_cell_vis_graph_node_h_

#include <vcl_vector.h>
#include "boxm_neighb_graph_node.h"

template <class T_loc, class T_data, class T_aux>
class boxm_cell_vis_graph_node
{
public:
  boxm_cell_vis_graph_node(){}
  ~boxm_cell_vis_graph_node(){}
  //bool visible() { return visible; }
  unsigned int incount;

  
  bool visible;
private:
  //: the number of incoming edges to this node
  int in_count;

  //: graph links to the outgoing blocks
  vcl_vector<T*> out_links_;

  vcl_vector<T*>  neighbors_;
};
#define BOXM_CELL_VIS_GRAPH_NODE_INSTANTIATE(T) \
template boxm_cell_vis_graph_node<T>; 

#endif