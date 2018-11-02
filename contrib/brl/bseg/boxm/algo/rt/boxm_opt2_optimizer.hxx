#ifndef boxm_opt2_optimizer_hxx_
#define boxm_opt2_optimizer_hxx_

#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include "boxm_opt2_optimizer.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_aux_scene.h>
#include <boxm/boxm_apm_traits.h>

template <class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
boxm_opt2_optimizer<T_loc,APM,AUX>::boxm_opt2_optimizer(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                                                        std::vector<std::string>  image_ids)
                                                        : image_ids_(std::move(image_ids)), scene_(scene), max_cell_P_(0.995f), min_cell_P_(0.0001f)
{}


template <class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
bool boxm_opt2_optimizer<T_loc,APM,AUX>::update()
{
  // get auxiliary scenes associated with each input image
  typedef typename boxm_aux_traits<AUX>::sample_datatype aux_type;

  typedef boct_tree<T_loc, boxm_sample<APM> > tree_type;

  std::vector<boxm_aux_scene<T_loc,  boxm_sample<APM>, aux_type> > aux_scenes;
  for (unsigned int i=0; i<image_ids_.size(); ++i) {
    boxm_aux_scene<T_loc, boxm_sample<APM>, aux_type> aux_scene(&scene_,image_ids_[i],boxm_aux_scene<T_loc, boxm_sample<APM>, aux_type>::LOAD, APM);
    aux_scenes.push_back(aux_scene);
  }

  std::vector<boxm_opt2_sample<typename boxm_apm_traits<APM>::obs_datatype> > aux_samples;

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
        //boxm_opt_sample<typename boxm_apm_traits<APM>::obs_datatype> aux_cell = temp_cell.data();
        if (!temp_cell.code_.isequal(&(cell->code_))) {
          std::cerr << "error: temp_cell idx does not match cell idx.\n";
          return false;
        }
        if (temp_cell.data().seg_len_ > 0.0f) {
          aux_samples.push_back(temp_cell.data());
        }
      }
      std::vector<float> pre_vector;
      std::vector<float> vis_vector;
      std::vector<typename boxm_apm_traits<APM>::obs_datatype> obs_vector;

      // UPDATE METHOD #1 : "OR" of occlusion probabilities
      double log_pass_prob_sum = 0.0;
      double weighted_seg_len_sum = 0.0;
      for (unsigned int s=0; s<aux_samples.size(); ++s) {
        const float obs_seg_len = aux_samples[s].seg_len_;
        const float obs_weighted_seg_len = aux_samples[s].weighted_seg_len_sum_;

        if (obs_seg_len > 1e-6) {
          if (obs_weighted_seg_len > 1e-6) {
            log_pass_prob_sum += aux_samples[s].log_pass_prob_sum_;
            weighted_seg_len_sum += obs_weighted_seg_len;
          }
          // add observation and vis,pre to list
          obs_vector.push_back(aux_samples[s].obs_ / obs_seg_len);
          pre_vector.push_back(aux_samples[s].pre_ / obs_seg_len);
          vis_vector.push_back(aux_samples[s].vis_ / obs_seg_len);
        }
      }
      // update the occlusion density
      if (weighted_seg_len_sum > 1e-6) {
        data.alpha = (float)(-log_pass_prob_sum / weighted_seg_len_sum);
      }

      // do bounds check on new alpha value
      vgl_box_3d<double> cell_bb = tree->cell_bounding_box(cell);
      const float cell_len = float(cell_bb.width());
      const float max_alpha = -std::log(1.0f - max_cell_P_)/cell_len;
      const float min_alpha = -std::log(1.0f - min_cell_P_)/cell_len;
      if (data.alpha > max_alpha) {
        data.alpha = max_alpha;
      }
      if (data.alpha < min_alpha) {
        data.alpha = min_alpha;
      }
      if (!((data.alpha >= min_alpha) && (data.alpha <= max_alpha)) ){
        std::cerr << "\nerror: data.alpha = " << data.alpha << '\n';
        //std::cerr << " log_alpha_sum = " << log_alpha_sum << ", n_actual_samples = " << n_actual_samples << '\n';
      }
      // update with new appearance
      const float min_sigma = 0.02f;
      boxm_apm_traits<APM>::apm_processor::compute_appearance(obs_vector, pre_vector, vis_vector, data.appearance_, min_sigma);

      //std::cout << "cell OUT " << data.alpha << data.appearence_ << std::endl << std::endl;
      cell->set_data(data);
    }
    scene_.write_active_block();
    for (unsigned int i=0; i<aux_readers.size(); ++i) {
      aux_readers[i]->close();
    }
    iter++;
  }
#ifdef DEBUG
  std::cout << "done with all cells" << std::endl;
#endif
  // clear the aux scenes so that its starts with the refined scene next time
  for (unsigned i=0; i<aux_scenes.size(); i++) {
    aux_scenes[i].clean_scene();
  }

  return true;
}


#define BOXM_OPT2_OPTIMIZER_INSTANTIATE(T1,T2,T3) \
  template class boxm_opt2_optimizer<T1,T2,T3 >

#endif // boxm_opt2_optimizer_hxx_
