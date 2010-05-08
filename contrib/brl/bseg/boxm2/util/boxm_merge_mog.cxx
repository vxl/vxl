#include "boxm_merge_mog.h"
//:
// \file

#include <vcl_vector.h>
#include <boxm2/boxm_scene.h>
#include <vcl_cassert.h>
#include <vnl/vnl_math.h> // for vnl_math_sqr()

void boxm_merge_mog::kl_merge(mix_gauss_type const& mixture,bsta_gauss_f1 &gaussian)
{
  //When merging components of gaussian mixtures, we need to perform the following calculations
  //1. find the normalizing weight of the new component.
  //   In this case this value is 1 because we are merging all the components
  //2. find the mean
  //3. find the variance

  float mean = 0.0f;
  float var = 0.0f;

  //CAUTION: only mixtures of 3 modes are supported for now.
  assert(mixture.num_components() == 3);
  unsigned num_components = mixture.num_components();

  for (unsigned i = 0; i<num_components; ++i)
  {
    // note: these equations omit the weight of the final distribution
    // only because in this case it is 1
    mean += mixture.weight(i) * mixture.distribution(i).mean();
    var  += mixture.weight(i) *(mixture.distribution(i).var() +
                                vnl_math_sqr(mixture.distribution(i).mean()));
  }
  if (var <  vnl_math_sqr(mean))
    vcl_cout<< "This should not happen: " << mixture << vcl_endl;
  var -= vnl_math_sqr(mean);
  assert(var >= 0);

  gaussian.set_mean(mean);
  gaussian.set_var(var);
  return;
}

//: Merges the components of the gaussian mixtures at each voxel into a single gaussian
//  Thus the resulting grid contains unimodal gaussians
bool boxm_merge_mog::kl_merge_scene(boxm_scene<boct_tree<short, mix_gauss_type> > &mog_scene,
                                    boxm_scene<boct_tree<short, gauss_type> > &gauss_scene)
{
    // iterate through the scene
    boxm_block_iterator<mog_tree_type > mog_iter = mog_scene.iterator();
    boxm_block_iterator<gauss_tree_type > gauss_iter = gauss_scene.iterator();

    for (mog_iter.begin(), gauss_iter.begin(); !mog_iter.end(); ++mog_iter, ++gauss_iter) {
      mog_scene.load_block(mog_iter.index());
      gauss_scene.load_block(gauss_iter.index());
      mog_tree_type  *mog_tree = (*mog_iter)->get_tree();
      gauss_tree_type  *gauss_tree = mog_tree->clone_to_type<gauss_type>();
#if 0
      gauss_tree->init_cells(func_max.min_response());
#endif
      this->kl_merge_octree(mog_tree,gauss_tree);
      (*gauss_iter)->init_tree(gauss_tree);
      gauss_scene.write_active_block();
    }

    return true;
}

//: Merges the components of the gaussian mixtures at each voxel into a single gaussian
//  Thus the resulting grid contains unimodal gaussians
bool boxm_merge_mog::kl_merge_octree(boct_tree<short, mix_gauss_type> *mog_tree,
                                     boct_tree<short, gauss_type> *gauss_tree)
{
  // iterate through the trees
  vcl_vector<mog_cell_type*> mog_cells = mog_tree->all_cells();
  vcl_vector<gauss_cell_type*> gauss_cells = gauss_tree->all_cells();

  if (mog_cells.size()!=gauss_cells.size() ) {
    vcl_cerr << "Different size vectors in boxm_merge_mog::kl_merge_octree\n";
    return false;
  }


  for (unsigned i = 0; i < mog_cells.size(); ++i)
  {
    gauss_type result_gauss;
    this->kl_merge(mog_cells[i]->data(), result_gauss);
    gauss_cells[i]->set_data(result_gauss);
  }
  return true;
}
