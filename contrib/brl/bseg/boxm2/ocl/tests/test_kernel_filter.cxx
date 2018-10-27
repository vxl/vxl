//:
// \file
// \author Isabel Restrepo
// \date 4/16/12

#include "boxm2_ocl_test_utils.h"

#include <testlib/testlib_test.h>

#include <bocl/bocl_manager.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/ocl/pro/processes/boxm2_ocl_kernel_filter_process.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>


#include <bvpl/kernels/bvpl_gauss3d_kernel_factory.h>
#include <bvpl/kernels/bvpl_gauss3d_x_kernel_factory.h>


typedef vnl_vector_fixed<unsigned char, 16> uchar16;

bool test_gauss_filter()
{
  //create a simple scene
  boxm2_scene_sptr scene = new boxm2_scene();
  float val1 = 0.3f, val2 = 0.6f;
  boxm2_ocl_test_utils::create_edge_scene(scene, val1, val2);
  boxm2_ocl_test_utils::print_alpha_scene(scene);

  //create a bvpl_kernel
  float axis_x = 1.0, axis_y = 0.0, axis_z = 0.0;
  vnl_float_3 axis(axis_x, axis_y, axis_z);

  //Create the factory
  bvpl_gauss3d_kernel_factory factory(2.0, 2.0, 2.0);
  factory.set_rotation_axis(axis);
  factory.set_angle(0.0);

  bvpl_kernel_sptr filter = new bvpl_kernel(factory.create());
  //filter->print();

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
  boxm2_lru_cache::create(scene);
  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache( device );
  boxm2_ocl_kernel_filter_process_globals::process(device, scene, opencl_cache, filter);

  //verify values of the response

  //iterate through response blocks
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  bool result = true;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    std::cout << "Printing results for block " << id << ':' << std::endl;

    boxm2_block *     blk     = boxm2_cache::instance()->get_block(scene,id);
    std::stringstream filter_ident; filter_ident << filter->name() << '_' << filter->id();
    boxm2_data_base * response    = boxm2_cache::instance()->get_data_base(scene,id, boxm2_data_traits<BOXM2_FLOAT>::prefix(filter_ident.str()));
    boxm2_block_metadata data = blk_iter->second;

    //3d array of trees
    const boxm2_array_3d<uchar16>& trees = blk->trees();
    auto * response_data = (boxm2_data_traits<BOXM2_FLOAT>::datatype*) response->data_buffer();

    //iterate through each block, filtering the root level first

    for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
      std::cout << '[' << x << '/' << trees.get_row1_count() << ']' << std::endl;
      for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
        for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
          //load current block/tree
          uchar16 tree = trees(x, y, z);
          boct_bit_tree bit_tree((unsigned char*) tree.data_block(), data.max_level_);

          //FOR ALL LEAVES IN CURRENT TREE
          std::vector<int> leafBits = bit_tree.get_leaf_bits();
          std::vector<int>::iterator iter;
          for (iter = leafBits.begin(); iter != leafBits.end(); ++iter)
          {
            int currBitIndex = (*iter);
            int currIdx = bit_tree.get_data_index(currBitIndex);

            result = result && ( (std::abs(response_data[currIdx]-val1) < 1e-5) ||(std::abs(response_data[currIdx]-val2) < 1e-5) || (std::abs(response_data[currIdx]-0.0f) < 1e-5));

            //std::cout << " P(leaf[" << currIdx << "])=" << response_data[currIdx] << std::endl;
          }
        }
      }
    }
  }

  boxm2_cache::instance()->clear_cache();
  opencl_cache->clear_cache();
  boxm2_ocl_test_utils::clean_up();
  return result;
}

bool test_gauss_x_filter()
{
  //create a simple scene
  boxm2_scene_sptr scene = new boxm2_scene();

  boxm2_lru_cache::create(scene);

  float val1 = 0.3f, val2 = 0.6f;
  boxm2_ocl_test_utils::create_edge_scene(scene, val1, val2);
  boxm2_ocl_test_utils::print_alpha_scene(scene);

  //create a bvpl_kernel
  float axis_x = 1.0, axis_y = 0.0, axis_z = 0.0;
  vnl_float_3 axis(axis_x, axis_y, axis_z);

  //Create the factory
  bvpl_gauss3d_x_kernel_factory factory(1.0, 1.0, 1.0);
  factory.set_rotation_axis(axis);
  factory.set_angle(0.0);

  bvpl_kernel_sptr filter = new bvpl_kernel(factory.create());
  //filter->print();

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
  boxm2_lru_cache::create(scene);
  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(device);
  boxm2_ocl_kernel_filter_process_globals::process(device, scene, opencl_cache, filter);

  //verify values of the response

  //iterate through response blocks
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  bool result = true;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    std::cout << "Printing results for block " << id << ':' << std::endl;

    boxm2_block *     blk     = boxm2_cache::instance()->get_block(scene,id);
    std::stringstream filter_ident; filter_ident << filter->name() << '_' << filter->id();
    boxm2_data_base * response    = boxm2_cache::instance()->get_data_base(scene,id, boxm2_data_traits<BOXM2_FLOAT>::prefix(filter_ident.str()));
    boxm2_block_metadata data = blk_iter->second;

    //3d array of trees
    const boxm2_array_3d<uchar16>& trees = blk->trees();
    auto * response_data = (boxm2_data_traits<BOXM2_FLOAT>::datatype*) response->data_buffer();

    //iterate through each block, filtering the root level first

    for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
      std::cout << '[' << x << '/' << trees.get_row1_count() << ']' << std::endl;
      for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
        for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
          //load current block/tree
          uchar16 tree = trees(x, y, z);
          boct_bit_tree bit_tree((unsigned char*) tree.data_block(), data.max_level_);

          //FOR ALL LEAVES IN CURRENT TREE
          std::vector<int> leafBits = bit_tree.get_leaf_bits();
          std::vector<int>::iterator iter;
          for (iter = leafBits.begin(); iter != leafBits.end(); ++iter)
          {
            int currBitIndex = (*iter);
            int currIdx = bit_tree.get_data_index(currBitIndex);

            result = result && ((std::abs(response_data[currIdx]-0.0f) < 1e-5));

            //std::cout << "P(leaf[" << currIdx << "])=" << response_data[currIdx] << std::endl;
          }
        }
      }
    }
  }

  boxm2_cache::instance()->clear_cache();
  opencl_cache->clear_cache();

  boxm2_ocl_test_utils::clean_up();
  return result;
}

void test_kernel_filter()
{
  TEST("Gauss response has a valid value", test_gauss_filter(), true);
#if 0
  TEST("Gauss_x response has a valid value", test_gauss_x_filter(), true);
#endif
}

TESTMAIN( test_kernel_filter );
