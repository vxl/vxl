
#include <testlib/testlib_test.h>

#include <boxm/boxm_scene.h>
#include <boct/boct_tree.h>
#include <vpl/vpl.h>

MAIN( test_binary_io )
{
  START ("CREATE SCENE");
  short nlevels=5;
  boct_tree<short,vgl_point_3d<double> > * block = new boct_tree<short,vgl_point_3d<double> >(nlevels);
  TEST("No of Max levels of tree",nlevels, block->num_levels());
  
  block->split();
  block->print();

  bgeo_lvcs lvcs(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<double> world_dim(30,30,30);
  boxm_scene<boct_tree<short,vgl_point_3d<double> > > scene(lvcs, origin, block_dim, world_dim);
  scene.set_paths("./boxm_scene", "block");
  x_write(vcl_cout, scene, "scene");

  vsl_b_ofstream os("scene.bin");
  scene.b_write(os);
  os.close();

  boxm_scene<boct_tree<short,vgl_point_3d<double> > > scene_out;
  vsl_b_ifstream is("scene.bin", vcl_ios_binary);
  scene_out.b_read(is);
  vpl_unlink("scene.bin");
  SUMMARY();  
}
