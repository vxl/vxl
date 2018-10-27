//:
// \file
// \author J.L. Mundy
// \date 03/12/15


#include <fstream>
#include <string>
#include <testlib/testlib_test.h>
#include <vul/vul_timer.h>
#include "../boxm2_vecf_middle_fat_pocket.h"
#include "../boxm2_vecf_middle_fat_pocket_params.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

//#define BUILD_TEST_MIDDLE_FAT_POCKET
void test_middle_fat_pocket()
{
#ifdef BUILD_TEST_MIDDLE_FAT_POCKET
  std::string scene_base_path = "D:/VisionSystems/Janus/experiments/vector_flow/fat_pocket/";
  std::string geo_base_path = "D:/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/fat_pocket/";
  std::string param_path = geo_base_path + "middle_fat_pocket_params.txt";
  std::string geo_path = scene_base_path+ "middle_fat_pocket_geo.txt";
  std::string rand_pts_path = geo_base_path + "random_pocket_points.txt";
  boxm2_vecf_middle_fat_pocket fpoc(geo_path);
  boxm2_vecf_middle_fat_pocket_params fparams;
  fparams.fit_to_subject_ = false;
  fparams.lambda_ = 0.9;
  fparams.gamma_ = 0.6;
  fparams.principal_offset_ = 10.0;
  fpoc.set_params(fparams);
  //  fpoc.apply_deformation_params();
  //  fpoc.print_vf_centroid_scan(0.0);
#if 1
  boxm2_vecf_middle_fat_pocket def_poc = fpoc.deform();
  vgl_pointset_3d<double> ptset = def_poc.random_pointset(100000);
  std::ofstream ostr(rand_pts_path.c_str());
  if(ostr){
    ostr << ptset;
    ostr.close();
  }else
   std::cout << "couldn't open " << rand_pts_path << '\n';
#endif
#endif
}
TESTMAIN( test_middle_fat_pocket);
