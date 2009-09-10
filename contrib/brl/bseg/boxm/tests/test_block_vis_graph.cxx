
#include <testlib/testlib_test.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_block_vis_graph_iterator.h>
#include "test_utils.h"
#include <vpl/vpl.h>
#include <vul/vul_file.h>

MAIN( test_block_vis_graph )
{
  START ("CREATE SCENE");

  // create scene
  bgeo_lvcs lvcs(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  vgl_point_3d<double> origin(0,0,0);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(2,2,3);
  boxm_scene<tree_type> scene(lvcs, origin, block_dim, world_dim);
  vul_file::make_directory("boxm_scene");
  scene.set_paths("boxm_scene", "block");


  vgl_box_3d<double> world;
  world.add(origin);
  world.add(vgl_point_3d<double>(origin.x()+block_dim.x()*world_dim.x(), 
                                 origin.y()+block_dim.y()*world_dim.y(), 
                                 origin.z()+block_dim.z()*world_dim.z()));
  vpgl_camera_double_sptr camera = generate_camera_top(world);
  boxm_block_vis_graph_iterator<tree_type> block_vis_iter(camera, &scene, IMAGE_U, IMAGE_V);

  int vis_count = -1;
  vcl_vector<boxm_block<tree_type>*> blocks;
  int counts[] = {4,4,4};
  while (block_vis_iter.next()) {
    int block_count = 0;
    ++vis_count;
    vcl_cout << "Frontier\n";
    blocks = block_vis_iter.frontier_blocks();
    for (unsigned i=0; i<blocks.size(); ++i) {
      vcl_cout << i << "- " << blocks[i]->bounding_box() << vcl_endl;
      ++block_count;
    }
    TEST("block count", block_count, counts[vis_count]);
  }
  vpl_rmdir("boxm_scene");
  TEST("vis count", ++vis_count, 3);

  SUMMARY();
}
