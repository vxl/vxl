//:
// \file
// \author J.L. Mundy
// \date 03/12/15


#include <fstream>
#include <string>
#include <testlib/testlib_test.h>
#include <vul/vul_timer.h>
#include "../boxm2_vecf_middle_fat_pocket_params.h"
#include "../boxm2_vecf_middle_fat_pocket_scene.h"
#include "../boxm2_vecf_middle_fat_pocket.h"
#include <vgl/vgl_point_3d.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

//#define BUILD_TEST_FAT_POCKET_SCENE
void test_fat_pocket_scene()
{
#ifdef BUILD_TEST_FAT_POCKET_SCENE
  std::string pc_dir = "d:/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/fat_pocket/";
  std::string scene_dir = "d:/VisionSystems/Janus/experiments/vector_flow/fat_pocket/";
  std::string middle_fat_pocket_scene_path = scene_dir + "fat_pocket_8.xml";
  if(!vul_file::exists(middle_fat_pocket_scene_path))
    {
      std::cout<<"middle_fat_pocket scene file does not exist"<<std::endl;
      return;
    }
  std::string middle_fat_pocket_geo_path = scene_dir + "middle_fat_pocket_geo.txt";
  if(!vul_file::exists(middle_fat_pocket_geo_path))
    {
      std::cout<<"middle_fat_pocket geo file does not exist"<<std::endl;
      return;
    }

  boxm2_vecf_middle_fat_pocket_scene fpscene(middle_fat_pocket_scene_path, middle_fat_pocket_geo_path, true);
 // boxm2_vecf_middle_fat_pocket_scene fpscene(middle_fat_pocket_scene_path);
  boxm2_scene_sptr scene = fpscene.scene();
  if(!scene)
    return;
  //  boxm2_cache::instance()->write_to_disk();

#if 0  // debug
  boxm2_vecf_middle_fat_pocket_params params;
  boxm2_vecf_articulated_params& aparams = params;
  params.lambda_ = 0.5;
  //params.lambda_ = 1.0;
  fpscene.set_params(aparams);
  fpscene.print_vf_centroid_scan(25.0);


  std::string target_scene_path = scene_dir + "target_fat_pocket_8.xml";

  if(!vul_file::exists(target_scene_path))
    {
      std::cout<<"target scene file) does not exist"<<std::endl;
      return;
    }
  boxm2_scene_sptr target = new boxm2_scene(target_scene_path);
  if(!target)
    return;
  fpscene.map_to_target(target);

  boxm2_cache::instance()->write_to_disk();
#endif
#endif
}
TESTMAIN( test_fat_pocket_scene );
