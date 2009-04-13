#ifndef boxm_cell_vis_graph_iterator_txx_
#define boxm_cell_vis_graph_iterator_txx_

template <T_loc,T_data>
boxm_cell_vis_graph_iterator::boxm_cell_vis_graph_iterator(vpgl_camera_double_sptr cam,
                                                           boct_tree<T_loc,T_data>* tree,
                                                           bool rev_iter,
                                                           unsigned img_ni,
                                                           unsigned img_nj)
    : camera_(cam), reverse_iter_(iter)
{
  // compute the visibility graph
}

template <class T>
bool next()
{
 return false;
}

#define BOXM_cell_VIS_GRAPH_ITERATOR_INSTANTIATE(T) \
template boxm_cell_vis_graph_iterator<T >

#endif // boxm_cell_vis_graph_iterator_txx_
