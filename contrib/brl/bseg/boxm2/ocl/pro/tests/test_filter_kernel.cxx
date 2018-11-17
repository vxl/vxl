//:
// \file
// \author Andrew Miller
// \date May 24, 2011
#include <iostream>
#include <ios>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_where_root_dir.h>

#include <boct/boct_bit_tree.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_buffer_mgr.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/pro/processes/boxm2_ocl_filter_process.h>

#include <boxm2/ocl/pro/boxm2_ocl_processes.h>

#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/io/boxm2_sio_mgr.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_vector_fixed.h>
#include <brdb/brdb_value.h>


void print_probs(boxm2_block* blk, const float* alphas, boxm2_block_metadata data, int /*dataSize*/)
{
  //print in 3x3
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;
  std::cout.precision(4);
  std::cout.setf(std::ios::fixed, std::ios::floatfield);   // floatfield set to fixed
  for (int j=0; j<3; ++j) {
    std::cout<<'|';

    float side_len = 1.0f;
    for (int i=0; i<3; ++i) {
      float prob=0.0f;
      uchar16 tree = blk->trees()(i,j,0);
      boct_bit_tree bit_tree( (unsigned char*) tree.data_block(), 3 );
      int idx = bit_tree.get_data_ptr();

      if ( bit_tree.is_leaf(0) ) {
        prob = 1.0 - std::exp(-alphas[idx] * data.sub_block_dim_.x());
      }
      else {
        float totalAlpha = 0.0f;
        for (int ci=0; ci<8; ++ci) {
          if ( bit_tree.is_leaf(ci+1) ) {
            side_len = 1.0/2.0f;
            totalAlpha += alphas[ci+1];
          }
          else {
            int giStart = (ci+1)*8+1;
            side_len = 1.0/4.0;
            for (int gi=0; gi<8; ++gi)
              totalAlpha += alphas[giStart+gi];
          }
        }
        prob = 1.0-std::exp(-totalAlpha * data.sub_block_dim_.x() * side_len);
      }
      std::cout<<'('<<i<<','<<j<<") = "<<prob<<'|';
    }
    std::cout<<std::endl;
  }
}

void print_alphas(boxm2_block* blk, const float* alphas, const boxm2_block_metadata& /*data*/, int /*dataSize*/)
{
  //print in 3x3
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;
  std::cout.precision(4);
  std::cout.setf(std::ios::fixed, std::ios::floatfield);   // floatfield set to fixed
  for (int j=0; j<3; ++j) {
    std::cout<<'|';
    for (int i=0; i<3; ++i) {

      float prob=0.0f;
      uchar16 tree = blk->trees()(i,j,0);
      boct_bit_tree bit_tree( (unsigned char*) tree.data_block(), 3 );
      int idx = bit_tree.get_data_ptr();
      if ( bit_tree.is_leaf(0) ) {
        prob = alphas[idx];
      }
      else {
        float totalAlpha = 0.0f;
        for (int ci=0; ci<8; ++ci) {
          if ( bit_tree.is_leaf(ci+1) ) {
            totalAlpha += alphas[ci+1];
          }
          else {
            int giStart = (ci+1)*8 + 1;
            for (int gi=0; gi<8; ++gi)
              totalAlpha += alphas[giStart+gi];
          }
        }
        prob = totalAlpha;
      }
      std::cout<<'('<<i<<','<<j<<") = "<<prob<<'|';
    }
    std::cout<<std::endl;
  }
}

/////////////////////////////////////////////////////////////////////////////
// set up data array, set up looks like this:
//  ___ ___ ___
// |   |   |   |
// |___|___|___|
// |   |_|_|   |
// |___|_|_|___|
// |   |   |   |
// |___|___|___|
//
// Each outer cell will need the sub tree combo of the inner cell - and some
// will store it as their own (will be the median).
void test_inner_cluster(const boxm2_scene_sptr& scene,
                        boxm2_block* blk,
                        boxm2_block_metadata& data,
                        bocl_device_sptr& device,
                        boxm2_opencl_cache_sptr& opencl_cache,
                        const boxm2_block_id& id,
                        bocl_kernel* kern,
                        cl_command_queue& queue)
{
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  int dataSize = 8+9;
  auto* alphas = new float[dataSize]; //8 single cell trees, one 8 leaf + node tree
  boxm2_array_3d<uchar16> trees=blk->trees_copy();
  //set up fake trees
  int count = 0;
  for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j) {
      uchar16 tree = trees(i,j,0);
      boct_bit_tree bit_tree( (unsigned char*) tree.data_block(), 3 );

      //make middle one complex
      if (i==1 && j==1) {
        bit_tree.set_data_ptr(count);
        bit_tree.set_bit_at(0, true);
        for (int cell=0; cell<9; ++cell)
          alphas[count+cell] = 1/32.0;
        count+=9;
      }
      else
      {
        alphas[count] = (1)/4.0;
        //alphas[count] = 8.0f;
        bit_tree.set_bit_at(0, false);
        bit_tree.set_data_ptr(count);
        count++;
      }

      //store tree in blk
      std::memcpy( tree.data_block(), bit_tree.get_bits(), 16 );
      trees(i,j,0) = tree;
    }
  }
  blk->set_trees(trees);
  std::cout<<"Original probs ----------------------"<<std::endl;
  print_probs(blk, alphas, data, dataSize);
  std::cout<<"Original alphas"<<std::endl;
  print_alphas(blk, alphas, data, dataSize);
  std::cout<<"-------------------------------------\n\n";
#ifdef DEBUG
  for (int i=0; i<dataSize; ++i)
    std::cout<<"alpha "<<i<<':'<<alphas[i]<<std::endl;
#endif // DEBUG
  /////////////////////////////////////////////////////////////////////////////
  //Prep opencl buffers and run kernel
  /////////////////////////////////////////////////////////////////////////////
  //get opencl buffers
  bocl_mem* alpha_buffer = new bocl_mem(device->context(), alphas, dataSize*sizeof(float), "old alpha buffer");
  alpha_buffer->create_buffer(CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR);

  auto* new_alpha_buffer = new float[ dataSize ];
  std::fill(new_alpha_buffer, new_alpha_buffer + dataSize, 100.0f);
  bocl_mem* new_alphas = new bocl_mem(device->context(), new_alpha_buffer,  dataSize*sizeof(float), "filtered alpha buffer ");
  new_alphas->create_buffer(CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR);

  //grab the block out of the cache as well
  bocl_mem* blk_mem = opencl_cache->get_block(scene,id);
  bocl_mem* blk_info = opencl_cache->loaded_block_info();
  blk_mem->write_to_buffer(queue);

  //Get helper buffers
  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //center buffers
  bocl_mem_sptr centerX = new bocl_mem(device->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
  bocl_mem_sptr centerY = new bocl_mem(device->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
  bocl_mem_sptr centerZ = new bocl_mem(device->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //set workspace
  std::size_t lThreads[] = {1, 1, 1};
  std::size_t gThreads[] = { RoundUp(data.sub_block_num_.x(), lThreads[0]),
                            RoundUp(data.sub_block_num_.y(), lThreads[1]),
                            RoundUp(data.sub_block_num_.z(), lThreads[2]) };

  //make it a reference so the destructor isn't called at the end...
  kern->set_arg( blk_info );
  kern->set_arg( blk_mem );
  kern->set_arg( alpha_buffer );
  kern->set_arg( new_alphas );
  kern->set_arg( lookup.ptr() );
  kern->set_arg( centerX.ptr() );
  kern->set_arg( centerY.ptr() );
  kern->set_arg( centerZ.ptr() );
  kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*10*sizeof(cl_uchar) ); //cumsum (10 bytes per local thread
  kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //local trees (uchar16 per local thread)
  kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //neighbor trees (uchar16 per local thread)
  kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_float4)*6 );  //local neighbor points 6*float4 per local thread)

  //execute kernel
  kern->execute( queue, 3, lThreads, gThreads);
  clFinish( queue);
  kern->clear_args();

  //check on output
  std::cout<<"New probs::::"<<std::endl;
  new_alphas->read_to_buffer(queue);
  print_probs(blk, new_alpha_buffer, data, dataSize);

  std::cout<<"New alphas:::"<<std::endl;
  print_alphas(blk, new_alpha_buffer, data, dataSize);


  for (int i=0; i<dataSize; ++i)
    std::cout<<"Newalpha: "<<new_alpha_buffer[i]<<std::endl;

  delete[] new_alpha_buffer;
  delete[] alphas;
}

/////////////////////////////////////////////////////////////////////////////
//Inverted test
// set up data array, set up looks like this:
//  ___ ___ ___
// |_|_|_|_|_|_|
// |_|_|_|_|_|_|
// |_|_|   |_|_|
// |_|_|___|_|_|
// |_|_|_|_|_|_|
// |_|_|_|_|_|_|
//
void test_outer_cluster(const boxm2_scene_sptr& scene,
                        boxm2_block* blk,
                        boxm2_block_metadata& data,
                        bocl_device_sptr& device,
                        boxm2_opencl_cache_sptr& opencl_cache,
                        const boxm2_block_id& id,
                        bocl_kernel* kern,
                        cl_command_queue& queue)
{
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;
  int dataSize = 8*(1+8+64) + 1;
  auto* alphas = new float[dataSize]; //8 single cell trees, one 8 leaf + node tree

  //set up fake trees
  int count = 0;
  boxm2_array_3d<uchar16> trees=blk->trees_copy();
  for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j) {
      uchar16 tree = trees(i,j,0);
      boct_bit_tree bit_tree( (unsigned char*) tree.data_block(), 3 );

      //make middle one simple
      if (i==1 && j==1) {
        alphas[count] = .5;
        //alphas[count] = 8.0f;
        bit_tree.set_bit_at(0, false);
        bit_tree.set_data_ptr(count);
        count++;
      }
      else
      {
        bit_tree.set_data_ptr(count);
        bit_tree.set_bit_at(0, true);
        for (int c=0; c<8; ++c) bit_tree.set_bit_at(c+1, true);
        for (int cell=0; cell<(1+8+64); ++cell) {
          alphas[count+cell] = 1.0/16.0;
        }
        count += (1+8+64);
      }
      //store tree in blk
      std::memcpy( tree.data_block(), bit_tree.get_bits(), 16 );
      trees(i,j,0) = tree;
    }
  }
  blk->set_trees(trees);
  std::cout<<"Original probs ----------------------"<<std::endl;
  print_probs(blk, alphas, data, dataSize);
  std::cout<<"Original alphas"<<std::endl;
  print_alphas(blk, alphas, data, dataSize);
  std::cout<<"-------------------------------------\n\n";

  ////////////////////////////////////////////////////////////////////////////
  //Prep opencl buffers and run kernel
  /////////////////////////////////////////////////////////////////////////////
  //get opencl buffers
  bocl_mem_sptr alpha_buffer = new bocl_mem(device->context(), alphas, dataSize*sizeof(float), "old alpha buffer");
  alpha_buffer->create_buffer(CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR);

  auto* new_alpha_buffer = new float[ dataSize ];
  std::fill(new_alpha_buffer, new_alpha_buffer + dataSize, 100.0f);
  bocl_mem_sptr new_alphas = new bocl_mem(device->context(), new_alpha_buffer,  dataSize*sizeof(float), "filtered alpha buffer ");
  new_alphas->create_buffer(CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR);

  //grab the block out of the cache as well
  bocl_mem* blk_mem = opencl_cache->get_block(scene,id);
  bocl_mem* blk_info = opencl_cache->loaded_block_info();
  blk_mem->write_to_buffer(queue);

  //Get helper buffers
  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //center buffers
  bocl_mem_sptr centerX = new bocl_mem(device->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
  bocl_mem_sptr centerY = new bocl_mem(device->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
  bocl_mem_sptr centerZ = new bocl_mem(device->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //set workspace
  std::size_t lThreads[] = {1, 1, 1};
  std::size_t gThreads[] = { RoundUp(data.sub_block_num_.x(), lThreads[0]),
                            RoundUp(data.sub_block_num_.y(), lThreads[1]),
                            RoundUp(data.sub_block_num_.z(), lThreads[2]) };

  //make it a reference so the destructor isn't called at the end...
  kern->set_arg( blk_info );
  kern->set_arg( blk_mem );
  kern->set_arg( alpha_buffer.ptr() );
  kern->set_arg( new_alphas.ptr() );
  kern->set_arg( lookup.ptr() );
  kern->set_arg( centerX.ptr() );
  kern->set_arg( centerY.ptr() );
  kern->set_arg( centerZ.ptr() );
  kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*10*sizeof(cl_uchar) ); //cumsum (10 bytes per local thread
  kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //local trees (uchar16 per local thread)
  kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_uchar16) );  //neighbor trees (uchar16 per local thread)
  kern->set_local_arg( lThreads[0]*lThreads[1]*lThreads[2]*sizeof(cl_float4)*6 );  //local neighbor points 6*float4 per local thread)

  //execute kernel
  kern->execute( queue, 3, lThreads, gThreads);
  clFinish( queue);
  kern->clear_args();

  //check on output
  std::cout<<"New probs::::"<<std::endl;
  new_alphas->read_to_buffer(queue);
  print_probs(blk, new_alpha_buffer, data, dataSize);

  std::cout<<"New alphas:::"<<std::endl;
  print_alphas(blk, new_alpha_buffer, data, dataSize);

  delete[] new_alpha_buffer;
  delete[] alphas;
}

void test_filter_kernel()
{
  //----------------------------------------------------------------------------
  //--- BEGIN BOXM2 API EXAMPLE ------------------------------------------------
  //----------------------------------------------------------------------------
  DECLARE_FUNC_CONS(boxm2_ocl_filter_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_filter_process, "boxm2OclFilterProcess");

  //REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(bocl_device_sptr);

  bocl_manager_child &mgr = bocl_manager_child::instance();
  if (mgr.gpus_.size()==0)
    return;

  bocl_device_sptr device = mgr.gpus_[0];
  auto* kern = new bocl_kernel();
  boxm2_ocl_filter_process_globals::compile_filter_kernel(device,kern);

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  //manufacture scene information
  boxm2_scene_sptr scene = new boxm2_scene();
  scene->set_local_origin( vgl_point_3d<double>(0,0,0) );

  //set scene block information
  std::map<boxm2_block_id, boxm2_block_metadata> blocks;
  boxm2_block_id id(0,0,0);
  boxm2_block_metadata data( id,                           //id
                             vgl_point_3d<double>(0,0,0),  //local_origin
                             vgl_vector_3d<double>(1.0, 1.0, 1.0), //sub_block_dim
                             vgl_vector_3d<unsigned>(3,3,1),  //sub block num
                             1,   //initlevel
                             3,   //max level
                             100,  //max mb
                             0.0); //Pinit starts out at 0 (will create 0 alphas?
  blocks[id] = data;
  scene->set_blocks(blocks);

  // list of appearance models/observation models used by this scene
  std::vector<std::string> appearances;
  appearances.push_back(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  scene->set_appearances(appearances);

  //grab boxm2_scene_info
  /* boxm2_scene_info* info = */ scene->get_blk_metadata(id);

  //instantiate a cache to grab the first block
  boxm2_lru_cache::create(scene);
  boxm2_block* blk = boxm2_cache::instance()->get_block(scene,id);

  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(device);

  //RUN Tests
  std::cout<<"Testing inner cluster"<<std::endl;
  test_inner_cluster(scene,blk,data,device,opencl_cache,id,kern,queue);
  std::cout<<"Testing outer clusters"<<std::endl;
  test_outer_cluster(scene,blk,data,device,opencl_cache,id,kern,queue);
}


TESTMAIN( test_filter_kernel );
