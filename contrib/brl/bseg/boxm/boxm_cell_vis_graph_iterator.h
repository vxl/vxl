#ifndef boxm_cell_vis_graph_iterator_h_
#define boxm_cell_vis_graph_iterator_h_

#include <vcl_vector.h>

template <class T>
class boxm_cell_vis_graph_iterator
{
public:
  boxm_cell_vis_graph_iterator(vpgl_camera_double_sptr cam, boxm_block<T>* block, bool rev_iter);

  ~boxm_cell_vis_graph_iterator(){}

  bool next();

  vcl_vector<boxm_tree_cell*> frontier() { return frontier_; }

  void set_reverse_iter(bool reverse) { reverse_iter_ = reverse; }

private:
  //: the camera
  vpgl_camera_double_sptr camera_;

  //: blocks that can be seen by the camera unocluded
  vcl_vector<boct_tree_cell*> frontier_;

  boxm_block<T>* block_;

  bool reverse_iter_;

};

#endif