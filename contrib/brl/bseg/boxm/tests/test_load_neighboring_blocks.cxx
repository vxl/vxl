//:
// \file
// \author Isabel Restrepo
// \date 17-Aug-2010

#include <testlib/testlib_test.h>

#include "test_utils.h"

void test_load_neighboring_blocks()
{
  clean_up();
  //create scene
  boxm_scene<boct_tree<short, float> > *scene = create_scene(4,4,4);
  std::cout << "Scene Created" << std::endl;

  //iterate through blocks, loading blocks and eighbors
  boxm_block_iterator<boct_tree<short, float> > iter=scene->iterator();
  iter.begin();
  bool result = true;
  while (!iter.end()) {
    if (scene->load_block_and_neighbors(iter.index().x(),iter.index().y(),iter.index().z())) {
      //std::cout << "Center blocks : "  << iter.index()<< std::endl;
      std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> > active_blocks = scene->active_blocks();

      //iterate through the active blocks and check that their trees are in memory
      auto it = active_blocks.begin();

      for (; it!=active_blocks.end(); it++){
        boxm_block<boct_tree<short, float> > *block = scene->get_block(*it);
        if(!block)
          result = false;
        boct_tree<short, float> *tree = block->get_tree();
        if(!tree)
          result =false;
      }


      unsigned sum =0;
      if (iter.index().x()>0 && iter.index().x()<3) sum++;
      if (iter.index().y()>0 && iter.index().y()<3) sum++;
      if (iter.index().z()>0 && iter.index().z()<3) sum++;
      if(sum == 0)
        if(active_blocks.size()!=8){
          result = false;
          std::cout << sum << ' ' << active_blocks.size() << std::endl;
        }
      if(sum == 1)
        if(active_blocks.size()!=12){
          result = false;
          std::cout << sum << ' ' << active_blocks.size() << std::endl;
        }
      if(sum == 2)
        if(active_blocks.size()!=18){
          result = false;
          std::cout << sum << ' ' << active_blocks.size() << std::endl;
        }
      if(sum == 3)
        if(active_blocks.size()!=27){
          result=false;
          std::cout << sum << ' ' << active_blocks.size() << std::endl;
        }
#if 0
      std::cout << "Active blocks : " << std::endl;
      std::set<vgl_point_3d<int> >::iterator it = active_blocks.begin();
      for (; it!=active_blocks.end(); it++)
        std::cout << *it << std::endl;
#endif
    }
    iter++;

  }

  scene->unload_active_blocks();
  if(scene->active_blocks().size()!=0){
    result = false;
    std::cout << "Failed to unload active blocks" << std::endl;
  }

#ifdef DEBUG_LEAKS
  std::cerr << "Leaks test_load_neighboring_blocks: " << boct_tree_cell<short, float >::nleaks() << std::endl;
#endif

  TEST("Valid Test", result, true);
  clean_up();
}


TESTMAIN(test_load_neighboring_blocks);
