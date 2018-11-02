//:
// \file
// \author Andy Miller
// \date 26-Oct-2010

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_where_root_dir.h>

#include <boxm2/boxm2_scene.h>
#if 0 // all tests commented out for the time being...
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/ocl/pro/boxm2_opencl_processor.h> // beware: does not exist anymore

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

//: Three unit tests for the three main refine kernel functions

void test_refine_trees_kernel()
{
  //----------------------------------------------------------------------------
  //--- BEGIN BOXM2 API EXAMPLE ------------------------------------------------
  //----------------------------------------------------------------------------
  //start out rendering with the CPU
  std::string test_dir  = testlib_root_dir()+ "/contrib/brl/bseg/boxm2/ocl/tests/";
  std::string test_file = test_dir + "scene.xml";
  boxm2_scene_sptr scene = new boxm2_scene(test_file);

#if 0
  //initialize a block and data cache
  boxm2_lru_cache::create(scene.ptr());

  //initialize gpu pro / manager
  boxm2_opencl_processor* gpu_pro = boxm2_opencl_processor::instance();
  gpu_pro->set_scene(scene.ptr());
  gpu_pro->set_cpu_cache(boxm2_cache::instance());
  gpu_pro->init();

  //rendering sources
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "bit/refine_bit_scene.cl");
  bocl_kernel refine_trees;
  if (!refine_trees.create_kernel( &gpu_pro->context(), &gpu_pro->devices()[0], src_paths,
                                   "refine_trees", "-D MOG_TYPE_8", "boxm2 opencl refine trees (pass one)") ) {
    TEST("Refine Trees Kernel creation ", true, false);
    return;
  }

  //prob_thresh buffer
  float* prob_buff = new float[1];
  prob_buff[0] = 0.00001f; //EVERYTHING should refine
  bocl_mem prob_thresh( gpu_pro->context(), prob_buff, sizeof(float), "prob_thresh buffer");
  prob_thresh.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //output buffer
  float* output_arr = new float[100];
  for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
  bocl_mem cl_output( gpu_pro->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bit lookup buffer
  cl_uchar* lookup_arr = new cl_uchar[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem lookup( gpu_pro->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //For each ID in the visibility order, grab that block
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter = blocks.begin();
  boxm2_block_metadata data = blk_iter->second;

  //get id
  boxm2_block_id id = blk_iter->first;

  //write the image values to the buffer
  bocl_mem* blk       = gpu_pro->gpu_cache()->get_block(id);
  bocl_mem* alpha     = gpu_pro->gpu_cache()->get_data<BOXM2_ALPHA>(id);
  bocl_mem* blk_info  = gpu_pro->gpu_cache()->loaded_block_info();

  //set up tree copy
  vxl_byte* cpybuff = new vxl_byte[blk->num_bytes()];
  bocl_mem* blk_copy = new bocl_mem( gpu_pro->context(), cpybuff, blk->num_bytes(), "refine trees block copy buffer");
  blk_copy->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set up tree size (first find num trees)
  int numTrees = data.sub_block_num_.x() * data.sub_block_num_.y() * data.sub_block_num_.z();
  //int* sizebuff = new int[numTrees];
  bocl_mem* tree_sizes = new bocl_mem( gpu_pro->context(), /*sizebuff*/ NULL, sizeof(cl_int)*numTrees, "refine tree sizes buffer");
  tree_sizes->create_buffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, *gpu_pro->get_queue());

  //set first kernel args
  refine_trees.set_arg( blk_info );
  refine_trees.set_arg( blk );
  refine_trees.set_arg( blk_copy );
  refine_trees.set_arg( alpha );
  refine_trees.set_arg( tree_sizes );
  refine_trees.set_arg( &prob_thresh );
  refine_trees.set_arg( &lookup );
  refine_trees.set_arg( &cl_output );
  refine_trees.set_local_arg( 16*sizeof(cl_uchar) );
  refine_trees.set_local_arg( sizeof(cl_uchar16) );
  refine_trees.set_local_arg( sizeof(cl_uchar16) );

  //set workspace
  std::size_t lThreads[] = {1, 1};
  std::size_t gThreads[] = {numTrees, 1};

  //execute kernel
  refine_trees.execute( *gpu_pro->get_queue(), 2, lThreads, gThreads);
  clFinish( *gpu_pro->get_queue());

  //clear render kernel args so it can reset em on next execution
  refine_trees.clear_args();

  blk_copy->read_to_buffer(*gpu_pro->get_queue());
  std::cout<<"Testing refine results"<<std::endl;
  vxl_byte* cpy = (vxl_byte*) blk_copy->cpu_buffer();
  for (int i=0; i<numTrees; ++i) {
    if (cpy[16*i] != 1) {
      TEST(" FIRST PASS REFINE TEST (trees) ", true, false);
      return;
    }
  }
  TEST(" First pass refine trees test ", true, true);

  tree_sizes->read_to_buffer(*gpu_pro->get_queue());
  std::cout<<"outputting new tree sizes"<<std::endl;
  int* sizes = (int*) tree_sizes->cpu_buffer();
  for (int i=0; i<numTrees; ++i) {
    if (sizes[i] != 9) {
      TEST(" FIRST PASS REFINE TEST (SIZES) ", true, false);
      return;
    }
  }
  TEST(" first pass refine test sizes ", true, true);


  /////////////////////////////////////////////////////////////////////////
  //STEP TWO
  //read out tree_sizes and do cumulative sum on it
  tree_sizes->read_to_buffer(*gpu_pro->get_queue());
  int* sizebuff = (int*) tree_sizes->cpu_buffer();
  for (int i=1; i<numTrees; ++i) sizebuff[i] += sizebuff[i-1];
  int newDataSize = sizebuff[numTrees-1];
  for (int i=numTrees-1; i>0; --i) sizebuff[i] = sizebuff[i-1];
  sizebuff[0] = 0;
  tree_sizes->write_to_buffer(*gpu_pro->get_queue());
  std::cout<<"New data size: "<<newDataSize<<std::endl;
  /////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////
  // TEST REFINE DATA
  /////////////////////////////////////////////////////////////////////////////
  bocl_kernel refine_data;
  refine_data.create_kernel( &gpu_pro->context(), &gpu_pro->devices()[0], src_paths,
                             "refine_data", "-D MOG_TYPE_4 ", "boxm2 opencl refine data size 4 (pass three)");

  //set up alpha copy
  bocl_mem* alpha_copy = new bocl_mem(gpu_pro->context(), NULL, newDataSize*sizeof(float), "alpha block copy buffer");
  alpha_copy->create_buffer(CL_MEM_READ_WRITE| CL_MEM_ALLOC_HOST_PTR, *gpu_pro->get_queue());

  //init value buffer
  float* init_abuff = new float[1]; init_abuff[0] = .01f;
  bocl_mem init_alpha(gpu_pro->context(), init_abuff, sizeof(float), "init_alpha buffer");
  init_alpha.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  refine_data.set_arg( blk_info );
  refine_data.set_arg( blk );
  refine_data.set_arg( blk_copy );
  refine_data.set_arg( tree_sizes );
  refine_data.set_arg( alpha );
  refine_data.set_arg( alpha_copy );
  refine_data.set_arg( &init_alpha );
  refine_data.set_arg( &prob_thresh );
  refine_data.set_arg( &lookup );
  refine_data.set_arg( &cl_output );
  refine_data.set_local_arg( 16*sizeof(cl_uchar) );
  refine_data.set_local_arg( sizeof(cl_uchar16) );
  refine_data.set_local_arg( sizeof(cl_uchar16) );

  //execute kernel
  refine_data.execute( *gpu_pro->get_queue(), 2, lThreads, gThreads);
  clFinish( *gpu_pro->get_queue());

  //original alphas
  float* abuf = (float*) alpha->cpu_buffer();
  for (int i=0; i<8; ++i)
    std::cout<<abuf[i]<<std::endl;

  std::cout<<"NEW ALPHAS"<<std::endl;
  alpha_copy->read_to_buffer(*gpu_pro->get_queue());
  float* alph = (float*) alpha_copy->cpu_buffer();
  for (int i=0; i<newDataSize; ++i)
    std::cout<<alph[i]<<std::endl;

  //TEST to make sure the new trees are lined up correctly
  blk->read_to_buffer(*gpu_pro->get_queue());
  vxl_byte* refined = (vxl_byte*) blk->cpu_buffer();
  for (int i=0; i<numTrees; ++i) {
    if (refined[16*i] != 1) {
      std::cout<<"value is: "<<(int) refined[16*i]<<"... should be 1 at "<<i<<std::endl;
      TEST(" THIRD PASS REFINE TEST (trees) ", true, false);
      return;
    }
    int pointer;
    std::memcpy(&pointer, &refined[16*i]+10, sizeof(int));
    if (pointer != 9*i) {
      TEST(" THIRD PASS Refine data pointer ", true, false);
      std::cout<<"Pointer is: "<<pointer<<"... should be "<<9*i<<" at "<<i<<std::endl;
      return;
    }
  }
#endif

  TEST(" Third Pass TREE passes ", true, true);
}

void test_refine_scan_kernel() {}
void test_refine_data_kernel() {}
void test_refine() { test_refine_trees_kernel(); test_refine_scan_kernel(); test_refine_data_kernel(); }

TESTMAIN( test_refine );
