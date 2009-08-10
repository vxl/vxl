#ifndef psm_aux_scene_txx_
#define psm_aux_scene_txx_

#include <vbl/vbl_bounding_box.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>

#include "boxm_aux_traits.h"
#include "boxm_aux_scene.h"

template <class T_loc, class T, class T_AUX>
boxm_aux_scene<T_loc,T,T_AUX>::boxm_aux_scene(boxm_scene<boct_tree<T_loc, T> >* scene, vcl_string storage_suffix = "")
{
  vcl_string aux_storage_dir(scene->path() + "/" + "aux");//boxm_aux_traits<T_AUX>::storage_subdir());
  vul_file::make_directory(aux_storage_dir);
  aux_storage_dir_ = aux_storage_dir;
  //boxm_aux_scene_base_sptr aux_scene_base = get_aux_scene<T_AUX>(storage_suffix);
  //boxm_aux_scene<T_AUX> *aux_scene = dynamic_cast<psm_aux_scene<T_AUX>*>(aux_scene_base.ptr());
  aux_scene_ = new boxm_scene<aux_tree_type>(scene->lvcs(), scene->origin(), scene->block_dim(), scene->world_dim());
  aux_scene_->set_path(aux_storage_dir_,  storage_suffix);

  // loop through valid blocks and init same blocks in aux scene
  boxm_block_iterator<boct_tree<T_loc, T> > iter(scene);
  iter.begin();
  while (!iter.end()) {
    boxm_block<boct_tree<T_loc,T> >* block = *iter;
    boct_tree<T_loc,T>* tree=block->get_tree();
    boct_tree<T_loc, T_AUX>* aux_tree = tree->template clone_to_type<T_AUX>();
    boxm_block<boct_tree<T_loc,T_AUX> >* aux_block = new boxm_block<boct_tree<T_loc,T_AUX> >(block->bounding_box(), aux_tree);
    aux_scene_->set_block(iter.index(), aux_block);
  }
}

template <class T_loc, class T, class T_AUX>
boct_tree_cell_reader<T_loc, T_AUX>* boxm_aux_scene<T_loc,T,T_AUX>::get_block_incremental(vgl_point_3d<int> block_idx)
{
  vcl_string path=aux_scene_->gen_block_path(block_idx.x(), block_idx.y(), block_idx.z());
  vsl_b_ifstream is(path);
  boct_tree_cell_reader<T_loc, T_AUX>* reader = new boct_tree_cell_reader<T_loc, T_AUX>(&is);
  reader->begin();
  return reader;
}

#define BOXM_AUX_SCENE_INSTANTIATE(T1,T2,T3) \
template class boxm_aux_scene<T1,T2,T3 >

#endif
