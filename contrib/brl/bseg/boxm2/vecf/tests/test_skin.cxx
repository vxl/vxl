//:
// \file
// \author J.L. Mundy
// \date 03/12/15


#include <iostream>
#include <fstream>
#include <string>
#include <testlib/testlib_test.h>
#include <vul/vul_timer.h>
#include "../boxm2_vecf_skin_params.h"
#include "../boxm2_vecf_skin_scene.h"
#include "../boxm2_vecf_skin.h"
#include <vgl/vgl_point_3d.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

//#define BUILD_TEST_SKIN
void test_skin()
{
#ifdef BUILD_TEST_SKIN
  std::string pc_dir = "d:/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/skull/";
  std::string scene_dir = "d:/VisionSystems/Janus/experiments/vector_flow/skin/";
  std::string skin_scene_path = scene_dir + "skin.xml";
  //std::string skin_scene_path = scene_dir + "skin_orig.xml";
  std::string vrml_path = pc_dir + "skin_vrml.wrl";

  if(!vul_file::exists(skin_scene_path))
    {
      std::cout<<"skin scene file does not exist"<<std::endl;
      return;
    }
  std::string skin_geo_path = scene_dir + "skin_geo.txt";
  if(!vul_file::exists(skin_geo_path))
    {
      std::cout<<"skin geo file does not exist"<<std::endl;
      return;
    }

  boxm2_vecf_skin_scene skscene(skin_scene_path, skin_geo_path);
  //  boxm2_vecf_skin_scene skscene(skin_scene_path);
  boxm2_scene_sptr scene = skscene.scene();
  if(!scene)
    return;
  boxm2_cache::instance()->write_to_disk();

#if 0
  //// create vrml from existing geo
  boxm2_vecf_skin skin(skin_geo_path);
  std::ofstream ostr(vrml_path.c_str());
  skin.display_vrml(ostr);
  //////
  //  create geo from existing scene
  std::string skin_pc_path = pc_dir + "skin_orig_appearance.txt";
  std::ofstream ostr(skin_pc_path.c_str());
  if(!ostr){
    std::cout << "couldn't open output stream for " << skin_pc_path << "\n";
    return;
  }
  skscene.export_point_cloud_with_appearance(ostr);
  ostr.close();
  //  create from existing scene
#endif
#if 0

  std::string target_scene_path = scene_dir + "target_skin.xml";

  if(!vul_file::exists(target_scene_path))
    {
      std::cout<<"target scene file) does not exist"<<std::endl;
      return;
    }
  boxm2_scene_sptr target = new boxm2_scene(target_scene_path);
  if(!target)
    return;
  skscene.map_to_target(target);

  boxm2_cache::instance()->write_to_disk();
#endif
#endif
}
TESTMAIN( test_skin );
