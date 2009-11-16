#ifndef boxm_cell_vis_graph_iterator_txx_
#define boxm_cell_vis_graph_iterator_txx_

#include "boxm_cell_vis_graph_iterator.h"
#include "boxm_utils.h"

template <class T_loc,class T_data>
boxm_cell_vis_graph_iterator<T_loc,T_data>::boxm_cell_vis_graph_iterator(vpgl_camera_double_sptr cam,
                                                                         boct_tree<T_loc,T_data>* tree,
                                                                         unsigned img_ni,
                                                                         unsigned img_nj) : camera_(cam),img_ni_(img_ni), img_nj_(img_nj)
{
  if (!tree)
  {
   vcl_cout<<"No Tree found"<<vcl_endl;
  }
  else
  {
    list_of_vis_nodes_.clear();
    // check if each node is visible or notr
    check_cell_visibility(tree,tree->root());
    // compute the visibility graph
    for (unsigned i=0;i<list_of_vis_nodes_.size();i++)
    {
      boct_face_idx vis_faces = boxm_utils::visible_faces(tree->cell_bounding_box(list_of_vis_nodes_[i]), cam);
      if (vis_faces & ::X_HIGH) {
        form_graph_per_cell(list_of_vis_nodes_[i],X_HIGH,tree);
      }
      else if (vis_faces & ::X_LOW) {
        form_graph_per_cell(list_of_vis_nodes_[i],X_LOW,tree);
      }
      if (vis_faces & ::Y_HIGH) {
        form_graph_per_cell(list_of_vis_nodes_[i],Y_HIGH,tree);
      }
      else if (vis_faces & ::Y_LOW) {
        form_graph_per_cell(list_of_vis_nodes_[i],Y_LOW,tree);
      }
      if (vis_faces & ::Z_HIGH) {
        form_graph_per_cell(list_of_vis_nodes_[i],Z_HIGH,tree);
      }
      else if (vis_faces & ::Z_LOW) {
        form_graph_per_cell(list_of_vis_nodes_[i],Z_LOW,tree);
      }
    }
    list_of_vis_nodes_.clear();
  }
}

template <class T_loc,class T_data>
bool boxm_cell_vis_graph_iterator<T_loc,T_data>::next()
{
  vcl_vector<boct_tree_cell<T_loc,T_data>* > to_process;
  vcl_vector<boct_tree_cell<T_loc,T_data>* > to_process_refine;

  typename vcl_vector<boct_tree_cell<T_loc,T_data>* >::iterator cell_it = curr_cells_.begin();
  for (; cell_it != curr_cells_.end(); ++cell_it) {
    // decrement each cell's count in list. if count == 0, add to list of nodes to process
    if (--((*cell_it)->vis_node()->incoming_count) == 0) {
      to_process.push_back(*cell_it);
    }
  }
  if (to_process.size() == 0) {
    return false;
  }
   frontier_.clear();

  for (cell_it = to_process.begin(); cell_it != to_process.end(); ++cell_it) {
    frontier_.push_back((*cell_it));
  }
  // add linked cells to list for next iteration
  curr_cells_.clear();
  for (cell_it = to_process.begin(); cell_it != to_process.end(); ++cell_it) {
    vcl_vector<boct_tree_cell<T_loc,T_data>* > links = (*cell_it)->vis_node()->outgoing_links;
    typename vcl_vector<boct_tree_cell<T_loc,T_data>* >::iterator neighbor_it = links.begin();
    for (; neighbor_it != links.end(); ++neighbor_it) {
      curr_cells_.push_back(*neighbor_it);
    }
  }
  return true;
}

template<class T_loc,class T_data>
void boxm_cell_vis_graph_iterator<T_loc,T_data>::check_cell_visibility(boct_tree<T_loc,T_data>* tree,
                                                                       boct_tree_cell<T_loc,T_data>* tree_cell)
{
  vgl_box_3d<double> bbox=tree->cell_bounding_box(tree_cell);
  if (boxm_utils::is_visible(bbox, camera_,img_ni_, img_nj_, true))
  {
    // base case for recursion
    if (tree_cell->is_leaf())
    {
      boct_cell_vis_graph_node<T_loc,T_data> * node=new boct_cell_vis_graph_node<T_loc,T_data>();
      node->visible=true;
      tree_cell->set_vis_node(node);
      list_of_vis_nodes_.push_back(tree_cell);
    }
    else
    {
      boct_tree_cell<T_loc,T_data>* children = tree_cell->children();
      for (unsigned i=0; i<8; i++)
      check_cell_visibility(tree,&children[i]);
    }
  }
}

template<class T_loc,class T_data>
void boxm_cell_vis_graph_iterator<T_loc,T_data>::form_graph_per_cell(boct_tree_cell<T_loc,T_data>* cell,boct_face_idx face_idx,boct_tree<T_loc,T_data>* tree)
{
  boct_cell_vis_graph_node<T_loc,T_data>* vis_node=cell->vis_node();
  vcl_vector<boct_tree_cell<T_loc,T_data>*> neighbors;
  cell->find_neighbors(face_idx, neighbors,tree->root_level());
  typename vcl_vector<boct_tree_cell<T_loc,T_data>*>::iterator neighbor_it = neighbors.begin();
  bool visible_neighbors=false;
  for (; neighbor_it != neighbors.end(); ++neighbor_it) {
    boct_cell_vis_graph_node<T_loc,T_data>* neighbor_vis_node=(*neighbor_it)->vis_node();
    if (neighbor_vis_node) {
      vis_node->incoming_count++;
      neighbor_vis_node->outgoing_links.push_back(cell);
      visible_neighbors=true;
    }
  }
  if (!visible_neighbors) {
    vis_node->incoming_count++;
    curr_cells_.push_back(cell);
  }
}

#define BOXM_CELL_VIS_GRAPH_ITERATOR_INSTANTIATE(T1,T2) \
template class boxm_cell_vis_graph_iterator<T1,T2 >

#endif // boxm_cell_vis_graph_iterator_txx_
