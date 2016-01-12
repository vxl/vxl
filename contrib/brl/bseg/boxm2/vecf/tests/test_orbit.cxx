//:
// \file
// \author J.L. Mundy
// \date 11/19/14


#include <testlib/testlib_test.h>
#include <vcl_fstream.h>
#include <vgl/vgl_box_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vul/vul_timer.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <vul/vul_file.h>
#include "../boxm2_vecf_orbit_scene.h"
#include <vul/vul_timer.h>
#include "../boxm2_vecf_orbit_params.h"
#include "../boxm2_vecf_eyelid.h"
#include "../boxm2_vecf_eyelid_crease.h"
typedef vnl_vector_fixed<unsigned char, 16> uchar16;
#define BUILD_TEST_ORBIT
void test_orbit()
{
#ifdef BUILD_TEST_ORBIT
  boxm2_vecf_eyelid_crease ec;
#if 0
  double t = 1.0;
  vcl_cout << "{\n";
  for(double x = -10.0; x<=10.0; x+=0.5){
    double y = ec.gi(x, t);
    vcl_cout << "{" << x << ','<<y<<"},";
  }
  vcl_cout << "}\n";
  vcl_cout << ec.t(0.0,5.70955-0.88) << ' ' << ec.t(0.0,8.91894-0.88)<< '\n';
#endif
#if 1
  // Set up the scenes
  vcl_string base_dir_path = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/";
  //vcl_string orbit_scene_path = base_dir_path + "orbit/orbit.25.xml";
  //vcl_string target_scene_path = base_dir_path + "orbit/target_orbit.25.xml";
  vcl_string orbit_scene_path = base_dir_path + "orbit/orbit.xml";
  vcl_string target_scene_path = base_dir_path + "orbit/target_orbit.xml";
  if(!vul_file::exists(orbit_scene_path))
  {
      vcl_cout<<"orbit scene file) does not exist"<<vcl_endl;
      return;
  }
  //bool init = false;
  bool init = true;
  boxm2_vecf_orbit_scene* orbit = new boxm2_vecf_orbit_scene(orbit_scene_path, init);
  if(init)
    boxm2_cache::instance()->write_to_disk();
#if 0
  boxm2_scene_sptr target_scene = new boxm2_scene(target_scene_path);
  boxm2_cache::instance()->add_scene(target_scene);
  orbit->map_to_target(target_scene);
  boxm2_cache::instance()->write_to_disk();
#endif
#endif
#endif //BUILD_TEST_ORBIT
}
TESTMAIN( test_orbit );

