//:
// \file
// \author Vishal Jain
// \date 6/17/13

#include "boxm2_ocl_test_utils.h"

#include <testlib/testlib_test.h>

#include <bocl/bocl_manager.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_util.h>


#include <bvpl/kernels/bvpl_gauss3d_kernel_factory.h>
#include <bvpl/kernels/bvpl_gauss3d_x_kernel_factory.h>


typedef vnl_vector_fixed<unsigned char, 16> uchar16;

bool test_opencl_cache2()
{
  boxm2_scene_sptr scene1,scene2;
  boxm2_ocl_test_utils::create_test_simple_scene(scene1);
  boxm2_ocl_test_utils::create_test_simple_scene(scene2);
  //set up openCL
  bocl_manager_child &mgr = bocl_manager_child::instance();

  unsigned gpu_idx = 0;

  if (mgr.numGPUs()<1) {
    std::cerr << "No GPU available\n";
    return false;
  }
  if (mgr.numGPUs()==1) gpu_idx = 0;
  if (mgr.numGPUs()==2) gpu_idx = 1;

  bocl_device_sptr device = mgr.gpus_[gpu_idx];

  boxm2_lru_cache::create(scene1);
  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(device);
  //iterate through response blocks
  std::map<boxm2_block_id, boxm2_block_metadata> blocks1 = scene1->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter1;
  bool result = true;
  for (blk_iter1 = blocks1.begin(); blk_iter1 != blocks1.end(); ++blk_iter1)
  {
      boxm2_block_id id = blk_iter1->first;
      std::cout << "Printing results for block " << id << ':' << std::endl;
      bocl_mem*  blk_mem = opencl_cache->get_block(scene1, id);
      std::cout<<"Status "<<opencl_cache->to_string()<<std::endl;
      if(!blk_mem)     result = result && false;

      opencl_cache->unref_mem(blk_mem);
  }
  TEST("Scene 1 getting blocks ", result, true );
  std::map<boxm2_block_id, boxm2_block_metadata> blocks2 = scene2->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter2;
  for (blk_iter2 = blocks2.begin(); blk_iter2 != blocks2.end(); ++blk_iter2)
  {
      boxm2_block_id id = blk_iter2->first;
      std::cout << "Printing results for block " << id << ':' << std::endl;
      bocl_mem*  blk_mem = opencl_cache->get_block(scene2, id);


      std::cout<<"Status "<<opencl_cache->to_string()<<std::endl;
      if(!blk_mem)      result = result && false;

      opencl_cache->unref_mem(blk_mem);
  }
  opencl_cache->get_cpu_cache()->clear_cache();
  TEST("Scene 2 getting blocks ", result, true );
  return result;
}

TESTMAIN( test_opencl_cache2 );
