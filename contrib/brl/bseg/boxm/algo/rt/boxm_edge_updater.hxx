#ifndef boxm_edge_updater_hxx_
#define boxm_edge_updater_hxx_

#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include "boxm_edge_updater.h"

#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_aux_scene.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T_loc, class APM, class AUX>
boxm_edge_updater<T_loc,APM,AUX>::boxm_edge_updater(boxm_scene<boct_tree<T_loc,
                                                    boxm_edge_sample<APM> > > &scene,
                                                    std::vector<std::string>  image_ids)
: image_ids_(std::move(image_ids)), scene_(scene)
{}


template <class T_loc, class APM, class AUX>
bool boxm_edge_updater<T_loc,APM,AUX>::add_cells()
{
  // get auxiliary scenes associated with each input image
  //typedef typename boxm_aux_traits<AUX>::sample_datatype AUX;

  typedef boct_tree<T_loc, boxm_edge_sample<APM> > tree_type;

  std::vector<boxm_aux_scene<T_loc,  boxm_edge_sample<APM>, boxm_aux_edge_sample<AUX> > > aux_scenes;
  for (unsigned int i=0; i<image_ids_.size(); ++i) {
    boxm_aux_scene<T_loc, boxm_edge_sample<APM>, boxm_aux_edge_sample<AUX> > aux_scene(&scene_,image_ids_[i],boxm_aux_scene<T_loc, boxm_edge_sample<APM>, boxm_aux_edge_sample<AUX> >::LOAD);
    aux_scenes.push_back(aux_scene);
  }

  std::vector<boxm_aux_edge_sample<APM> > aux_samples;

  // for each block
  boxm_block_iterator<tree_type> iter(&scene_);
  iter.begin();
  while (!iter.end())
  {
    scene_.load_block(iter.index());
    boxm_block<tree_type>* block = *iter;
    boct_tree<T_loc, boxm_edge_sample<APM> >* tree = block->get_tree();
    std::vector<boct_tree_cell<T_loc,boxm_edge_sample<APM> >*> cells = tree->leaf_cells();

    // get a vector of incremental readers for each aux scene.
    std::vector<boct_tree_cell_reader<T_loc, boxm_aux_edge_sample<AUX> > *> aux_readers(aux_scenes.size());
    for (unsigned int i=0; i<aux_scenes.size(); ++i) {
      aux_readers[i] = aux_scenes[i].get_block_incremental(iter.index());
    }
    // iterate over cells
    for (unsigned i=0; i<cells.size(); ++i)
    {
      aux_samples.clear();
      boct_tree_cell<T_loc,boxm_edge_sample<APM> >* cell = cells[i];
      boxm_edge_sample<APM> data = cell->data();
      //std::cout << "cell IN " << data.alpha << data.appearence_<< std::endl;
      for (unsigned j=0; j<aux_readers.size(); j++) {
        boct_tree_cell<T_loc, boxm_aux_edge_sample<AUX> > temp_cell;

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

      //std::vector<typename boxm_edge_sample<APM> > obs_vector(aux_samples.size());
      for (unsigned int s=0; s<aux_samples.size(); ++s) {
        data.edge_prob_ += aux_samples[s].obs_/aux_samples[s].seg_len_;
      }
      data.num_obs_ += 1;
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


#define BOXM_EDGE_UPDATER_INSTANTIATE(T1,T2,T3) \
template class boxm_edge_updater<T1,T2,T3 >

#endif // boxm_edge_updater_hxx_
