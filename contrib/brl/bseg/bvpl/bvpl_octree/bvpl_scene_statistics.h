// This is brl/bseg/bvpl/bvpl_octree/bvpl_scene_statistics.h
#ifndef bvpl_scene_statistics_h
#define bvpl_scene_statistics_h
//:
// \file
// \brief A set of functions to provide statistics about response scenes
// \author Isabel Restrepo mir@lems.brown.edu
// \date  22-Sep-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bsta/bsta_histogram.h>
#include <boxm/boxm_scene.h>

bool compute_scene_statistics(boxm_scene< boct_tree<short, vnl_vector_fixed<float,3> > > *scene, bsta_histogram<float> &response_hist );


template <class T_loc, class T_data>
bool compute_scene_statistics(boxm_scene<boct_tree<T_loc, T_data > >* scene, bsta_histogram<float>& response_hist )//, bsta_histogram<float>& level_hist, unsigned& n_leaves)
{
  typedef boct_tree<T_loc, T_data> tree_type;
  typedef boct_tree_cell<T_loc,T_data> cell_type;
  
  //(1) Traverse the leaves of the scene
  boxm_cell_iterator<tree_type > iterator = scene->cell_iterator(&boxm_scene<tree_type>::load_block, true);
  
  iterator.begin();
  float cell_count = 0;
  T_data max = (*iterator)->data();
  T_data min = max;
  T_data this_val = max;
  while (!iterator.end()) {
    cell_count++;
    this_val = (*iterator)->data();
    if ( this_val > max)  max = this_val;
    if ( this_val < min)  min = this_val;
    ++iterator;
  }
  
  unsigned nbins = vcl_floor(vcl_sqrt(cell_count));
  response_hist = bsta_histogram<float>(min, max, nbins);
  scene->unload_active_blocks();
  iterator.begin();
  while (!iterator.end()) {
    response_hist.upcount(static_cast<T_data>((*iterator)->data()), 1.0f);
    ++iterator;
  }
  
  scene->unload_active_blocks();
  
  return true;
  
}

//: Function that compute average value of a fraction of samples in the specified block. 
//  Datatype is assumed to be float, but if could be templated to expand to other types
double bvpl_average_value(boxm_scene_base_sptr scene_base, int block_i, int block_j, int block_k, unsigned long tree_nsamples);

//: Function that compute average value of all positive samples in the specified block. 
//  Datatype is assumed to be float, but if could be templated to expand to other types
double bvpl_average_value(boxm_scene_base_sptr scene_base, int block_i, int block_j, int block_k);






#endif // bvpl_scene_statistics_h
