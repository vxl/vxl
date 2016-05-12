//:
// \file
// \author Isabel Restrepo
// \date Nov 23 2011

#include <testlib/testlib_test.h>
#include "test_utils.h"

#include <boxm/algo/boxm_scene_levels_utils.h>


void test_remove_level0()
{
  clean_up();
  //create scene
  boxm_scene<boct_tree<short, float> > *scene1 = create_scene(2,2,2,true, "test_scene1");

  //clone the input scene
  boxm_scene<boct_tree<short, float> > *scene2 = new boxm_scene<boct_tree<short, float> >(scene1->lvcs(), scene1->origin(), scene1->block_dim(),
                                                                                          scene1->world_dim(), scene1->max_level(), scene1->init_level(),
                                                                                          scene1->load_all_blocks(), scene1->save_internal_nodes());

  std::stringstream block_pref;
  block_pref << scene1->block_prefix() << "_max_level_"  << (scene1->max_level() - 1);
  scene2->set_paths(scene1->path(), block_pref.str());
  scene2->set_appearance_model(scene1->appearence_model());

  std::cout << "Cloning\n";
  scene1->clone_blocks(*scene2);
  std::cout << "Restructuring\n";
  boxm_remove_level_0_leaves(scene2);
  std::cout << "Done\n";

  //Show contents of the scenes scene
  boxm_block_iterator<boct_tree<short, float> > it1 = scene1->iterator();
  boxm_block_iterator<boct_tree<short, float> > it2 = scene2->iterator();
  for (it1.begin(), it2.begin(); !it1.end() && !it2.end(); ++it1, ++it2)
  {
    scene1->load_block(it1.index());
    scene2->load_block(it2.index());

    boxm_block<boct_tree<short, float> > *block1 = scene1->get_active_block();
    boxm_block<boct_tree<short, float> > *block2 = scene2->get_active_block();

    block1->get_tree()->print();
    block2->get_tree()->print();
  }

  scene1->unload_active_blocks();
  scene2->unload_active_blocks();
}


TESTMAIN(test_remove_level0);
