#ifndef boxm_cell_vis_graph_node_h_
#define boxm_cell_vis_graph_node_h_

#include <vcl_vector.h>
#include <boct/boct_tree_cell.h>

#include "boxm_neighb_graph_node.h"

class boxm_cell_vis_graph_node
{
public:
  boxm_cell_vis_graph_node(){}
  ~boxm_cell_vis_graph_node(){}
  bool visible() { return false; }
  void inc_in_count() { in_count_++; }
  void dec_in_count() { in_count_--; }
  int in_count() {return in_count; }
  void insert_out_link(boct_tree_cell* link) { out_links_.push_back(link); }
  void clear_outlinks() { out_links_.empty(); }
  
private:
  //: the number of incoming edges to this node
  int in_count;

  //: graph links to the outgoing blocks
  vcl_vector<boct_tree_cell*> out_links_;

  boxm_neighb_graph_node neighbors_;
};

#endif