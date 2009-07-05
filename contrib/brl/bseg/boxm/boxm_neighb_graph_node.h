#ifndef boxm_neighb_graph_node_h_
#define boxm_neighb_graph_node_h_

#include <vcl_vector.h>
#include <vcl_map.h>
#include <boct/boct_tree_cell.h>

#include "boxm_neighb_graph_node.h"
#include <vgl/vgl_point_3d.h>

class boxm_neighb_graph_node
{
 public:
  boxm_neighb_graph_node() {}

  ~boxm_neighb_graph_node() {}

  void insert(boct_face_idx face, boct_tree_cell<short,vgl_point_3d<double> >* cell)
  { neighbors_[face].push_back(cell); }

  vcl_vector<boct_tree_cell<short,vgl_point_3d<double> >*> neighbors(boct_face_idx face)
  { return neighbors_[face]; }

  int num_neighbors() const { return neighbors_.size(); }

 private:
  vcl_map<boct_face_idx, vcl_vector<boct_tree_cell<short,vgl_point_3d<double> >*> > neighbors_;
};

#endif
