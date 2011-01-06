//:
// \file
// \author Isabel Restrepo
// \date 22-Sep-2010

#include "bvpl_scene_statistics.h"

bool compute_scene_statistics(boxm_scene< boct_tree<short, vnl_vector_fixed<float,3> > > *scene, bsta_histogram<float> &response_hist )
{
  typedef boct_tree<short, vnl_vector_fixed<float,3> > f3_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<float,3> > cell_type;


  //(1) Traverse the leaves of the scene
  boxm_cell_iterator<f3_tree_type > iterator = scene->cell_iterator(&boxm_scene<f3_tree_type>::load_block, true);

  iterator.begin();
  float cell_count = 0;
  float max = (*iterator)->data().magnitude();
  float min = max;
  float mag = max;
  while (!iterator.end()) {
    cell_count++;
    mag = (*iterator)->data().magnitude();
    if ( mag > max)  max = mag;
    if ( mag < min)  min = mag;
    ++iterator;
  }

  unsigned nbins = vcl_floor(vcl_sqrt(cell_count));
  response_hist = bsta_histogram<float>(min, max, nbins);

  iterator.begin();

  while (!iterator.end()) {
    response_hist.upcount(static_cast<float>((*iterator)->data().magnitude()), 1.0f);
    ++iterator;
  }

  scene->unload_active_blocks();
  
  return true;
}
