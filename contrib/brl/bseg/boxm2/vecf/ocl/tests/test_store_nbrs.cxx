//:
// \file
// \author J.L. Mundy
// \date 10/12/14


#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vul/vul_timer.h>
#include <bocl/bocl_manager.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_util.h>
#include <vul/vul_file.h>
#include "../boxm2_vecf_ocl_store_nbrs.h"

typedef vnl_vector_fixed<unsigned char, 16> uchar16;

void test_store_nbrs()
{
  // Set up the scenes
  std::string base_dir_path = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/eye/";
  //  std::string base_dir_path = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/mean_face/";
  //std::string source_scene_path = base_dir_path + "source_scene.xml";
    std::string source_scene_path = base_dir_path + "eye.xml";
  if(!vul_file::exists(source_scene_path))
  {
      std::cout<<"source scene file does not exist"<<std::endl;
      return;
  }
  boxm2_scene_sptr source_scene = new boxm2_scene(source_scene_path);
  boxm2_lru_cache::create(source_scene);

  bocl_manager_child &mgr = bocl_manager_child::instance();
  unsigned gpu_idx = 1; //on JLM's alienware
  bocl_device_sptr device = mgr.gpus_[gpu_idx];
  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(device);
  boxm2_vecf_ocl_store_nbrs snbrs(source_scene, opencl_cache);
  snbrs.augment_1_blk();
}
TESTMAIN( test_store_nbrs );
