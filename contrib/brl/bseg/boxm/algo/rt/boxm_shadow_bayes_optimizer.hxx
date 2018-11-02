#ifndef boxm_shadow_bayes_optimizer_hxx_
#define boxm_shadow_bayes_optimizer_hxx_

#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include "boxm_shadow_bayes_optimizer.h"
#include "boxm_shadow_appearance_estimator.h"

#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_aux_scene.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
boxm_shadow_bayes_optimizer<T_loc,APM,AUX>::boxm_shadow_bayes_optimizer(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                                                                        std::vector<std::string>  image_ids, float min_app_sigma,
                                                                        float shadow_prior, float shadow_mean, float shadow_sigma,
                                                                        bool verbose, vgl_point_3d<double> debug_pt)
  : image_ids_(std::move(image_ids)), scene_(scene), max_cell_P_(0.995f), min_cell_P_(0.0001f),
    min_app_sigma_(min_app_sigma), shadow_prior_(shadow_prior),
    shadow_mean_(shadow_mean), shadow_sigma_(shadow_sigma),
    verbose_(verbose), debug_pt_(debug_pt)
{}


template <class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
bool boxm_shadow_bayes_optimizer<T_loc,APM,AUX>::optimize_cells(double damping_factor)
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
    boct_tree_cell<T_loc,boxm_sample<APM> >* debug_cell  = nullptr;
    if (verbose_)
      debug_cell = tree->locate_point_global(debug_pt_);
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
      unsigned n_samp = aux_samples.size();
      std::vector<float> pre_vector(n_samp);
      std::vector<float> vis_vector(n_samp);
      // new discrete probability code
      float log_sum = 0.0f;
      float total_length_sum = 0.0f;
      //===
      typedef typename boxm_apm_traits<APM>::obs_datatype obs_type;
      std::vector<obs_type> obs_vector(n_samp);
//#if 0 // old beta calculation
      double Beta = 1.0;
//#endif
      if (verbose_&&cell == debug_cell){
        std::cout << "Contents of Debug Cell\n"
                 << "s    vis[s]    pre[s]    beta[s]\n";
      }
      double vis_sum = 0;
      for (unsigned int s=0; s<n_samp; ++s) {
        float seg_len_sum = aux_samples[s].seg_len_;
        obs_type obs = aux_samples[s].obs_;
        if (seg_len_sum > 1e-5) {
          obs_vector[s] = obs / seg_len_sum;
          float pre = aux_samples[s].pre_ / seg_len_sum;
          float vis = aux_samples[s].vis_ / seg_len_sum;
          pre_vector[s] = pre;
          vis_vector[s] = vis;

//#if 0 // old beta calculation
          double beta_s = (aux_samples[s].Beta_ / seg_len_sum);
          Beta *= beta_s;
//#else // New discrete prob code
          log_sum += aux_samples[s].log_sum_;
          total_length_sum += seg_len_sum;
//#endif
          // ====
          if (cell == debug_cell)
          {
            std::cout << s << ' ' << vis << ' ' << pre << ' '
                     << beta_s << '\n';
          }
        }
        else {
          pre_vector[s] = 0.0f;
          vis_vector[s] = 0.0f;
          obs_vector[s] = typename boxm_apm_traits<APM>::obs_datatype(0);
        }
      }
      vis_sum /= n_samp;

#if 0 // old beta calculation
      double damped_Beta = 1.0;
      if (n_samp>1.0){
        damped_Beta = std::pow(Beta, 1.0/n_samp);
      }
#else
      double damped_Beta = (Beta + damping_factor)/(damping_factor*Beta + 1.0);

      if ((damped_Beta < 0.00000001) && (damped_Beta > -0.00000001))
        std::cout << "ERROR: damped_Beta is:" << damped_Beta << std::endl;

      data.alpha *= static_cast<float>(damped_Beta);

      float alpha_post = data.alpha;
      if (total_length_sum > 0.0f) alpha_post = log_sum/total_length_sum;
      data.alpha = alpha_post;
#endif
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
        std::cerr << "\nerror: data.alpha = " << data.alpha << std::endl;
      }
      // update with new appearance
      bool print_p = false;
      if (verbose_&&cell == debug_cell){
        std::cout << "Cell Level = " << cell->level()
                 << "  AlphaPosterior  = " << data.alpha << '\n';
        print_p = true;
      }
      boxm_compute_shadow_appearance<APM>(obs_vector, pre_vector, vis_vector, data.appearance_, min_app_sigma_, shadow_prior_, shadow_mean_, shadow_sigma_, print_p);
      cell->set_data(data);
    }
    scene_.write_active_block();
    for (unsigned int i=0; i<aux_readers.size(); ++i) {
      aux_readers[i]->close();
    }
    ++iter;
  }

  // clear the aux scenes so that its starts with the refined scene next time
  for (unsigned i=0; i<aux_scenes.size(); i++) {
    aux_scenes[i].clean_scene();
  }

  return true;
}


#define BOXM_SHADOW_BAYES_OPTIMIZER_INSTANTIATE(T1,T2,T3) \
template class boxm_shadow_bayes_optimizer<T1,T2,T3 >

#endif // boxm_shadow_bayes_optimizer_hxx_
