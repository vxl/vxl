//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <string>
#include <vector>
#include <boxm2/boxm2_scene.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vgl/vgl_point_3d.h>
#include <vpl/vpl.h>
void test_scene()
{
  //test xml file
  std::string test_dir  = testlib_root_dir()+ "/contrib/brl/bseg/boxm2/tests/";
  std::string test_file = test_dir + "test.xml";

  //create block metadata
  std::map<boxm2_block_id, boxm2_block_metadata> blocks;
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      double big_block_side = 2.0;
      boxm2_block_id id(i,j,0);
      boxm2_block_metadata data;
      data.id_ = id;
      data.local_origin_ = vgl_point_3d<double>(big_block_side*i, big_block_side*j, 0.0);
      data.sub_block_dim_ = vgl_vector_3d<double>(.2, .2, .2);
      data.sub_block_num_ = vgl_vector_3d<unsigned>(10, 10, 10);
      data.init_level_ = 1;
      data.max_level_ = 4;
      data.max_mb_ = 400;
      data.p_init_ = .001;
      data.version_ = 1;

      //push it into the map
      blocks[id] = data;
    }
  }

  //create scene
  boxm2_scene scene;
  scene.set_local_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  scene.set_rpc_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  vpgl_lvcs lvcs;
  scene.set_lvcs(lvcs);
  scene.set_xml_path(test_file);
  scene.set_data_path(test_dir);
  scene.set_blocks(blocks);
  scene.save_scene();

  //create test scene
  boxm2_scene test_scene_data(test_file);
  std::cout<<test_scene_data<<std::endl;

  //delete file created
  vpl_unlink(test_file.c_str());
  vgl_box_3d<double> bb;
  vgl_point_3d<double> pmin(-5.0, -5.0, -5.0);
  vgl_point_3d<double> pmax( 5.0,  5.0,  5.0);
  bb.add(pmin); bb.add(pmax);
  double sub_block_len = 0.2;
  std::vector<std::string> prefixes;
  prefixes.emplace_back("boxm2_mog3_grey");
  prefixes.emplace_back("boxm2_num_obs");
  boxm2_scene scene_one_block(test_dir, "scene_1b", "scene_data", prefixes, bb, sub_block_len);
  TEST("valid one block scene", scene_one_block.local_origin() == pmin, true);
}


TESTMAIN(test_scene);
