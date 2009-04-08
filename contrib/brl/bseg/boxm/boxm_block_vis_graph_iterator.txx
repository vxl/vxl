#ifndef boxm_block_vis_graph_iterator_txx_
#define boxm_block_vis_graph_iterator_txxx_


template <class T>
boxm_block_vis_graph_iterator::boxm_block_vis_graph_iterator(vpgl_camera_double_sptr cam, 
                                                             boxm_scene<T>* scene, 
                                                             bool rev_iter, 
                                                             unsigned img_ni, 
                                                             unsigned img_nj)
    : camera_(cam), scene_(scene), reverse_iter_(iter)
{
  // compute the visibility graph
  int scene_x, scene_y, scene_z;
  scene->block_num(scene_x, scene_y, scene_z);
  
  for (unsigned i=0; i<scene_x(); i++) {
    for (unsigned j=0; i<scene_y(); j++) {
      for (unsigned k=0; i<scene_z(); k++) {
         boxm_block<T>* block = scene->get_block(i,j,k);
         
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