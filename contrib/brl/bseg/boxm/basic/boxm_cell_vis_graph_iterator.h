#ifndef boxm_cell_vis_graph_iterator_h_
#define boxm_cell_vis_graph_iterator_h_
//:
// \file
#include <iostream>
#include <vector>
#include <boct/boct_tree_cell.h>
#include <boct/boct_tree.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T_loc, class T_data>
class boxm_cell_vis_graph_iterator
{
 public:
  boxm_cell_vis_graph_iterator(vpgl_camera_double_sptr cam,
                               boct_tree<T_loc,T_data>* tree,
                               unsigned img_ni,
                               unsigned img_nj);

  ~boxm_cell_vis_graph_iterator() = default;

  bool next();

  std::vector<boct_tree_cell<T_loc,T_data>*> frontier() { return frontier_; }

  void set_reverse_iter(bool reverse) { reverse_iter_ = reverse; }

 private:
  //: the camera
  vpgl_camera_double_sptr camera_;

  //: blocks that can be seen by the camera unoccluded
  std::vector<boct_tree_cell<T_loc,T_data>*> frontier_;
  std::vector<boct_tree_cell<T_loc,T_data>*> curr_cells_;
  bool reverse_iter_;

  unsigned img_ni_;
  unsigned img_nj_;

  //: recursive function
  void check_cell_visibility(boct_tree<T_loc,T_data>* tree,
                             boct_tree_cell<T_loc,T_data>* tree_cell);

  void form_graph_per_cell(boct_tree_cell<T_loc,T_data>* cell,
                           boct_face_idx face_idx,
                           boct_tree<T_loc,T_data>* tree);

  std::vector<boct_tree_cell<T_loc,T_data>* > list_of_vis_nodes_;
};

#endif
