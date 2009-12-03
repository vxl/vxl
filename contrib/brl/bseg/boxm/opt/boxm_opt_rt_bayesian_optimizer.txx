#ifndef boxm_opt_rt_bayesian_optimizer_txx_
#define boxm_opt_rt_bayesian_optimizer_txx_

#include "boxm_opt_rt_bayesian_optimizer.h"
#include "boxm_opt_appearance_estimator.h"

#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_aux_traits.h>
#include <boxm/boxm_scene.h>
#include <boxm/opt/boxm_aux_scene.h>

#include <vcl_vector.h>
#include <vcl_string.h>

template <class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
boxm_opt_rt_bayesian_optimizer<T_loc,APM,AUX>::boxm_opt_rt_bayesian_optimizer(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                                                                              vcl_vector<vcl_string> const& image_ids)
: image_ids_(image_ids), scene_(scene), max_cell_P_(0.995f), min_cell_P_(0.0001f)
{}


template <class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
bool boxm_opt_rt_bayesian_optimizer<T_loc,APM,AUX>::optimize_cells(double damping_factor)
{
  // get auxiliary scenes associated with each input image
  typedef typename boxm_aux_traits<AUX>::sample_datatype aux_type;

  typedef boct_tree<T_loc, boxm_sample<APM> > tree_type;
  typedef boct_tree<T_loc, aux_type > aux_tree_type;

  vcl_vector<boxm_aux_scene<T_loc,  boxm_sample<APM>, aux_type> > aux_scenes;
  for (unsigned int i=0; i<image_ids_.size(); ++i) {
    boxm_aux_scene<T_loc, boxm_sample<APM>, aux_type> aux_scene(&scene_,image_ids_[i],boxm_aux_scene<T_loc, boxm_sample<APM>, aux_type>::LOAD);
    aux_scenes.push_back(aux_scene);
  }

  vcl_vector<boxm_rt_sample<typename boxm_apm_traits<APM>::obs_datatype> > aux_samples;

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
      //vcl_cout << "cell IN " << data.alpha << data.appearence_<< vcl_endl;
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
        if (temp_cell.data().seg_len_ > 0.0f) {
          aux_samples.push_back(temp_cell.data());
        }
      }

      //boxm_sample<APM> &cell_data = cell_it->data();
      vcl_vector<float> pre_vector(aux_samples.size());
      vcl_vector<float> vis_vector(aux_samples.size());
      //vcl_vector<float> post_prob_vector(aux_samples.size());
      vcl_vector<typename boxm_apm_traits<APM>::obs_datatype> obs_vector(aux_samples.size());
      double Beta = 1.0;
      for (unsigned int s=0; s<aux_samples.size(); ++s) {
        float seg_len = aux_samples[s].seg_len_;
        if (seg_len > 1e-5) {
          Beta *= (aux_samples[s].Beta_ / seg_len);
          obs_vector[s] = aux_samples[s].obs_ / seg_len;
          pre_vector[s] = aux_samples[s].pre_ / seg_len;
          vis_vector[s] = aux_samples[s].vis_ / seg_len;
          //post_prob_vector[s] = aux_samples[s].post_prob_ / seg_len;
        }
        else {
          pre_vector[s] = 0.0f;
          vis_vector[s] = 0.0f;
          //post_prob_vector[s] = 0.0f;
          obs_vector[s] = typename boxm_apm_traits<APM>::obs_datatype(0);
        }
      }
      double damped_Beta = (Beta + damping_factor)/(damping_factor*Beta + 1.0);
      
      if ((damped_Beta < 0.00000001) && (damped_Beta > -0.00000001))
        vcl_cout << "ERROR: damped_Beta is:" << damped_Beta << vcl_endl;
        
      data.alpha *= (float)damped_Beta;
      // do bounds check on new alpha value
      vgl_box_3d<double> cell_bb = tree->cell_bounding_box(cell);
      const float cell_len = float(cell_bb.width());
      const float max_alpha = -vcl_log(1.0f - max_cell_P_)/cell_len;
      const float min_alpha = -vcl_log(1.0f - min_cell_P_)/cell_len;
      if (data.alpha > max_alpha) {
        data.alpha = max_alpha;
      }
      if (data.alpha < min_alpha) {
        data.alpha = min_alpha;
      }
      if (!((data.alpha >= min_alpha) && (data.alpha <= max_alpha)) ){
        vcl_cerr << "\nerror: data.alpha = " << data.alpha << vcl_endl
                 << "damped_Beta = " << damped_Beta << vcl_endl;
      }
      // update with new appearance
      boxm_opt_compute_appearance<APM>(obs_vector, pre_vector, vis_vector, data.appearance_);
      //vcl_cout << "cell OUT " << data.alpha << data.appearence_ << vcl_endl << vcl_endl;
      cell->set_data(data);
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


#define BOXM_OPT_RT_BAYESIAN_OPTIMIZER_INSTANTIATE(T1,T2,T3) \
template class boxm_opt_rt_bayesian_optimizer<T1,T2,T3 >

#endif // boxm_opt_rt_bayesian_optimizer_txx_
