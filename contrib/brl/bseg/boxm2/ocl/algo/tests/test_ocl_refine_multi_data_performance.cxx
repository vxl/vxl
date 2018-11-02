//:
// \file
// Tests the performance of boxm2_ocl_refine in comparison to
// boxm2_refine_block_multi_data.
//
// \author Raphael Kargon
// \date July 17th, 2017

#include <string>
#include <vector>
#include <iostream>
#include <ctime>
#include <testlib/testlib_root_dir.h>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bocl/bocl_manager.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/cpp/algo/boxm2_refine_block_multi_data.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/ocl/algo/boxm2_ocl_refine.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

void test_ocl_refine_multi_data_performance() {
  /* TEST OCL REFINE  */
  testlib_test_begin(
      "Testing performance of OCL Refinement and CPP Multi-Data Refinement.");
  std::clock_t start, end;

  std::string data_dir =
      testlib_root_dir() + "/contrib/brl/bseg/boxm2/ocl/algo/tests/";
  boxm2_scene_sptr scene_ocl =
      new boxm2_scene(data_dir + "/boxm2_site_ocl_refine/scene.xml");
  boxm2_lru_cache::create(scene_ocl.ptr());
  bocl_manager_child &mgr = bocl_manager_child::instance();
  testlib_test_assert("Init manager", mgr.gpus_.size() != 0);

  bocl_device_sptr device = mgr.gpus_[0];
  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(device);

  // Scene's initial probability is 0.01, so this should refine everything
  float prob_thresh = 0.00001f;
  start = std::clock();
  int num_cells_refined_regular = boxm2_ocl_refine::refine_scene(
      device, scene_ocl, opencl_cache, prob_thresh);
  end = std::clock();
  std::cout << "Elapsed time for OCL refine: " << float(end - start) / CLOCKS_PER_SEC << std::endl;
  testlib_test_assert("boxm2_ocl_refine", num_cells_refined_regular != -1);

  /* TEST MULTI-DATA REFINE  */
  // reload scene
  boxm2_scene_sptr scene_cpp =
      new boxm2_scene(data_dir + "/boxm2_site_cpp_multi_data_refine/scene.xml");
  boxm2_lru_cache::instance()->add_scene(scene_cpp);

  // Scene has only one block, so we get the first one
  boxm2_block_id first_block_id = scene_cpp->blocks().begin()->first;
  boxm2_block_sptr first_block =
      boxm2_lru_cache::instance()->get_block(scene_cpp, first_block_id);
  // Need to ref this because lru cache internally uses raw pointers and owns its data. We don't want smart pointers to delete the data a second time.
  first_block->ref();

  std::vector<std::string> appearances = scene_cpp->appearances();
  appearances.push_back(boxm2_data_traits<BOXM2_ALPHA>::prefix());
  start = std::clock();
  boxm2_refine_block_multi_data_function(scene_cpp, first_block, appearances,
                                         prob_thresh);
  end = std::clock();
  std::cout << "Elapsed time for Multi-Data refine: " << float(end - start) / CLOCKS_PER_SEC << std::endl;
}

TESTMAIN(test_ocl_refine_multi_data_performance);
