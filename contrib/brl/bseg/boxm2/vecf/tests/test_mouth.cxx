//:
// \file
// \author J.L. Mundy
// \date 03/12/15


#include <string>
#include <fstream>
#include <testlib/testlib_test.h>
#include <vul/vul_timer.h>
#include "../boxm2_vecf_mouth.h"
#include "../boxm2_vecf_mandible_params.h"
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

//#define BUILD_TEST_MOUTH
void test_mouth()
{
#ifdef BUILD_TEST_MOUTH
  std::string pc_dir = "c:/Users/mundy/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/skull/";
  std::string scene_dir = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/mouth/";
  std::string mouth_geo_path = scene_dir + "mouth_geo.txt";
  //std::string mouth_pt_path = pc_dir + "skin-trans-sampled-mandible-coupling-tooth-contour.txt";
  std::string mouth_pt_path = pc_dir + "skin-trans-sampled-mandible-coupling-lip-contour.txt";
  std::string mouth_display_pc_path = pc_dir + "mouth_open_v2.txt";
  if(!vul_file::exists(mouth_pt_path))
    {
      std::cout<<"mouth knot file does not exist"<<std::endl;
      return;
    }
  vgl_pointset_3d<double> ptset;
  std::ifstream istr(mouth_pt_path.c_str());
  if(!istr)
    return;
  istr >> ptset;
  istr.close();
  boxm2_vecf_mouth mouth(ptset);
  boxm2_vecf_mandible_params params;
  params.jaw_opening_angle_rad_ = 0.5;
  mouth.set_mandible_params(params);
  unsigned n_pts = 1000;
  vgl_pointset_3d<double> mouth_pts = mouth.random_pointset(n_pts);
  std::ofstream ostr(mouth_display_pc_path.c_str());
  if(!ostr)
    return;
  ostr << mouth_pts;
  ostr.close();
  params.jaw_opening_angle_rad_ = 0.0;
  mouth.set_mandible_params(params);
#endif
}
TESTMAIN( test_mouth);
