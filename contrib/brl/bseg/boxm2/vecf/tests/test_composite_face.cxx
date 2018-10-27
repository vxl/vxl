//:
// \file
// \author J.L. Mundy
// \date 03/12/15


#include <fstream>
#include <string>
#include <iostream>
#include <testlib/testlib_test.h>
#include <vul/vul_timer.h>
#include "../boxm2_vecf_composite_face_params.h"
#include "../boxm2_vecf_composite_face_scene.h"
#include <vgl/vgl_point_3d.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#if 0 //archive
  vgl_point_3d<double> pmin(-123.95,-197.5, -217.5);
  vgl_point_3d<double> pmax(129.25,182.3, 128.54);
  vgl_box_3d<double> bb;
  bb.add(pmin); bb.add(pmax);
  cfscene.set_target_box(bb);
#endif

//#define BUILD_TEST_COMPOSITE_FACE
void test_composite_face()
{
#ifdef BUILD_TEST_COMPOSITE_FACE

  std::string pc_dir = "d:/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/";
  //std::string pc_path = pc_dir + "skull/skin_orig_appearance_sampled-r10-s1.05-t10.txt";
  std::string pc_path = pc_dir + "cristina/cristina_trans_face.txt";
  std::string params_path = pc_dir + "cristina/cristina_face_params.txt";
  std::string scene_dir = "d:/VisionSystems/Janus/experiments/vector_flow/composite_face/";
  std::string scene_paths = scene_dir + "face_scene_paths.txt";



  if(!vul_file::exists(pc_path)){
      std::cout<<"pc path  " << pc_path << " does not exist"<<std::endl;
      return;
  }

  if(!vul_file::exists(params_path)){
    std::cout<<"pc params file " << params_path << " does not exist"<<std::endl;
    return;
  }
  boxm2_vecf_composite_face_params params, cparams;
  std::ifstream pistr(params_path.c_str());
  pistr >> params;

  if(!vul_file::exists(scene_paths))
    {
      std::cout<<"scene paths file " << scene_paths << " does not exist"<<std::endl;
      return;
    }
  boxm2_vecf_composite_face_scene cfscene(scene_paths);
  cfscene.set_params(params);
  //boxm2_cache::instance()->write_to_disk();

  cfscene.compute_target_box(pc_path);
  cparams = cfscene.params();

  boxm2_scene_sptr tscene = cfscene.construct_target_scene(scene_dir, "target_face", "target_model", 8.44);

  cfscene.map_to_target(tscene);

  boxm2_cache::instance()->write_to_disk();
  std::cout << *tscene << '\n';

#if 0
  std::string target_scene_path = scene_dir + "target_face.xml";
    //std::string target_scene_path = scene_dir + "../cranium/target_cranium_8.xml";

  if(!vul_file::exists(target_scene_path))
    {
      std::cout<<"target scene file) does not exist"<<std::endl;
      return;
    }
  boxm2_scene_sptr target = new boxm2_scene(target_scene_path);

  if(!target)
    return;
  cfscene.map_to_target(target);

  boxm2_cache::instance()->write_to_disk();
#endif

#endif
}
TESTMAIN( test_composite_face );
