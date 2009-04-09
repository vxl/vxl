#ifndef boxm_block_vis_graph_iterator_txx_
#define boxm_block_vis_graph_iterator_txx_

#define <boct/boct_tree_cell.h>

template <class T>
boxm_block_vis_graph_iterator::boxm_block_vis_graph_iterator(vpgl_camera_double_sptr cam, 
                                                             boxm_scene<T>* scene, 
                                                             bool rev_iter, 
                                                             unsigned img_ni, 
                                                             unsigned img_nj)
    : camera_(cam), scene_(scene), reverse_iter_(iter)
{
  // compute the visibility graph
  boxm_block_iterator<T> iter = scene->iterator();
  for (; !iter.end(); iter++) {
    boxm_block<tree_type> block = *iter;
    if (is_visible(block.bounding_box(), cam, img_ni, img_nj))
      vis_graph_.insert(vcl_make_pair<vgl_point_3d<int>,boxm_visibility_graph_node>(iter.index(),boxm_visibility_graph_node()));
  }
  
  vcl_map<vgl_point_3d<int>, boxm_visibility_graph_node<T> >::iterator vis_iter;
  for (vis_iter=vis_graph_.begin(); vis_iter!=vis_graph_.end(); vis_iter++) {
    boct_face_idx vis_faces = visible_faces(scene->get_block(vis_iter->first).bounding_box(), cam);
    if (vis_faces & boct_cell_face::X_HIGH) {
      // check for neighbor on X_HIGH face
      vgl_point_3d<int> neighbor_idx = vis_iter->first;
      if (reverse_it_) 
        neighbor_idx += vgl_vector_3d<int>(-1,0,0);
      else 
        neighbor_idx += vgl_vector_3d<int>(1,0,0);
        
    } else if (vis_faces & psm_cube_face::X_LOW) {
      // check for neighbor on X_LOW face
      vgl_point_3d<int> neighbor_idx = vis_iter->first;
      if (reverse_it_) {
        neighbor_idx += vgl_vector_3d<int>(1,0,0);
      else 
        neighbor_idx += vgl_vector_3d<int>(-1,0,0);
    } 
    
    psm_block_vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
    if (nit == vis_graph_.end()) {
      vis_iter->second.incoming_count++;
      curr_blocks_.push_back(vis_iter);
    } else {
      vis_iter->second.incoming_count++;
      nit->second.outgoing_links.push_back(vis_iter);
    }
    
    if (vis_faces & psm_cube_face::Y_HIGH) {
      // check for neighbor on Y_HIGH face
      vgl_point_3d<int> neighbor_idx = vis_iter->first;
      if (reverse_it_)
        neighbor_idx += vgl_vector_3d<int>(0,-1,0);
      else 
        neighbor_idx += vgl_vector_3d<int>(0,1,0);
    }else if (vis_faces & psm_cube_face::Y_LOW) {
      // check for neighbor on Y_LOW face
      vgl_point_3d<int> neighbor_idx = vis_iter->first;
      if (reverse_it_) 
        neighbor_idx += vgl_vector_3d<int>(0,1,0);
      else 
        neighbor_idx += vgl_vector_3d<int>(0,-1,0);
      }
      psm_block_vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
      if (nit == vis_graph_.end()) {
        vis_iter->second.incoming_count++;
        curr_blocks_.push_back(vis_iter);
      } else {
        vis_iter->second.incoming_count++;
        nit->second.outgoing_links.push_back(vis_iter);
      }
    }
    
    if (vis_faces & psm_cube_face::Z_HIGH) {
      // check for neighbor on Z_HIGH face
      vgl_point_3d<int> neighbor_idx = vis_iter->first;
      if (reverse_it_)
        neighbor_idx += vgl_vector_3d<int>(0,0,-1);
      else 
        neighbor_idx += vgl_vector_3d<int>(0,0,1);
    }else if (vis_faces & psm_cube_face::Z_LOW) {
      // check for neighbor on Y_LOW face
      vgl_point_3d<int> neighbor_idx = vis_iter->first;
      if (reverse_it_) 
        neighbor_idx += vgl_vector_3d<int>(0,0,1);
      else 
        neighbor_idx += vgl_vector_3d<int>(0,0,-1);
      }
      
      psm_block_vis_graph_type::iterator nit = vis_graph_.find(neighbor_idx);
      if (nit == vis_graph_.end()) {
        vis_iter->second.incoming_count++;
        curr_blocks_.push_back(vis_iter);
      } else {
        vis_iter->second.incoming_count++;
        nit->second.outgoing_links.push_back(vis_iter);
      }
    }
  }
}

template <class T>
bool next()
{
 return false;
}

#define BOXM_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(T) \
template boxm_block_vis_graph_iterator<T>; 

#endif