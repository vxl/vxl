#include <testlib/testlib_test.h>

#include <boxm/boxm_scene.h>
#include <boct/boct_tree.h>

#include "test_utils.h"

typedef boct_tree<short,vgl_point_3d<double> > tree_type;

static void test_block_iter()
{
  clean_up();
  short nlevels=5;
  auto * block = new tree_type(nlevels);
  TEST("No of Max levels of tree",nlevels, block->number_levels());

  block->split();
  block->print();
  delete block;

  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(3,3,3);
  boxm_scene<tree_type> scene(lvcs, origin, block_dim, world_dim);
  scene.set_paths("./boxm_scene", "block");

  boxm_block_iterator<tree_type> iter = scene.iterator();
  int num_blocks=0;
  for (; !iter.end(); ++iter) {
    scene.load_block(iter.index());
    boxm_block<tree_type> *block = scene.get_active_block();
    block->set_tree(new tree_type(5,3));
    std::cout << block->bounding_box() << std::endl;
    ++num_blocks;
    scene.write_active_block();
  }
  int x,y,z;
  scene.block_num(x,y,z);
  TEST("Number of blocks iterator visits", num_blocks, x*y*z);
#ifdef DEBUG_LEAKS
  std::cerr << "Leaks in test_block_iter: " << boct_tree_cell<short,vgl_point_3d<double> >::nleaks() << '\n';
#endif
  clean_up();
}

TESTMAIN(test_block_iter);
