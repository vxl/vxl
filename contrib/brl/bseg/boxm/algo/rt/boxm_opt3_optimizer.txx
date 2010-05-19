#ifndef boxm_opt3_optimizer_txx_
#define boxm_opt3_optimizer_txx_

#include "boxm_opt3_optimizer.h"

#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_aux_scene.h>
#include <boxm/sample/boxm_opt3_sample.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>

#include <vnl/vnl_random.h>
#include <vcl_vector.h>
#include <vcl_string.h>

template <class T_loc, boxm_apm_type APM, boxm_apm_type AUX_APM>
boxm_opt3_optimizer<T_loc,APM,AUX_APM>::boxm_opt3_optimizer(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                                                            vcl_vector<vcl_string> const& image_ids)
                                                            : image_ids_(image_ids), scene_(scene), max_cell_P_(0.995f), min_cell_P_(0.0001f)
{}


template <class T_loc, boxm_apm_type APM, boxm_apm_type AUX_APM>
bool boxm_opt3_optimizer<T_loc,APM,AUX_APM>::optimize_cells()
{
  const unsigned int n_samples_per_obs = 10;

  // get auxiliary scenes associated with each input image
  typedef boxm_opt3_sample<AUX_APM> aux_type;

  typedef boct_tree<T_loc, boxm_sample<APM> > tree_type;
  typedef boct_tree<T_loc, aux_type > aux_tree_type;

  typedef typename boxm_apm_traits<APM>::obs_datatype obs_t;

  vcl_vector<boxm_aux_scene<T_loc,  boxm_sample<APM>, aux_type> > aux_scenes;
  for (unsigned int i=0; i<image_ids_.size(); ++i) {
    boxm_aux_scene<T_loc, boxm_sample<APM>, aux_type> aux_scene(&scene_,image_ids_[i],boxm_aux_scene<T_loc, boxm_sample<APM>, aux_type>::LOAD, APM);
    aux_scenes.push_back(aux_scene);
  }

  vcl_vector<float> vis_vector;
  vcl_vector<obs_t> obs_vector;
  vnl_random rand_gen;
  vcl_vector<boxm_opt3_sample<AUX_APM> > aux_samples;

  // for each block
  boxm_block_iterator<tree_type> iter(&scene_);
  iter.begin();
  while (!iter.end())
  {
    scene_.load_block(iter.index());
    boxm_block<tree_type>* block = *iter;
    boct_tree<T_loc, boxm_sample<APM> >* tree = block->get_tree();
    vcl_vector<boct_tree_cell<T_loc,boxm_sample<APM> >*> cells = tree->leaf_cells();

    // get a vector of incremental readers for each aux scene.
    vcl_vector<boct_tree_cell_reader<T_loc, aux_type>* > aux_readers(aux_scenes.size());
    for (unsigned int i=0; i<aux_scenes.size(); ++i) {
      aux_readers[i] = aux_scenes[i].get_block_incremental(iter.index());
    }

    // iterate over cells
    for (unsigned i=0; i<cells.size(); ++i)
    {
      aux_samples.clear();
      boct_tree_cell<T_loc,boxm_sample<APM> >* cell = cells[i];
      boxm_sample<APM> data = cell->data();

      for (unsigned j=0; j<aux_readers.size(); j++) {
        boct_tree_cell<T_loc, aux_type> temp_cell;

        if (!aux_readers[j]->next(temp_cell)) {
          vcl_cerr << "error: incremental reader returned false.\n";
          return false;
        }
        //boxm_opt_sample<typename boxm_apm_traits<APM>::obs_datatype> aux_cell = temp_cell.data();
        if (!temp_cell.code_.isequal(&(cell->code_))) {
          vcl_cerr << "error: temp_cell idx does not match cell idx.\n";
          return false;
        }
        aux_samples.push_back(temp_cell.data());
      }

      vis_vector.clear();
      obs_vector.clear();

      double log_alpha_sum = 0.0;
      unsigned int n_actual_observations = 0;

      for (unsigned int s=0; s<aux_samples.size(); ++s) {
        double total_seg_len = aux_samples[s].seg_len_sum_;

        if (total_seg_len > 1e-6) {
          ++n_actual_observations;
          // compute mean vis
          float obs_vis = (float)(aux_samples[s].weighted_vis_sum_ / total_seg_len);
          // draw samples from obs_dist

          for (unsigned int n=0; n<n_samples_per_obs; ++n) {
            obs_t obs_sample = boxm_apm_traits<AUX_APM>::apm_processor::sample(aux_samples[s].obs_dist_, rand_gen);
            //obs_t obs_sample = boxm_apm_traits<AUX_APM>::apm_processor::expected_color(aux_samples[s].obs_dist_);
            obs_vector.push_back(obs_sample);
            vis_vector.push_back(obs_vis);
          }
          double alpha_s = -aux_samples[s].log_pass_prob_sum_ / total_seg_len;
          log_alpha_sum += vcl_log(alpha_s);
        }
      }
      float alpha_new = 0.0f;
      if (n_actual_observations > 0) {
        //vcl_cout << "cell IN " << data.alpha << ' ' << data.appearance_<< vcl_endl;
        // compute new alpha value
        alpha_new = (float)vcl_exp(log_alpha_sum / n_actual_observations);

        // do bounds check on new alpha value
        vgl_box_3d<double> cell_bb = tree->cell_bounding_box(cell);
        const float cell_len = float(cell_bb.width());
        const float max_alpha = -vcl_log(1.0f - max_cell_P_)/cell_len;
        const float min_alpha = -vcl_log(1.0f - min_cell_P_)/cell_len;
        if (alpha_new > max_alpha) {
          alpha_new = max_alpha;
        }
        if (alpha_new < min_alpha) {
          alpha_new = min_alpha;
        }
        if (!((alpha_new >= min_alpha) && (alpha_new <= max_alpha)) ){
          vcl_cerr << vcl_endl << "error: alpha_new = " << alpha_new << '\n'
                   << "n_actual_observations = " << n_actual_observations << " log_alpha_sum = " << log_alpha_sum << '\n';
          alpha_new = min_alpha;
        }
        else {
          data.alpha = alpha_new;
        }
        // update with new appearance
        //vcl_cout << obs_vector.size() << ' ' << vcl_endl;
        if (obs_vector.size() != n_samples_per_obs * n_actual_observations) {
          vcl_cerr << "error: n_samples_per_obs = " << n_samples_per_obs << " n_actual_observations = " << n_actual_observations << " obs_vector.size() " << obs_vector.size() << '\n';
        }
        boxm_apm_traits<APM>::apm_processor::compute_appearance(obs_vector, vis_vector, data.appearance_, 0.025f);
        //vcl_cout << "cell OUT " << data.alpha << data.appearance_ << vcl_endl << vcl_endl;
        cell->set_data(data);
      }
    }
    scene_.write_active_block();
    for (unsigned int i=0; i<aux_readers.size(); ++i) {
      aux_readers[i]->close();
    }
    iter++;
  }
#ifdef DEBUG
  vcl_cout << "done with all cells" << vcl_endl;
#endif
  // clear the aux scenes so that its starts with the refined scene next time
  for (unsigned i=0; i<aux_scenes.size(); i++) {
    aux_scenes[i].clean_scene();
  }

  return true;
}


#define BOXM_OPT3_OPTIMIZER_INSTANTIATE(T1,T2,T3) \
  template class boxm_opt3_optimizer<T1,T2,T3 >

#endif // boxm_opt3_optimizer_txx_
