
#include <testlib/testlib_test.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_block_vis_graph_iterator.h>
#include "test_utils.h"
#include <vpl/vpl.h>

MAIN( test_block_vis_graph )
{
  START ("CREATE SCENE");
  short nlevels=5;

  // create scene
  bgeo_lvcs lvcs(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  vgl_point_3d<double> origin(0,0,0);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<double> world_dim(20,20,30);
  boxm_scene<tree_type> scene(lvcs, origin, block_dim, world_dim);
  scene.set_paths("./boxm_scene", "block");

  vgl_box_3d<double> world;
  world.add(origin);
  world.add(vgl_point_3d<double>(origin.x()+world_dim.x(), origin.y()+world_dim.y(), origin.z()+world_dim.z()));
  vpgl_camera_double_sptr camera = generate_camera_top(world);
  boxm_block_vis_graph_iterator<tree_type> block_vis_iter(camera, &scene, IMAGE_U, IMAGE_V);

  vcl_vector<boxm_block<tree_type>*> blocks;
  while (block_vis_iter.next()) {
    vcl_cout << "Frontier\n";
    blocks = block_vis_iter.frontier_blocks();
    for(unsigned i=0; i<blocks.size(); i++) {
      vcl_cout << i << "- " << blocks[i]->bounding_box() << vcl_endl; 
    }
  }
  vpl_rmdir("./boxm_scene");
  vpl_unlink("./scene.bin");

  //TEST("Number of blocks iterator visits", num_blocks, x*y*z);
  SUMMARY();
}
