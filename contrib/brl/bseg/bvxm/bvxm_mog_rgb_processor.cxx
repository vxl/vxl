#include "bvxm_mog_rgb_processor.h"
//:
// \file
#include "grid/bvxm_voxel_slab.h"
#include "grid/bvxm_voxel_slab.hxx"
#include "grid/bvxm_voxel_slab_iterator.h"
#include <vcl_cassert.h>

//: Return probability density of observing pixel values
bvxm_voxel_slab<float>
bvxm_mog_rgb_processor::prob_density(bvxm_voxel_slab<apm_datatype> const& appear,
                  bvxm_voxel_slab<obs_datatype> const& obs)
{
  //the output
  bvxm_voxel_slab<float> probabilities(appear.nx(), appear.ny(), appear.nz());

  //the slab iterators
  bvxm_voxel_slab<apm_datatype>::const_iterator appear_it;
  bvxm_voxel_slab<obs_datatype>::const_iterator obs_it = obs.begin();
  bvxm_voxel_slab<float>::iterator prob_it = probabilities.begin();

  for (appear_it = appear.begin(); appear_it!= appear.end(); ++appear_it, ++obs_it, ++prob_it)
  {
    if ((*appear_it).num_components() ==0)
      (*prob_it)= 1.00f;
    else
    (*prob_it) = (*appear_it).prob_density(*obs_it);
   }

  return probabilities;
}

//: Return probabilities that pixels are in range [min,max]
bvxm_voxel_slab<float>
bvxm_mog_rgb_processor::prob_range(bvxm_voxel_slab<apm_datatype> const& appear,
                                    bvxm_voxel_slab<obs_datatype> const& obs_min,
                                    bvxm_voxel_slab<obs_datatype> const& obs_max)
{
  //the output
  bvxm_voxel_slab<float> probabilities(appear.nx(), appear.ny(), appear.nz());

  //the slab iterators
  bvxm_voxel_slab<apm_datatype>::const_iterator appear_it;
  bvxm_voxel_slab<obs_datatype>::const_iterator min_it = obs_min.begin();
  bvxm_voxel_slab<obs_datatype>::const_iterator max_it = obs_max.begin();
  bvxm_voxel_slab<float>::iterator prob_it = probabilities.begin();

  for (appear_it = appear.begin(); appear_it!= appear.end(); ++appear_it, ++min_it, ++max_it, ++prob_it)
  {
    if ((*appear_it).num_components() ==0)
      (*prob_it)= 1.00f;
    else
    (*prob_it) = (*appear_it).probability(*min_it,*max_it);
   }

  return probabilities;
}


//: Update with a new sample image
bool bvxm_mog_rgb_processor::update( bvxm_voxel_slab<apm_datatype> &appear,
            bvxm_voxel_slab<obs_datatype> const& obs,
            bvxm_voxel_slab<float> const& weight)
{
  // the model
  float init_variance = 0.008f;
  float min_stddev = 0.02f;
  float g_thresh = 2.5; // number of std devs from mean sample must be
  //bsta_gauss_if3 init_gauss(obs_datatype(0.0f),obs_datatype(0.01f));
  bsta_gaussian_indep<float,3>::covar_type init_covar(init_variance);
  bsta_gauss_if3 init_gauss(obs_datatype(0.0f),init_covar);

  // the updater
  //bsta_mg_grimson_weighted_updater<mix_gauss> updater(init_gauss);
  bsta_mg_grimson_weighted_updater<mix_gauss> updater(init_gauss,this->n_gaussian_modes_,g_thresh,min_stddev);

  //check dimensions match
  assert(appear.nx() == obs.nx());
  assert(appear.ny() == obs.ny());
  assert(appear.nz() == obs.nz());

  //the iterators
  bvxm_voxel_slab<apm_datatype>::iterator appear_it;
  bvxm_voxel_slab<obs_datatype>::const_iterator obs_it = obs.begin();
  bvxm_voxel_slab<float>::const_iterator weight_it = weight.begin();

  for (appear_it = appear.begin(); appear_it!= appear.end(); ++appear_it, ++obs_it, ++weight_it)
  {
    if (*weight_it > 0)
      updater(*appear_it, *obs_it, *weight_it);
  }
  return true;
}


//: Expected value
bvxm_voxel_slab<bvxm_mog_rgb_processor::obs_datatype> bvxm_mog_rgb_processor::expected_color(bvxm_voxel_slab<mix_gauss_type > const& appear)
{
  //the output
   bvxm_voxel_slab<obs_datatype> expected_color(appear.nx(),appear.ny(),appear.nz());

   //the iterator
   bvxm_voxel_slab<apm_datatype>::const_iterator appear_it;
   bvxm_voxel_slab<obs_datatype>::iterator ec_it = expected_color.begin();

  for (appear_it = appear.begin(); appear_it!= appear.end();++appear_it, ++ec_it)
  {
    //obs_datatype total_weight,c;
    //total_weight.fill(0);
    float total_weight= 0;
    obs_datatype c(0.0f);

    //should be components used
    for (unsigned i = 0 ; i< (*appear_it).num_components(); i++)
    {
      total_weight += (*appear_it).weight(i);
      c+= (*appear_it).distribution(i).mean() * (*appear_it).weight(i);
    }
    if (total_weight > 0.0f)
      (*ec_it)= (1/total_weight)*c;
   }

  return expected_color;
}

//: color of the most probable mode in the mixtures in the slab
bvxm_voxel_slab<bvxm_mog_rgb_processor::obs_datatype> bvxm_mog_rgb_processor::most_probable_mode_color(bvxm_voxel_slab<mix_gauss_type > const& appear)
{
  //the output
   bvxm_voxel_slab<obs_datatype> color(appear.nx(),appear.ny(),appear.nz());

   //the iterator
   bvxm_voxel_slab<mix_gauss_type>::const_iterator appear_it;
   bvxm_voxel_slab<obs_datatype>::iterator ec_it = color.begin();

  for (appear_it = appear.begin(); appear_it!= appear.end();++appear_it, ++ec_it)
  {
    if ((*appear_it).num_components() > 0) {
      if ((*appear_it).weight(0) > 0.0f) {
        (*ec_it) = (*appear_it).distribution(0).mean();
      }
    }
  }

  return color;
}

bvxm_voxel_slab<bvxm_mog_rgb_processor::obs_datatype> bvxm_mog_rgb_processor::sample(bvxm_voxel_slab<mix_gauss_type > const& appear)
{
  //the output
   bvxm_voxel_slab<obs_datatype> color(appear.nx(),appear.ny(),appear.nz());

   //the iterator
   bvxm_voxel_slab<mix_gauss_type>::const_iterator appear_it;
   bvxm_voxel_slab<obs_datatype>::iterator ec_it = color.begin();

  vnl_random rng;
  for (appear_it = appear.begin(); appear_it!= appear.end();++appear_it, ++ec_it)
  {
    if ((*appear_it).num_components() > 0) {
      (*ec_it) = (*appear_it).sample(rng);
    }
  }

  return color;
}

