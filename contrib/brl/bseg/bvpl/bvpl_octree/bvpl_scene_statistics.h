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

template <class T_loc, class T_data>
bool compute_scene_statistics(boxm_scene<boct_tree<T_loc, T_data > >& scene, bsta_histogram<float>& response_hist )//, bsta_histogram<float>& level_hist, unsigned& n_leaves)
{
  typedef boct_tree<T_loc, T_data> tree_type;
  typedef boct_tree_cell<T_loc,T_data> cell_type;


  //(1) Traverse the leaves of the scene
  boxm_cell_iterator<boct_tree<short, T_data > > iterator =
  scene.cell_iterator(&boxm_scene<boct_tree<short, T_data> >::load_block);

  iterator.begin();
  float cell_count = 0;
  while (!iterator.end()) {
    cell_count++;
  }

  float nbins = vcl_sqrt(cell_count);
  response_hist = bsta_histogram<float>(0.0f, 1.0f, nbins);
  //level_hist = bsta_histogram<float>(0.0f, 10.0f, 10);

  iterator.begin();

  while (!iterator.end()) {
    boct_tree_cell<short,T_data> *cell = *iterator;
    //level_hist.upcount(static_cast<float>(level));
    response_hist.upcount(static_cast<float>(cell->data().mean()), 1.0f);
  }

  return true;
}

#endif // bvpl_scene_statistics_h
