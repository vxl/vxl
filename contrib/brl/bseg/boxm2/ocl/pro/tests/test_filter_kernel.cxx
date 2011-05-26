//:
// \file
// \author Andrew Miller
// \date May 24, 2011
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_where_root_dir.h>

#include <boct/boct_bit_tree2.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_buffer_mgr.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/pro/processes/boxm2_ocl_filter_process.cxx>

#include <boxm2/ocl/pro/boxm2_ocl_processes.h>

#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/io/boxm2_lru_cache.h>

#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <vnl/vnl_vector_fixed.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <brdb/brdb_value.h>


void print_probs(boxm2_block* blk, float* alphas, boxm2_block_metadata data, int dataSize)
{
  //print in 3x3
  typedef vnl_vector_fixed<unsigned char, 16> uchar16; 
  vcl_cout.precision(4); 
  vcl_cout.setf(std::ios::fixed, std::ios::floatfield);   // floatfield set to fixed
  for(int j=0; j<3; ++j) {
    vcl_cout<<"|"; 

    for(int i=0; i<3; ++i) {
      
      float prob=0.0f; 
      uchar16 tree = blk->trees()(i,j,0); 
      boct_bit_tree2 bit_tree( (unsigned char*) tree.data_block(), 3 ); 
      int idx = bit_tree.get_data_ptr(); 
      
      
      if( !(i==1 && j==1) || dataSize==9) {
        prob = 1.0 - vcl_exp(-alphas[idx] * data.sub_block_dim_.x()); 
      }
      else {
        float totalAlpha = 0.0f; 
        for(int ai=idx+1; ai<idx+1+8; ++ai) {
          totalAlpha += alphas[ai]; 
        }
        prob = 1.0-vcl_exp(-totalAlpha * data.sub_block_dim_.x()/2.0); 
        
      }
      vcl_cout<<"("<<i<<','<<j<<") = "<<prob<<"|";
    }
    vcl_cout<<vcl_endl;
  }  
}

void print_alphas(boxm2_block* blk, float* alphas, boxm2_block_metadata data, int dataSize)
{
  //print in 3x3
  typedef vnl_vector_fixed<unsigned char, 16> uchar16; 
  vcl_cout.precision(4); 
  vcl_cout.setf(std::ios::fixed, std::ios::floatfield);   // floatfield set to fixed
  for(int j=0; j<3; ++j) {
    vcl_cout<<"|"; 
    for(int i=0; i<3; ++i) {
      
      float prob=0.0f; 
      uchar16 tree = blk->trees()(i,j,0); 
      boct_bit_tree2 bit_tree( (unsigned char*) tree.data_block(), 3 ); 
      int idx = bit_tree.get_data_ptr(); 
      if( !(i==1 && j==1) || dataSize==9 ) {
        prob = alphas[idx];  
      }
      else {
        float totalAlpha = 0.0f; 
        for(int ai=idx+1; ai<idx+1+8; ++ai) {
          totalAlpha += alphas[ai]; 
        }
        prob = totalAlpha; 
        
      }
      vcl_cout<<"("<<i<<','<<j<<") = "<<prob<<"|";
    }
    vcl_cout<<vcl_endl;
  }  
}

void test_filter_kernel()
{
  //----------------------------------------------------------------------------
  //--- BEGIN BOXM2 API EXAMPLE ------------------------------------------------
  //----------------------------------------------------------------------------
  typedef vnl_vector_fixed<unsigned char, 16> uchar16; 
  DECLARE_FUNC_CONS(boxm2_ocl_filter_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_filter_process, "boxm2OclFilterProcess");

  //REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(bocl_device_sptr);
  
  bocl_manager_child_sptr mgr = bocl_manager_child::instance();
  if (mgr->gpus_.size()==0) 
    return;
  
  bocl_device_sptr device = mgr->gpus_[0];
  bocl_kernel* kern = new bocl_kernel();
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
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks;
  boxm2_block_id id(0,0,0);
  boxm2_block_metadata data( id,                           //id
                             vgl_point_3d<double>(0,0,0),  //local_origin
                             vgl_vector_3d<double>(1.0/16.0, 1.0/16.0, 1.0/16.0), //sub_block_dim
                             vgl_vector_3d<unsigned>(3,3,1),  //sub block num
                             1,   //initlevel 
                             3,   //max level
                             100,  //max mb
                             0.0); //Pinit starts out at 0 (will create 0 alphas?
  blocks[id] = data;
  scene->set_blocks(blocks);
 
  //: list of appearance models/observation models used by this scene
  vcl_vector<vcl_string> appearances;
  appearances.push_back(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  scene->set_appearances(appearances);

  //grab boxm2_scene_info
  boxm2_scene_info* info = scene->get_blk_metadata(id);

  //instantiate a cache to grab the first block
  boxm2_lru_cache::create(scene);
  boxm2_block* blk = boxm2_cache::instance()->get_block(id);

  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(scene, device);
  int numTrees = data.sub_block_num_.x() * data.sub_block_num_.y() * data.sub_block_num_.z();

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
  
  int dataSize = 8+9; 
  float* alphas = new float[dataSize]; //8 single cell trees, one 8 leaf + node tree
  
  //set up fake trees
  int count = 0; 
  for(int i=0; i<3; ++i) {
    for(int j=0; j<3; ++j) {
      uchar16 tree = blk->trees()(i,j,0); 
      boct_bit_tree2 bit_tree( (unsigned char*) tree.data_block(), 3 ); 
      
      //make middle one complex
      if(i==1 && j==1 && dataSize==8+9) {
        bit_tree.set_data_ptr(count);
        bit_tree.set_bit_at(0, true); 
        
        for(int cell=0; cell<9; ++cell)
          alphas[count+cell] = 1.5f; 
        
        count+=9;
      }
      else 
      {
        alphas[count] = (count+1)/2.0; 
        //alphas[count] = 8.0f;
        bit_tree.set_data_ptr(count); 
        count++;
      } 
      
      //store tree in blk
      vcl_memcpy( tree.data_block(), bit_tree.get_bits(), 16 ); 
      blk->trees()(i,j,0) = tree;  
    }
  }
  
  print_probs(blk, alphas, data, dataSize); 

  /////////////////////////////////////////////////////////////////////////////
  //Prep opencl buffers and run kernel
  /////////////////////////////////////////////////////////////////////////////
  //get opencl buffers
  bocl_mem* alpha_buffer = new bocl_mem(device->context(), alphas, dataSize*sizeof(float), "old alpha buffer"); 
  alpha_buffer->create_buffer(CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR);

  float* new_alpha_buffer = new float[ dataSize ]; 
  vcl_fill(new_alpha_buffer, new_alpha_buffer + dataSize, 100.0f); 
  bocl_mem* new_alphas = new bocl_mem(device->context(), new_alpha_buffer,  dataSize*sizeof(float), "filtered alpha buffer ");
  new_alphas->create_buffer(CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR);

  //grab the block out of the cache as well
  bocl_mem* blk_mem = opencl_cache->get_block(id);
  bocl_mem* blk_info = opencl_cache->loaded_block_info();

  //Get helper buffers
  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    
  //center buffers
  bocl_mem_sptr centerX = new bocl_mem(device->context(), boct_bit_tree2::centerX, sizeof(cl_float)*585, "centersX lookup buffer"); 
  bocl_mem_sptr centerY = new bocl_mem(device->context(), boct_bit_tree2::centerY, sizeof(cl_float)*585, "centersY lookup buffer"); 
  bocl_mem_sptr centerZ = new bocl_mem(device->context(), boct_bit_tree2::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer"); 
  centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR); 
  centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);     
  centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR); 
  
  //set workspace
  vcl_size_t lThreads[] = {1, 1, 1};
  vcl_size_t gThreads[] = { RoundUp(data.sub_block_num_.x(), lThreads[0]), 
                            RoundUp(data.sub_block_num_.y(), lThreads[1]),
                            RoundUp(data.sub_block_num_.z(), lThreads[2]) };
                            
                            

  for(int i=0; i<2; ++i) {
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
  vcl_cout<<"New probs::::"<<vcl_endl;
  new_alphas->read_to_buffer(queue); 
  print_probs(blk, new_alpha_buffer, data, dataSize); 
  
  vcl_cout<<"New alphas:::"<<vcl_endl;
  print_alphas(blk, new_alpha_buffer, data, dataSize); 
  
  }
}


TESTMAIN( test_filter_kernel );
