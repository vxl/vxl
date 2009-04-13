#ifndef boxm_cell_vis_graph_iterator_txx_
#define boxm_cell_vis_graph_iterator_txx_

#include "boxm_cell_vis_graph_iterator.h"
#include "boxm_utils.h"

template <class T_loc,class T_data, class T_aux>
boxm_cell_vis_graph_iterator<T_loc,T_data,T_aux>::boxm_cell_vis_graph_iterator(vpgl_camera_double_sptr cam,
                                                           boct_tree<T_loc,T_data,T_aux>* tree,
                                                           bool rev_iter,
                                                           unsigned img_ni,
                                                           unsigned img_nj) : camera_(cam), reverse_iter_(rev_iter), img_ni_(img_ni), img_nj_(img_nj)
{
   list_of_vis_nodes_.clear();
   // check if each node is visible or notr
   check_cell_visibility(tree,tree->root());
   // compute the visibility graph
   for(unsigned i=0;i<list_of_vis_nodes_.size();i++)
   {
	    boct_face_idx vis_faces = boxm_utils::visible_faces(tree->cell_bounding_box(list_of_vis_nodes_[i]), cam);
		if (vis_faces & boct_cell_face::X_HIGH) {
			form_graph_per_cell(list_of_vis_nodes_[i],boct_cell_face::X_HIGH,tree);
		}
		else if (vis_faces & boct_cell_face::X_LOW) {
				form_graph_per_cell(list_of_vis_nodes_[i],boct_cell_face::X_LOW,tree);
		}
		if (vis_faces & boct_cell_face::Y_HIGH) {
			form_graph_per_cell(list_of_vis_nodes_[i],boct_cell_face::Y_HIGH,tree);
		}
		else if (vis_faces & boct_cell_face::Y_LOW) {
				form_graph_per_cell(list_of_vis_nodes_[i],boct_cell_face::Y_LOW,tree);
		}
		if (vis_faces & boct_cell_face::Z_HIGH) {
			form_graph_per_cell(list_of_vis_nodes_[i],boct_cell_face::Z_HIGH,tree);
		}
		else if (vis_faces & boct_cell_face::Z_LOW) {
			form_graph_per_cell(list_of_vis_nodes_[i],boct_cell_face::Z_LOW,tree);
		}
	}
    list_of_vis_nodes_.clear();
}

template <class T_loc,class T_data,class T_aux>
bool boxm_cell_vis_graph_iterator<T_loc,T_data,T_aux>::next()
{
  vcl_vector<boct_tree_cell<T_loc,T_data,T_aux>* > to_process;
  vcl_vector<boct_tree_cell<T_loc,T_data,T_aux>* > to_process_refine;

  vcl_vector<boct_tree_cell<T_loc,T_data,T_aux>* >::iterator cell_it = frontier_.begin();
  for (; cell_it != frontier_.end(); ++cell_it) {
    // decrement each cell's count in list. if count == 0, add to list of nodes to process
    if (--((*cell_it)->vis_node()->incoming_count) == 0) {
      to_process.push_back(*cell_it);
    }
  }
  if (to_process.size() == 0) {
    return false;
  }

  // add linked cells to list for next iteration
  frontier_.clear();
  for (cell_it = to_process.begin(); cell_it != to_process.end(); ++cell_it) {
    vcl_vector<boct_tree_cell<T_loc,T_data,T_aux>* > links = (*cell_it)->vis_node()->outgoing_links;
    vcl_vector<boct_tree_cell<T_loc,T_data,T_aux>* >::iterator neighbor_it = links.begin();
    for (; neighbor_it != links.end(); ++neighbor_it) {
      frontier_.push_back(*neighbor_it);
    }
  }
  return true;
}

template<class T_loc,class T_data, class T_aux>
void boxm_cell_vis_graph_iterator<T_loc,T_data,T_aux>::check_cell_visibility(boct_tree<T_loc,T_data,T_aux>* tree,
																			 boct_tree_cell<T_loc,T_data,T_aux>* tree_cell)
{
   vgl_box_3d<double> bbox=tree->cell_bounding_box(tree_cell);
   if(boxm_utils::is_visible(bbox, camera_,img_ni_, img_nj_, true))
	{
	//: base case for recursion
	if(tree_cell->is_leaf())
	{
		boct_cell_vis_graph_node<T_loc,T_data,T_aux> * node=new boct_cell_vis_graph_node<T_loc,T_data,void>();
		node->visible=true;
		tree_cell->set_vis_node(node);
		list_of_vis_nodes_.push_back(tree_cell);
	}
	else
	{
	  boct_tree_cell<T_loc,T_data,T_aux>* children = tree_cell->children();
	  for (unsigned i=0; i<8; i++)
		 check_cell_visibility(tree,&children[i]);
	}
	}
}
template<class T_loc,class T_data, class T_aux>
void boxm_cell_vis_graph_iterator<T_loc,T_data,T_aux>::form_graph_per_cell(boct_tree_cell<T_loc,T_data,T_aux>* cell,boct_face_idx face_idx,boct_tree<T_loc,T_data,T_aux>* tree)
{
		boct_cell_vis_graph_node<T_loc,T_data,T_aux>* vis_node=cell->vis_node();
	    vcl_vector<boct_tree_cell<T_loc,T_data,T_aux>*> neighbors;
	    cell->find_neighbors(face_idx, neighbors,tree->num_levels());
        unsigned int visible_neighbor_count = 0;
        vcl_vector<boct_tree_cell<T_loc,T_data,T_aux>*>::iterator neighbor_it = neighbors.begin();
        bool visible_neighbors=false;
        for (; neighbor_it != neighbors.end(); ++neighbor_it) {
          boct_cell_vis_graph_node<T_loc,T_data,T_aux>* neighbor_vis_node=(*neighbor_it)->vis_node();
          if (neighbor_vis_node) {
            vis_node->incoming_count++;
            neighbor_vis_node->outgoing_links.push_back(cell);
            visible_neighbors=true;
          }
        }
        if (!visible_neighbors) {
          vis_node->incoming_count++;
          frontier_.push_back(cell);
        }
}
#define BOXM_CELL_VIS_GRAPH_ITERATOR_INSTANTIATE(T1,T2,T3) \
template boxm_cell_vis_graph_iterator<T1,T2,T3>;

#endif // boxm_cell_vis_graph_iterator_txx_
