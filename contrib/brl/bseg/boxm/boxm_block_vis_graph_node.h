#ifndef boxm_block_vis_graph_node_h_
#define boxm_block_vis_graph_node_h_

#include <vcl_vector.h>

class boxm_block_vis_graph_node
{
public:
  boxm_block_vis_graph_node();
  ~boxm_block_vis_graph_node();

private:
  //: the number of incoming edges to this node
  int in_count;

  //: graph links to the outgoing blocks
  vcl_vector<boxm_block*> out_links_;
};

#define BOXM_BLOCK_VIS_GRAPH_NODE_INSTANTIATE(T) \
template boxm_block_vis_graph_node<T>; 

#endif