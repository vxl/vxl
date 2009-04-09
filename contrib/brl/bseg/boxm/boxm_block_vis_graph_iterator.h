#ifndef boxm_block_vis_graph_iterator_h_
#define boxm_block_vis_graph_iterator_h_

#include "boxm_block_vis_graph_node.h"

#include <vcl_vector.h>

template <class T>
class boxm_block_vis_graph_iterator
{
public:
  boxm_block_vis_graph_iterator(vpgl_camera_double_sptr cam, boxm_scene<T>* scene, bool rev_iter,
    unsigned img_ni, unsigned img_nj);

  ~boxm_block_vis_graph_iterator(){}

  bool next();

  vcl_vector<boxm_block*> frontier() { return frontier_; }

  void set_reverse_iter(bool reverse) { reverse_iter_ = reverse; }

private:

  // visibility graph, indexed by the block indices
  vcl_map<vgl_point_3d<unsigned>, boxm_visibility_graph_node<T> > vis_graph_;

  //: the camera
  vpgl_camera_double_sptr camera_;

  //: blocks that can be seen by the camera unoccluded
  vcl_vector<boxm_block*> frontier_;

  boxm_scene<T>* scene_;

  bool reverse_iter_;

};

#endif