#include <testlib/testlib_test.h>

#include <boxm/boxm_scene.h>
#include <boct/boct_tree.h>

typedef boct_tree<short,vgl_point_3d<double> > tree_type;

MAIN( test_block_iter )
{
  START ("CREATE SCENE");
  short nlevels=5;
  tree_type * block = new tree_type(nlevels);
  TEST("No of Max levels of tree",nlevels, block->number_levels());

  block->split();
  block->print();

  bgeo_lvcs lvcs(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(3,3,3);
  boxm_scene<tree_type> scene(lvcs, origin, block_dim, world_dim);
  scene.set_paths("./boxm_scene", "block");

  boxm_block_iterator<tree_type> iter = scene.iterator();
  int num_blocks=0;
  for (; !iter.end(); ++iter) {
    boxm_block<tree_type> *block = *iter;
    vcl_cout << block->bounding_box() << vcl_endl;
    ++num_blocks;
  }
  int x,y,z;
  scene.block_num(x,y,z);
  TEST("Number of blocks iterator visits", num_blocks, x*y*z);
  SUMMARY();
}
