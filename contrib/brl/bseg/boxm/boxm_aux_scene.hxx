#ifndef boxm_aux_scene_hxx_
#define boxm_aux_scene_hxx_

#include "boxm_aux_scene.h"
#include <boxm/boxm_aux_traits.h>

#include <vpl/vpl.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T_loc, class T, class T_AUX>
boxm_aux_scene<T_loc,T,T_AUX>::boxm_aux_scene(boxm_scene<tree_type>* scene,
                                              std::string storage_suffix,
                                              tree_creation_type type,
                                              boxm_apm_type app_model)
: aux_scene_(nullptr)
{
  std::string aux_storage_dir;
  aux_storage_dir_ = scene->path();
  aux_scene_ = new boxm_scene<aux_tree_type >(scene->lvcs(), scene->origin(), scene->block_dim(), scene->world_dim(), scene->load_all_blocks(),scene->save_internal_nodes(), scene->save_platform_independent());
  aux_scene_->set_path(aux_storage_dir_,  storage_suffix);
  aux_scene_->set_appearance_model(app_model);
  aux_scene_->set_octree_levels(scene->max_level(),scene->init_level());
  if (type != EMPTY)
  {
    // loop through valid blocks and init same blocks in aux scene
    boxm_block_iterator<tree_type > iter(scene);
    iter.begin();
    while (!iter.end()) {
      if (scene->discover_block(iter.index().x(),iter.index().y(),iter.index().z())) {
        if (type == LOAD) {
          if (aux_scene_->discover_block(iter.index().x(),iter.index().y(),iter.index().z())) {
#if 0
            // auxiliary block file exist
            aux_scene_->load_block(iter.index());
            boxm_block<boct_tree<T_loc,T_AUX> > * block=aux_scene_->get_active_block();
            assert(block->get_tree() != 0);
#endif
          }
        }
        else if (type==CLONE) {  //clone from the primary block
          scene->load_block(iter.index().x(),iter.index().y(),iter.index().z());
          boxm_block<boct_tree<T_loc,T> > * block=scene->get_active_block();
          boct_tree<T_loc,T>* tree=block->get_tree();
          aux_tree_type *aux_tree = tree->template clone_to_type<T_AUX>();
          boxm_block<aux_tree_type >* aux_block = new boxm_block<aux_tree_type >(block->bounding_box(),aux_tree );
          aux_scene_->set_block(iter.index(), aux_block);
          aux_scene_->write_active_block();
        }
      }
      ++iter;
    }
    aux_scene_->write_scene(storage_suffix+".xml");
    scene->unload_active_blocks();
    aux_scene_->unload_active_blocks();
    }
}


template <class T_loc, class T, class T_AUX>
boct_tree_cell_reader<T_loc, T_AUX>* boxm_aux_scene<T_loc,T,T_AUX>::get_block_incremental(vgl_point_3d<int> block_idx)
{
  if (aux_scene_) {
    std::string path=aux_scene_->gen_block_path(block_idx.x(), block_idx.y(), block_idx.z());
    boct_tree_cell_reader<T_loc, T_AUX>* reader = new boct_tree_cell_reader<T_loc, T_AUX>(path);
    reader->begin();
    return reader;
  }
  else {
    return nullptr;
  }
}

template <class T_loc, class T, class T_AUX>
void boxm_aux_scene<T_loc,T,T_AUX>::clean_scene()
{
  std::cout<<"Clean AUX scene "<<std::endl;
  aux_scene_->unload_active_blocks();
  boxm_block_iterator<boct_tree<T_loc, T_AUX> > iter(aux_scene_);
  iter.begin();
  if (!aux_scene_->load_all_blocks())
  {
    while (!iter.end()) {
      if (aux_scene_->discover_block(iter.index().x(),iter.index().y(),iter.index().z())) {
        std::string filename=aux_scene_->gen_block_path(iter.index().x(),iter.index().y(),iter.index().z());
        vpl_unlink(filename.c_str());
      }
      iter++;
    }
  }
  else
  {
    while (!iter.end()) {
      if (iter->get_tree()!=nullptr)
      {
        iter->delete_tree();
        iter->set_tree(nullptr);
      }
      iter++;
    }
  }
}

#if 0 // commented out
template <class T_loc, class T, class T_AUX>
boct_tree_cell_reader<T_loc, T_AUX>* boxm_aux_scene<T_loc,T,T_AUX>::save_scene()
{
  boxm_block_iterator<boct_tree<T_loc, T_AUX> > iter(&aux_scene);
  iter.begin();
  while (!iter.end()) {
    // body
  }
}
#endif // 0

#define BOXM_AUX_SCENE_INSTANTIATE(T1,T2,T3) \
template class boxm_aux_scene<T1,T2,T3 >

#endif
