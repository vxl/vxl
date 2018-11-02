// This is brl/bseg/boxm/algo/boxm_scene_levels_utils.h
#ifndef boxm_scene_levels_utils_h
#define boxm_scene_levels_utils_h
//:
// \file
// \brief Functions to explore the levels of an octre
// \author Isabel Restrepo mir@lems.brown.edu
// \date  15-Sep-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <boxm/boxm_scene.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T_data>
boxm_scene<boct_tree<short, bool> >* boxm_explore_cells_at_level(boxm_scene<boct_tree<short, T_data> > *scene_in, short level)
{
  //parameters and structure of the output scene are the same as those of the input scene
  boxm_scene<boct_tree<short, bool> > *scene_out = new boxm_scene<boct_tree<short, bool> >(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
  scene_out->set_paths(scene_in->path(), "level");
  scene_out->set_appearance_model(BOXM_BOOL);
  scene_out->write_scene("level.xml");

  scene_in->template clone_blocks_to_type<boct_tree<short, bool> >(*scene_out, false);

  //iterate through the leaf cells
  boxm_cell_iterator<boct_tree<short, T_data> > it_in =
    scene_in->cell_iterator(&boxm_scene<boct_tree<short, T_data> >::load_block, true);
  boxm_cell_iterator<boct_tree<short, bool> > it_out =
    scene_out->cell_iterator(&boxm_scene<boct_tree<short, bool> >::load_block);

  for (it_in.begin(), it_out.begin(); !it_in.end() && !it_out.end(); ++it_in, ++it_out)
  {
    boct_tree_cell<short,T_data > *cell_in = *it_in;
    boct_tree_cell<short,bool > *cell_out = *it_out;

    //check for level
    if ((cell_in->level() == level)) {
      cell_out->set_data(true);
    }
  }

  scene_in->unload_active_blocks();
  scene_out->unload_active_blocks();

  return scene_out;
}


template <class T_data>
void boxm_remove_level_0_leaves(boxm_scene<boct_tree<short, T_data> > *scene_in)
{
  typedef boct_tree<short,T_data> tree_type;

  //Iterate through the blocks
  boxm_block_iterator<tree_type> it_in = scene_in->iterator();
  for (it_in.begin(); !it_in.end(); ++it_in)
  {
    scene_in->load_block(it_in.index());
    tree_type *tree_in= (*it_in)->get_tree();

    std::vector<boct_tree_cell<short, T_data> * > all_cells =  tree_in->all_cells();
    typename std::vector<boct_tree_cell<short, T_data> * >::iterator cells_it = all_cells.begin();

    for (; cells_it!=all_cells.end(); cells_it++) {
      boct_tree_cell<short, T_data> * cell_in = *cells_it;
      //check for level
      short level = cell_in->level();
      if (level == 0) {
        cell_in = nullptr;
      }
      else {
        if (level == 1)
          cell_in->set_children_null();
        //shift loccode one space to the rigth
        boct_loc_code<short> shift_code;
        shift_code.set_code(cell_in->get_code().x_loc_ >> 1, cell_in->get_code().y_loc_ >> 1, cell_in->get_code().z_loc_ >> 1);
        shift_code.set_level(level - 1);
        cell_in->set_code(shift_code);
      }
    }

    tree_in->reset_num_levels(tree_in->number_levels() - 1);
    scene_in->write_active_block();
    std::cout << '.';
  }
  std::cout << "\nCleaning\n";
  scene_in->unload_active_blocks();
}

#endif
