#include <testlib/testlib_test.h>
#include <boxm/algo/rt/boxm_edge_tangent_updater.h>

#include <boxm/boxm_scene.h>
#include <boxm/sample/boxm_inf_line_sample.h>
#include <boxm/sample/boxm_rt_sample.h>
#include <boct/boct_tree.h>

static void test_edges()
{
  typedef boxm_inf_line_sample<float> data_type;
  // create the main scene
  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(1,1,1);
  boxm_scene<boct_tree<short,data_type> > scene(lvcs, origin, block_dim, world_dim);
  // create the auxiliary scenes for each image
  std::vector<std::string> image_names;
  image_names.emplace_back("image1");
  image_names.emplace_back("image2");
  image_names.emplace_back("image3");
  //boxm_edge_tangent_updater<short, float, aux_type> u(scene,imgs);
}

TESTMAIN(test_edges);
