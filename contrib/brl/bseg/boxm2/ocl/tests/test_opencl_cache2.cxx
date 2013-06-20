//:
// \file
// \author Vishal Jain
// \date 6/17/13

#include "boxm2_ocl_test_utils.h"

#include <testlib/testlib_test.h>

#include <bocl/bocl_manager.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/ocl/boxm2_opencl_cache2.h>
#include <boxm2/boxm2_util.h>


#include <bvpl/kernels/bvpl_gauss3d_kernel_factory.h>
#include <bvpl/kernels/bvpl_gauss3d_x_kernel_factory.h>


typedef vnl_vector_fixed<unsigned char, 16> uchar16;

bool test_opencl_cache2()
{
  //create a simple scene
  vcl_string scene_file1 = boxm2_ocl_test_utils::save_test_simple_scene("test1.xml");
  vcl_string scene_file2 = boxm2_ocl_test_utils::save_test_simple_scene("test2.xml");
  //set up openCL
  bocl_manager_child_sptr mgr = bocl_manager_child::instance();

  unsigned gpu_idx = 0;

  if (mgr->numGPUs()<1) {
    vcl_cerr << "No GPU available\n";
    return false;
  }
  if (mgr->numGPUs()==1) gpu_idx = 0;
  if (mgr->numGPUs()==2) gpu_idx = 1;

  bocl_device_sptr device = mgr->gpus_[gpu_idx];
  boxm2_scene_sptr scene1 = new boxm2_scene(scene_file1);
  boxm2_scene_sptr scene2 = new boxm2_scene(scene_file2);

  boxm2_lru_cache2::create(scene1);
  boxm2_lru_cache2::create(scene2);
  boxm2_opencl_cache2_sptr opencl_cache = new boxm2_opencl_cache2(device);
  //iterate through response blocks
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks1 = scene1->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter1;
  bool result = true;
  for (blk_iter1 = blocks1.begin(); blk_iter1 != blocks1.end(); ++blk_iter1)
  {
      boxm2_block_id id = blk_iter1->first;
      vcl_cout << "Printing results for block " << id << ':' << vcl_endl;
      bocl_mem * blk_mem = opencl_cache->get_block(scene1, id);
      

      vcl_cout<<"Status "<<opencl_cache->to_string()<<vcl_endl;
      if(!blk_mem)
          result = false; 
  }
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks2 = scene2->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter2;
  for (blk_iter2 = blocks2.begin(); blk_iter2 != blocks2.end(); ++blk_iter2)
  {
      boxm2_block_id id = blk_iter2->first;
      vcl_cout << "Printing results for block " << id << ':' << vcl_endl;
      bocl_mem * blk_mem = opencl_cache->get_block(scene2, id);

      vcl_cout<<"Status "<<opencl_cache->to_string()<<vcl_endl;
      if(!blk_mem)
          result = false; 
  }
  return result;
}

TESTMAIN( test_opencl_cache2 );
