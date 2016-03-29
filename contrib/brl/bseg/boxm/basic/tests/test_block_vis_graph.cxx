
#include <testlib/testlib_test.h>

#include <boxm/boxm_scene.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <boxm/util/boxm_test_util.h>
#include <vpl/vpl.h>
#include <vul/vul_file.h>

static void test_block_vis_graph()
{
  // create scene
  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
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
  std::vector<boxm_block<tree_type>*> blocks;
  int counts[] = {4,4,4};
  while (block_vis_iter.next()) {
    int block_count = 0;
    ++vis_count;
    std::cout << "Frontier\n";
    blocks = block_vis_iter.frontier_blocks();
    for (unsigned i=0; i<blocks.size(); ++i) {
      std::cout << i << "- " << blocks[i]->bounding_box() << std::endl;
      ++block_count;
    }
    TEST("block count", block_count, counts[vis_count]);
  }
  vpl_rmdir("boxm_scene");
  TEST("vis count", ++vis_count, 3);
}

TESTMAIN(test_block_vis_graph);
