//:
// \file
// \author J.L. Mundy
// \date 02/20/16


#include <fstream>
#include <string>
#include <testlib/testlib_test.h>
#include <vul/vul_timer.h>
#include "../boxm2_vecf_composite_face_params.h"
#include "../boxm2_vecf_fit_face.h"
#include <vgl/vgl_point_3d.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

//#define BUILD_TEST_FIT_FACE
void test_fit_face()
{
#ifdef BUILD_TEST_FIT_FACE

  std::string pc_dir = "d:/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/";
  //  std::string source_pc_path = pc_dir + "cristina/cristina_pc.txt";
  //std::string source_pc_path = pc_dir + "bs000_N_N_1/bs000_N_N_1_pc.txt";
  std::string source_pc_path = pc_dir + "bs000_E_HAPPY_0/bs000_E_HAPPY_0_pc.txt";
  //  std::string targ_pc_path = pc_dir + "cristina/cristina_inv_trans.txt";
  //std::string targ_pc_path = pc_dir + "bs000_N_N_1/bs000_N_N_1_inv_trans.txt";
  std::string targ_pc_path = pc_dir + "bs000_E_HAPPY_0/bs000_E_HAPPY_0_inv_trans.txt";
  //  std::string params_path = pc_dir + "cristina/cristina_face_params.txt";
  //std::string params_path = pc_dir + "bs000_N_N_1/bs000_N_N_1_face_params.txt";
  std::string params_path = pc_dir + "bs000_E_HAPPY_0/bs000_E_HAPPY_0_face_params.txt";
  std::string scene_dir = "d:/VisionSystems/Janus/experiments/vector_flow/composite_face/";
  std::string scene_paths = scene_dir + "face_scene_paths.txt";
  if(!vul_file::exists(source_pc_path)){
      std::cout<<"pc path  " << source_pc_path << " does not exist"<<std::endl;
      return;
  }
  if(!vul_file::exists(params_path)){
      std::cout<<"params path  " << params_path << " does not exist"<<std::endl;
      return;
  }
  boxm2_vecf_fit_face fitter;
  if(!fitter.load_composite_face_params(params_path))
    return;
  if(!fitter.inverse_transform_face(source_pc_path, targ_pc_path))
    return;
#endif
}
TESTMAIN( test_fit_face );
