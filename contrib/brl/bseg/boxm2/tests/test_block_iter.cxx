#include <testlib/testlib_test.h>

#include <boxm2/boxm_scene.h>
#include <boct/boct_tree.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h>
typedef boct_tree<short,vgl_point_3d<double> > tree_type;

static void test_block_iter()
{
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
    scene.load_block(iter.index());
    boxm_block<tree_type> *block = scene.get_active_block();
    block->set_tree(new tree_type(5,3));
    vcl_cout << block->bounding_box() << vcl_endl;
    ++num_blocks;
    scene.write_active_block();
  }
  int x,y,z;
  scene.block_num(x,y,z);
  TEST("Number of blocks iterator visits", num_blocks, x*y*z);

}

TESTMAIN(test_block_iter);
