//:
// \file
// \author Isabel Restrepo
// \date 22-Sep-2010

#include "bvpl_scene_statistics.h"


double bvpl_average_value(boxm_scene_base_sptr scene_base, int block_i, int block_j, int block_k, unsigned long tree_nsamples)
{
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;
  boxm_scene<float_tree_type> * scene = dynamic_cast<boxm_scene<float_tree_type>* > (scene_base.as_pointer());

  scene->load_block(block_i, block_j, block_k);

  //get the leaves
  float_tree_type* tree = scene->get_block(block_i, block_j, block_k)->get_tree();
  vcl_vector<float_cell_type*> leaves = tree->leaf_cells();

  int tree_ncells = leaves.size();
  //unsigned long tree_nsamples = (tree_ncells/scene_ncells)*(float)nsamples_;

  double avg_val = 0.0;
  vnl_random rng(9667566ul);
  for (unsigned i=0; i<tree_nsamples; i++)
  {
    unsigned long sample = rng.lrand32(tree_ncells-1);

    boct_tree_cell<short, float> *center_cell = leaves[sample];
    // vgl_point_3d<double> center_cell_centroid = tree->global_centroid(center_cell);

    //if neighborhood is not inclusive we would have missing features
    if ((double)center_cell->data()< -0.5) {
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
  if (!scene) {
    vcl_cerr << "Error in bvpl_average_value: Error scene is of incorrect type\n";
    return false;
  }
  scene->load_block(block_i, block_j, block_k);

  //get the leaves
  float_tree_type* tree = scene->get_block(block_i, block_j, block_k)->get_tree();
  vcl_vector<float_cell_type*> leaves = tree->leaf_cells();

  double actual_samples = 0.0;
  double avg_val = 0.0;
  for (unsigned i=0; i<leaves.size(); i++)
  {
    boct_tree_cell<short, float> *center_cell = leaves[i];
    // vgl_point_3d<double> center_cell_centroid = tree->global_centroid(center_cell);

    //if neighborhood is not inclusive we would have missing features
    if ((double)center_cell->data()< -0.5) {
      continue;
    }
    else {
      avg_val+=(double)center_cell->data();
      actual_samples = actual_samples +1.0;
    }
  }

  vcl_cout << "Adding errors of : " << actual_samples << " samples" << vcl_endl;

  return avg_val/actual_samples;
}
