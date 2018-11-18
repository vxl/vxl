//:
// \file
// \author J.L. Mundy
// \date 11/19/14


#include <string>
#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "../boxm2_vecf_point_cloud_orbit_viewer.h"
#include "../boxm2_vecf_orbit_params.h"

//#define BUILD_TEST_PC_VIEWER
void test_pc_viewer()
{
#ifdef BUILD_TEST_PC_VIEWER
  boxm2_vecf_point_cloud_orbit_viewer pcv;
  std::string base_dir ="c:/Users/mundy/VisionSystems/Janus/experiments/Helena/";
  std::string pc_in_str = "linden/linden_sampled_points.txt";
  std::string pc_in_path = base_dir + pc_in_str;
  std::string pc_out_str = "linden/linden_sampled_points_orbit_disp.txt";
  std::string pc_out_path = base_dir + pc_out_str;
  std::string pc_left_param_str = "linden/linden_left_orbit_params.txt";
  std::string pc_left_param_path = base_dir + pc_left_param_str;
   pcv.set_point_cloud(pc_in_path);
  std::ifstream istr(pc_left_param_path.c_str());
  if(!istr)
    return;
  boxm2_vecf_orbit_params left_params;
  istr >> left_params;
  pcv.display_orbit(left_params, false);
  pcv.save_point_cloud(pc_out_path);
#endif
}
TESTMAIN( test_pc_viewer );
