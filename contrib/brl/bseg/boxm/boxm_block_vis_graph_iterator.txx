#ifndef boxm_block_vis_graph_iterator_txx_
#define boxm_block_vis_graph_iterator_txxx_


template <class T>
boxm_block_vis_graph_iterator::boxm_block_vis_graph_iterator(vpgl_camera_double_sptr cam, boxm_scene<T>* scene, bool rev_iter)
    : camera_(cam), scene_(scene), reverse_iter_(iter)
{
  // compute the visibility graph
}

template <class T>
bool next()
{
 return false;
}

#define BOXM_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(T) \
template boxm_block_vis_graph_iterator<T>; 

#endif