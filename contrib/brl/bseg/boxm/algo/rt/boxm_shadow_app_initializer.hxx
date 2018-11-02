#ifndef boxm_shadow_app_initializer_hxx_
#define boxm_shadow_app_initializer_hxx_

#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include "boxm_shadow_app_initializer.h"
#include "boxm_shadow_appearance_estimator.h"

#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_aux_scene.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
boxm_shadow_app_initializer<T_loc,APM,AUX>::boxm_shadow_app_initializer(
            boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
            std::vector<std::string>  image_ids, float min_app_sigma,
            float shadow_prior, float shadow_mean, float shadow_sigma,
            bool verbose)
  : image_ids_(std::move(image_ids)), scene_(scene), min_app_sigma_(min_app_sigma),
    shadow_prior_(shadow_prior), shadow_mean_(shadow_mean),
    shadow_sigma_(shadow_sigma),verbose_(verbose)
{}


template <class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
bool boxm_shadow_app_initializer<T_loc,APM,AUX>::initialize()
{
  // get auxiliary scenes associated with each input image
  typedef typename boxm_aux_traits<AUX>::sample_datatype aux_type;

  typedef boct_tree<T_loc, boxm_sample<APM> > tree_type;

  std::vector<boxm_aux_scene<T_loc,  boxm_sample<APM>, aux_type> > aux_scenes;
  for (unsigned int i=0; i<image_ids_.size(); ++i) {
    boxm_aux_scene<T_loc, boxm_sample<APM>, aux_type> aux_scene(&scene_,image_ids_[i],boxm_aux_scene<T_loc, boxm_sample<APM>, aux_type>::LOAD);
    aux_scenes.push_back(aux_scene);
  }

  std::vector<boxm_rt_sample<typename boxm_apm_traits<APM>::obs_datatype> > aux_samples;

  // for each block
  boxm_block_iterator<tree_type> iter(&scene_);
  iter.begin();
  while (!iter.end())
  {
    scene_.load_block(iter.index());
    boxm_block<tree_type>* block = *iter;
    boct_tree<T_loc, boxm_sample<APM> >* tree = block->get_tree();
    std::vector<boct_tree_cell<T_loc,boxm_sample<APM> >*> cells = tree->leaf_cells();

    // get a vector of incremental readers for each aux scene.
    std::vector<boct_tree_cell_reader<T_loc, aux_type>* > aux_readers(aux_scenes.size());
    for (unsigned int i=0; i<aux_scenes.size(); ++i) {
      aux_readers[i] = aux_scenes[i].get_block_incremental(iter.index());
    }
    // iterate over cells
    for (unsigned i=0; i<cells.size(); ++i)
    {
      aux_samples.clear();
      boct_tree_cell<T_loc,boxm_sample<APM> >* cell = cells[i];
      boxm_sample<APM> data = cell->data();
      //std::cout << "cell IN " << data.alpha << data.appearence_<< std::endl;
      for (unsigned j=0; j<aux_readers.size(); j++) {
        boct_tree_cell<T_loc, aux_type> temp_cell;

        if (!aux_readers[j]->next(temp_cell)) {
          std::cerr << "error: incremental reader returned false.\n";
            return false;
        }
        if (!temp_cell.code_.isequal(&(cell->code_))) {
          std::cerr << "error: temp_cell idx does not match cell idx.\n";
          return false;
        }
        if (temp_cell.data().seg_len_ > 0.0f) {
          aux_samples.push_back(temp_cell.data());
        }
      }
      typedef typename boxm_apm_traits<APM>::obs_datatype obs_type;
      std::vector<obs_type> obs_vector;
      for (unsigned int s=0; s<aux_samples.size(); ++s) {
        float seg_len = aux_samples[s].seg_len_;
        obs_type obs = aux_samples[s].obs_;
        if ( seg_len > 1e-5 && (obs > obs_type(0)) ){
          obs_vector.push_back(obs / seg_len);
        }
      }
      unsigned n_obs = obs_vector.size();
      if(n_obs>1){
      std::vector<float> pre_vector(n_obs, 0.0f);
      std::vector<float> vis_vector(n_obs, 1.0f);
      boxm_compute_shadow_appearance<APM>(obs_vector, pre_vector,
                                          vis_vector, data.appearance_,
                                          min_app_sigma_, shadow_prior_,
                                          shadow_mean_, shadow_sigma_,
                                          verbose_);
      cell->set_data(data);
      }
    }
    scene_.write_active_block();
    for (unsigned int i=0; i<aux_readers.size(); ++i) {
      aux_readers[i]->close();
    }
    iter++;
  }
  // clear the aux scenes
  for (unsigned i=0; i<aux_scenes.size(); i++)
    aux_scenes[i].clean_scene();
  return true;
}


#define BOXM_SHADOW_APP_INITIALIZER_INSTANTIATE(T1,T2,T3) \
template class boxm_shadow_app_initializer<T1,T2,T3 >

#endif // boxm_shadow_app_initializer_hxx_
