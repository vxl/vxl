//:
// \file
// \author J.L. Mundy
// \date 10/12/14

#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vul/vul_timer.h>
#include <bocl/bocl_manager.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_util.h>
#include <vul/vul_file.h>
#include "../boxm2_vecf_ocl_filter.h"

typedef vnl_vector_fixed<unsigned char, 16> uchar16;
void test_filter()
{

  // Set up the scenes
  //  std::string base_dir_path = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/putin_face/";
        //  std::string base_dir_path = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/mean_face/";
        //  std::string source_scene_path = base_dir_path + "filter_source_scene.xml";
  //  std::string temp_scene_path = base_dir_path + "filter_temp_scene.xml";
        std::string base_dir_path = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/eye/";
        std::string source_scene_path = base_dir_path + "eye.xml";
  if(!vul_file::exists(source_scene_path))
  {
      std::cout<<"One or both of the secene files do not exist"<<std::endl;
      return;
  }
  boxm2_scene_sptr source_scene = new boxm2_scene(source_scene_path);
  //  boxm2_scene_sptr temp_scene = new boxm2_scene(temp_scene_path);
  boxm2_scene_sptr temp_scene = source_scene->clone_no_disk();
  boxm2_lru_cache::create(source_scene);

  bocl_manager_child &mgr = bocl_manager_child::instance();
  unsigned gpu_idx = 1; //on JLM's alienware
  bocl_device_sptr device = mgr.gpus_[gpu_idx];
  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(device);
  boxm2_vecf_ocl_filter f(source_scene, temp_scene, opencl_cache);
  std::vector<float> coefs(8,0.125f);
  coefs[0]=0.25f;   coefs[7]=0.0f;
  f.filter(coefs, 3);

}

TESTMAIN( test_filter );
