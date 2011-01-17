#include "boxm2_opencl_refine_process.h"

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_save.h>
#include <boxm2/boxm2_util.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>

//TODO IN THIS INIT METHOD: Need to pass in a ref to the OPENCL_CACHE so this
//class can easily access BOCL_MEMs
bool boxm2_opencl_refine_process::init_kernel(cl_context* context,
                                              cl_device_id* device,
                                              vcl_string opts)
{
  context_ = context;

  //gather all render sources... seems like a lot for rendering...
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "bit/refine_bit_scene.cl");
  
  //have kernel construct itself using the context and device
  bool created =  refine_kernel_.create_kernel( context_,
                                                device,
                                                src_paths,
                                                "refine_bit_scene",   //kernel name
                                                "",                   //options
                                                "boxm2 opencl refine"); //kernel identifier (for error checking)
  return created;
}

//OPENCL_REFINE EXECUTE
bool boxm2_opencl_refine_process::execute(vcl_vector<brdb_value_sptr>& input, vcl_vector<brdb_value_sptr>& output)
{
  transfer_time_ = 0.0f; gpu_time_ = 0.0f; total_time_ = 0.0f;
  vul_timer total; 
  int i = 0;

  //scene argument
  brdb_value_t<boxm2_scene_sptr>* scene_brdb = static_cast<brdb_value_t<boxm2_scene_sptr>* >( input[i++].ptr() );
  boxm2_scene_sptr scene = scene_brdb->value(); 

  //prob_thresh buffer
  float* prob_buff = new float[1];
  prob_buff[0] = .3f;
  bocl_mem prob_thresh((*context_), prob_buff, sizeof(float), "prob_thresh buffer");
  prob_thresh.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //output buffer
  float* output_arr = new float[100];
  for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
  bocl_mem cl_output((*context_), output_arr, sizeof(float)*100, "output buffer");
  cl_output.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bit lookup buffer
  cl_uchar* lookup_arr = new cl_uchar[256];
  boxm2_util::set_bit_lookup(lookup_arr);
  bocl_mem lookup((*context_), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
   
  //For each ID in the visibility order, grab that block
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks(); 
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk; 
  for(blk = blocks.begin(); blk != blocks.end(); ++blk) 
  {
    //get id
    boxm2_block_id id = blk->first; 
    
    //write the image values to the buffer
    vul_timer transfer; 
    bocl_mem* blk       = cache_->get_block(id);
    bocl_mem* blk_ptrs  = cache_->get_loaded_tree_ptrs(); 
    bocl_mem* mem_ptrs  = cache_->get_loaded_mem_ptrs(); 
    bocl_mem* blks_per  = cache_->get_loaded_trees_per_buffer(); 
  
    //data
    bocl_mem* alpha     = cache_->get_data<BOXM2_ALPHA>(id);
    bocl_mem* mog       = cache_->get_data<BOXM2_MOG3_GREY>(id);
    bocl_mem* num_obs   = cache_->get_data<BOXM2_NUM_OBS>(id);
    bocl_mem* blk_info  = cache_->loaded_block_info(); 
    transfer_time_ += (float) transfer.all(); 
 
    //set kernel args
    refine_kernel_.set_arg( blk_info );
    refine_kernel_.set_arg( blk );
    refine_kernel_.set_arg( blk_ptrs );
    refine_kernel_.set_arg( mem_ptrs );
    refine_kernel_.set_arg( blks_per );
    refine_kernel_.set_arg( alpha );
    refine_kernel_.set_arg( mog );
    refine_kernel_.set_arg( num_obs );
    refine_kernel_.set_arg( &lookup );
    refine_kernel_.set_arg( &prob_thresh ); 
    refine_kernel_.set_arg( &cl_output );
    
    refine_kernel_.set_local_arg( 16*sizeof(cl_uchar) );
    refine_kernel_.set_local_arg( sizeof(cl_uchar16) );
    refine_kernel_.set_local_arg( sizeof(cl_uchar16) );
        
    //set workspace
    boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
    vcl_size_t lThreads[] = {1, 1};
    vcl_size_t gThreads[] = {info_buffer->num_buffer, 1};
    
    //execute kernel
    refine_kernel_.execute( (*command_queue_), lThreads, gThreads);
    clFinish(*command_queue_); 
    gpu_time_ += refine_kernel_.exec_time(); 
    
    //clear render kernel args so it can reset em on next execution
    refine_kernel_.clear_args();
    
    //read blocks to CPU
    blk->read_to_buffer(*command_queue_);
    blk_ptrs->read_to_buffer(*command_queue_);
    mem_ptrs->read_to_buffer(*command_queue_);
    alpha->read_to_buffer(*command_queue_);
    mog->read_to_buffer(*command_queue_);
    num_obs->read_to_buffer(*command_queue_);
  }
  
  //clean up camera, lookup_arr, img_dim_buff
  delete[] output_arr;
  delete[] lookup_arr;
  delete[] prob_buff;
  
  //record total time
  total_time_ = (float) total.all(); 
  return true;
}


bool boxm2_opencl_refine_process::clean()
{
  return true;
}

