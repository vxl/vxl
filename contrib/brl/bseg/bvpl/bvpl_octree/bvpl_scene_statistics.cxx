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


double bvpl_average_value(boxm_scene_base_sptr scene_base, int block_i, int block_j, int block_k, unsigned long tree_nsamples)
{
  
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;
  boxm_scene<float_tree_type> * scene = dynamic_cast<boxm_scene<float_tree_type>* > (scene_base.as_pointer());
  
  scene->load_block(block_i, block_j, block_k); 
  
  //get the leaves
  float_tree_type* tree = scene->get_block(block_i, block_j, block_k)->get_tree();
  vcl_vector<float_cell_type*> leaves = tree->leaf_cells();
  
  float tree_ncells = leaves.size();
  //unsigned long tree_nsamples = (tree_ncells/scene_ncells)*(float)nsamples_;
  
  double avg_val = 0.0;
  vnl_random rng(9667566ul);
  for(unsigned i=0; i<tree_nsamples; i++)
  {    
    unsigned long sample = rng.lrand32(tree_ncells-1);
    
    boct_tree_cell<short, float> *center_cell = leaves[sample];
    vgl_point_3d<double> center_cell_centroid = tree->global_centroid(center_cell);
    
    //if neighborhood is not inclusive we would have missing features
    if((double)center_cell->data()< -0.5){  
      i--;
      continue;
    }
    else {
      avg_val+=(double)center_cell->data();    
    }
    
  }    
  
  return avg_val/tree_nsamples;
}

double bvpl_average_value(boxm_scene_base_sptr scene_base, int block_i, int block_j, int block_k)
{
  
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;
  boxm_scene<float_tree_type> * scene = dynamic_cast<boxm_scene<float_tree_type>* > (scene_base.as_pointer());
  
  scene->load_block(block_i, block_j, block_k); 
  
  //get the leaves
  float_tree_type* tree = scene->get_block(block_i, block_j, block_k)->get_tree();
  vcl_vector<float_cell_type*> leaves = tree->leaf_cells();
  
  double actual_samples = 0.0;
  double avg_val = 0.0;
  for(unsigned i=0; i<leaves.size(); i++)
  {    
   
    boct_tree_cell<short, float> *center_cell = leaves[i];
    vgl_point_3d<double> center_cell_centroid = tree->global_centroid(center_cell);
    
    //if neighborhood is not inclusive we would have missing features
    if((double)center_cell->data()< -0.5){  
      continue;
    }
    else {
      avg_val+=(double)center_cell->data();    
    }
    actual_samples = actual_samples +1.0;
  }    
  
  vcl_cout << "Adding errors of : " << actual_samples << " samples" << vcl_endl;
  
  return avg_val/actual_samples;
}
