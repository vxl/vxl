#ifndef boxm_block_vis_graph_iterator_txx_
#define boxm_block_vis_graph_iterator_txx_

#include "boxm_block_vis_graph_iterator.h"
#include "boxm_block_vis_graph_node.h"
#include "boxm_utils.h"
#include <boct/boct_tree_cell.h>

template <class T>
boxm_block_vis_graph_iterator<T>::boxm_block_vis_graph_iterator(vpgl_camera_double_sptr cam,
                                                                boxm_scene<T>* scene,
                                                                unsigned img_ni,
                                                                unsigned img_nj,
                                                                bool rev_iter)
    : camera_(cam), scene_(scene), reverse_it_(rev_iter)
{
  // compute the visibility graph
  boxm_block_iterator<T> iter = scene->iterator();
  for (; !iter.end(); iter++) {
    boxm_block<T>* block = *iter;
    if (boxm_utils::is_visible(block->bounding_box(), cam, img_ni, img_nj))
      vis_graph_.insert(vcl_make_pair<vgl_point_3d<int>,boxm_block_vis_graph_node<T> >(iter.index(),boxm_block_vis_graph_node<T>()));
  }

  typename vis_graph_type::iterator vis_iter;
  for (vis_iter=vis_graph_.begin(); vis_iter!=vis_graph_.end(); vis_iter++) {
    vgl_point_3d<int> idx = vis_iter->first;
    boxm_block<T>* block = scene->get_block(idx);
    boct_face_idx vis_faces = boxm_utils::visible_faces(block->bounding_box(), cam);

    vgl_point_3d<int> neighbor_idx = vis_iter->first;
    bool face_in = false;
    if (vis_faces & X_HIGH) {
      // check for neighbor on X_HIGH face
      if (reverse_it_)
        neighbor_idx += vgl_vector_3d<int>(-1,0,0);
      else
        neighbor_idx += vgl_vector_3d<int>(1,0,0);
      face_in = true;
    } else if (vis_faces & X_LOW) {
      // check for neighbor on X_LOW face
      if (reverse_it_)
        neighbor_idx += vgl_vector_3d<int>(1,0,0);
      else
        neighbor_idx += vgl_vector_3d<int>(-1,0,0);
      face_in = true;
    }

    if (face_in) {
      typename vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
      if (nit == vis_graph_.end()) {
        vis_iter->second.in_count++;
        curr_blocks_.push_back(vis_iter);
      } else {
        vis_iter->second.in_count++;
        nit->second.out_links.push_back(vis_iter);
      }
    }

    face_in = false;
    neighbor_idx = vis_iter->first;
    if (vis_faces & Y_HIGH) {
      // check for neighbor on Y_HIGH face
      if (reverse_it_)
        neighbor_idx += vgl_vector_3d<int>(0,-1,0);
      else
        neighbor_idx += vgl_vector_3d<int>(0,1,0);
      face_in = true;
    } else if (vis_faces & Y_LOW) {
      // check for neighbor on Y_LOW face
      if (reverse_it_)
        neighbor_idx += vgl_vector_3d<int>(0,1,0);
      else
        neighbor_idx += vgl_vector_3d<int>(0,-1,0);
      face_in = true;
    }

    if (face_in) {
      typename vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
      if (nit == vis_graph_.end()) {
        vis_iter->second.in_count++;
        curr_blocks_.push_back(vis_iter);
      } else {
        vis_iter->second.in_count++;
        nit->second.out_links.push_back(vis_iter);
      }
    }

    face_in=false;
    neighbor_idx = vis_iter->first;
    if (vis_faces & Z_HIGH) {
      // check for neighbor on Z_HIGH face
      if (reverse_it_)
        neighbor_idx += vgl_vector_3d<int>(0,0,-1);
      else
        neighbor_idx += vgl_vector_3d<int>(0,0,1);
      face_in = true;
    }else if (vis_faces & Z_LOW) {
      // check for neighbor on Y_LOW face
      if (reverse_it_)
        neighbor_idx += vgl_vector_3d<int>(0,0,1);
      else
        neighbor_idx += vgl_vector_3d<int>(0,0,-1);
      face_in = true;
     }

    if (face_in) {
      typename vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
      if (nit == vis_graph_.end()) {
        vis_iter->second.in_count++;
        curr_blocks_.push_back(vis_iter);
      }
      else {
        vis_iter->second.in_count++;
        nit->second.out_links.push_back(vis_iter);
      }
    }
  }
}

template <class T>
bool boxm_block_vis_graph_iterator<T>::next()
{
  typename vcl_vector<typename vis_graph_type::iterator>::iterator block_iter = curr_blocks_.begin();
  vcl_vector<typename vis_graph_type::iterator> to_process;

  for (; block_iter != curr_blocks_.end(); block_iter++) {
    //vcl_cout << (*block_iter)->first;
    //vcl_cout << (*block_iter)->second.in_count << vcl_endl;
    if (--((*block_iter)->second.in_count) == 0)
      to_process.push_back(*block_iter);
  }

  if (to_process.size() == 0) {
    to_process_indices_.resize(0);
    return false;
  }
  to_process_indices_.resize(to_process.size());

  vcl_vector<vgl_point_3d<int> >::iterator to_proc_it = to_process_indices_.begin();
  for (block_iter = to_process.begin(); block_iter != to_process.end(); ++block_iter, ++to_proc_it) {
    *to_proc_it = (*block_iter)->first;
  }

  // add linked blocks to list for next iteration
  curr_blocks_.clear();
  for (block_iter = to_process.begin(); block_iter != to_process.end(); ++block_iter) {
    vcl_vector<typename vis_graph_type::iterator > &links = (*block_iter)->second.out_links;
    typename vcl_vector<typename vis_graph_type::iterator >::iterator neighbor_it = links.begin();
    for (; neighbor_it != links.end(); ++neighbor_it) {
      curr_blocks_.push_back(*neighbor_it);
    }
  }
  return true;
}

template <class T>
vcl_vector<boxm_block<T>*> boxm_block_vis_graph_iterator<T>::frontier_blocks()
{
  vcl_vector<boxm_block<T>*> frontier;

  vcl_vector<vgl_point_3d<int> > blocks = to_process_indices_;
  for (unsigned i=0; i<blocks.size(); i++) {
    vgl_point_3d<int> index = blocks[i];
    frontier.push_back(scene_->get_block(index)); //.x(), index.y(), index.z());
  }

  return frontier;
}

#define BOXM_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(T) \
template class boxm_block_vis_graph_iterator<T >

#endif
