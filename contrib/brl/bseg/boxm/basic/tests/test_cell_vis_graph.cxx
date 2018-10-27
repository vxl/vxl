#include <testlib/testlib_test.h>

#include <boxm/boxm_scene.h>
#include <boxm/basic/boxm_cell_vis_graph_iterator.h>
#include <boxm/util/boxm_test_util.h>
#include <vpl/vpl.h>
#include <vul/vul_file.h>

static void test_cell_vis_graph()
{
  // create scene
  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(1,1,1);
  boxm_scene<tree_type> scene(lvcs, origin, block_dim, world_dim);
  scene.set_paths("boxm_scene_cell", "block");
  vul_file::make_directory("boxm_scene_cell");

  vgl_box_3d<double> world;
  world.add(origin);
  world.add(vgl_point_3d<double>(origin.x()+block_dim.x()*world_dim.x(),
                                 origin.y()+block_dim.y()*world_dim.y(),
                                 origin.z()+block_dim.z()*world_dim.z()));

  vpgl_camera_double_sptr camera = generate_camera_top(world);
  boxm_block_iterator<boct_tree<short,vgl_point_3d<double> > > iter(&scene);

  while (!iter.end())
  {
    scene.load_block(iter.index().x(),iter.index().y(),iter.index().z());
    boxm_block<boct_tree<short,vgl_point_3d<double> > > * block=scene.get_active_block();
    auto * tree=new boct_tree<short,vgl_point_3d<double> >(3,2);
    block->init_tree(tree);
    boxm_cell_vis_graph_iterator<short,vgl_point_3d<double> > cell_iterator(camera,tree,IMAGE_U, IMAGE_V);
    double cnt=30;
    while (cell_iterator.next()){
      std::cout<<"Frontier\n";
      std::vector<boct_tree_cell<short,vgl_point_3d<double> > *> vis_cells=cell_iterator.frontier();
      for (auto & vis_cell : vis_cells)
      {
        vgl_box_3d<double> box=tree->cell_bounding_box(vis_cell);
        TEST("Returns correct frontier",cnt,box.max_z());
      }
      cnt-=5;
    }

    scene.write_active_block();
    iter++;
  }
  vpl_rmdir("boxm_scene_cell");
}

TESTMAIN(test_cell_vis_graph);
