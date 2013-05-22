// This is contrib/brl/bbas/volm/desc/tests/test_volm_descriptor_query.cxx
//:
// \file
// \brief Tests for volm_descriptor_query
// \author Yi Dong
// \date   May 20, 2013
//
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <testlib/testlib_test.h>
#include <volm/desc/volm_descriptor_query.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>

static void test_volm_descriptor_query()
{
  // create a depth map scene

  // load a depth map scene from file
  vcl_string dm_file = "z:/projects/FINDER/test1/p1a_test1_20/p1a_test1_20.vsl";
  depth_map_scene_sptr dm = new depth_map_scene;
  vsl_b_ifstream dis(dm_file.c_str());
  dm->b_read(dis);
  dis.close();

  // define the depth interval
  vcl_vector<double> radius;
  radius.push_back(1000);  radius.push_back(500);  radius.push_back(2000);
  vcl_vector<double> height;

  volm_descriptor_query vd_query(dm, radius, height);

  vd_query.print();
  vd_query.visualize("./test_vd_query.svg", 5);
}

TESTMAIN( test_volm_descriptor_query );
