#include "boxm2_opencl_refine_process.h"
//:
// \file

#include <vil/vil_save.h>

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>

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
  bool created =  refine_kernel_.create_kernel( context_, device, src_paths, "refine_bit_scene", opts, "boxm2 opencl refine"); //kernel identifier (for error checking)
  created = created&&refine_trees_.create_kernel( context_, device, src_paths, "refine_trees", opts, "boxm2 opencl refine trees (pass one)"); //kernel identifier (for error checking)
  //created = created&& refine_scan_.create_kernel( context_, device, src_paths, "refine_scan", opts, "boxm2 opencl refine scan (pass two)"); //kernel identifier (for error checking)

  //set refine datas
  bocl_kernel* rd4 = new bocl_kernel();
  bocl_kernel* rd8 = new bocl_kernel();
  bocl_kernel* rd16= new bocl_kernel();
  rd4->create_kernel( context_, device, src_paths, "refine_data", "-D MOG_TYPE_4 ", "boxm2 opencl refine data size 4 (pass three)");
  rd8->create_kernel( context_, device, src_paths, "refine_data", "-D MOG_TYPE_8 ", "boxm2 opencl refine data size 8 (pass three)");
  rd16->create_kernel( context_, device, src_paths, "refine_data", "-D MOG_TYPE_16 ", "boxm2 opencl refine data size 16 (pass three)");
  refine_datas_[4] = rd4;
  refine_datas_[8] = rd8;
  refine_datas_[16] = rd16;

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
  brdb_value_t<vcl_string>* brdb_data_type = static_cast<brdb_value_t<vcl_string>* >( input[i++].ptr() );
  data_type_=brdb_data_type->value();

  //prob_thresh buffer
  float* prob_buff = new float[1]; prob_buff[0] = .3f;
  prob_thresh_ = new bocl_mem((*context_), prob_buff, sizeof(float), "prob_thresh buffer");
  prob_thresh_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //output buffer
  float* output_arr = new float[100];
  for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
  cl_output_ = new bocl_mem((*context_), output_arr, sizeof(float)*100, "output buffer");
  cl_output_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bit lookup buffer
  cl_uchar* lookup_arr = new cl_uchar[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  lookup_ = new bocl_mem ((*context_), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //For each ID in the visibility order, grab that block
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    //----- IF THE BLOCK IS NOT RANDOMLY DISTRIBUTED, USE NEW METHOD -----------
    // New Method Summary:
    //  - NEED TO CLEAR OUT THE GPU CACHE BEFORE YOU START.. so you don't overwrite stuff accidentally...
    //  - Create Block Copy, refine trees into that copy, maintaining old copy and array of new tree sizes
    //  - Do scan on size vector (cum sum)
    //  - Swap data into new buffers: For each data type
    //    - get BOCL_MEM* data independent of cpu pointer (from cache)
    //    - remove the BOCL_MEM* from the gpu cache (don't delete it)
    //    - do a deep delete (delete CPU buffer from CPU cache)
    //    - get a new data pointer (with newSize), will create CPU buffer and GPU buffer
    //    - Run refine_data_kernel with the two buffers
    //    - delete the old BOCL_MEM*, and that's it...
    
    //clear cache
    cache_->clear_cache(); 
    boxm2_block_metadata data = blk_iter->second;
    if (!data.random_)
    {
      vcl_cout<<"Doing non random refine:"<<vcl_endl;

      ////////////////////////////////////////////////////////////////////////////
      // Step One... currently mimics C++ implementation
      //get id and refine block into tree copy, and calc vector of new tree sizes
      boxm2_block_id id = blk_iter->first;
      int numTrees = data.sub_block_num_.x() * data.sub_block_num_.y() * data.sub_block_num_.z();

      //set up tree copy
      vcl_cout<<"creating tree copy"<<vcl_endl;
      bocl_mem* blk_copy = new bocl_mem((*context_), NULL, numTrees*sizeof(cl_uchar16), "refine trees block copy buffer");
      blk_copy->create_buffer(CL_MEM_READ_WRITE| CL_MEM_ALLOC_HOST_PTR, (*command_queue_));

      //set up tree size (first find num trees)
      vcl_cout<<"creating tree sizes buff"<<vcl_endl;
      bocl_mem* tree_sizes = new bocl_mem((*context_), NULL, sizeof(cl_int)*numTrees, "refine tree sizes buffer");
      tree_sizes->create_buffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, (*command_queue_));
      clFinish((*command_queue_));

      //run refine block into copy
      this->refine_block_copy(id, numTrees, blk_copy, tree_sizes);

      /////////////////////////////////////////////////////////////////////////
      //STEP TWO
      //read out tree_sizes and do cumulative sum on it
      vul_timer scan_time; 
      tree_sizes->read_to_buffer((*command_queue_));
      int* sizebuff = (int*) tree_sizes->cpu_buffer();
      for (int i=1; i<numTrees; ++i)
        sizebuff[i] += sizebuff[i-1];
      int newDataSize = sizebuff[numTrees-1];
      for (int i=numTrees-1; i>0; --i)
        sizebuff[i] = sizebuff[i-1];
      sizebuff[0] = 0;
      tree_sizes->write_to_buffer((*command_queue_));
      vcl_cout<<"New data size: "<<newDataSize<<vcl_endl;
      vcl_cout<<"Scan data sizes time: "<<scan_time.all()<<vcl_endl;
      /////////////////////////////////////////////////////////////////////////


      /////////////////////////////////////////////////////////////////////////
      //STEP Three
      //  - Swap data into new buffers: For each data type
      //    - get BOCL_MEM* data independent of cpu pointer (from cache)
      //    - remove the BOCL_MEM* from the gpu cache (don't delete it)
      //    - do a deep delete (delete CPU buffer from CPU cache)
      //    - get a new data pointer (with newSize), will create CPU buffer and GPU buffer
      //    - Run refine_data_kernel with the two buffers
      //    - delete the old BOCL_MEM*, and that's it...
      // POSSIBLE PROBLEMS: data may not exist in cache and may need to be initialized...
      //this vector will be passed in (listing data types to refine)
      vcl_vector<vcl_string> data_types;
      data_types.push_back(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
      data_types.push_back(boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
      for (int i=0; i<data_types.size(); ++i)
      {
        vcl_cout<<"Swapping data of type: "<<data_types[i]<<vcl_endl;
        this->swap_data(id,data_types[i],newDataSize,numTrees,blk_copy,tree_sizes);
      }
      //ALWAYS GOING TO REFINE ALPHA
      vcl_cout<<"Swapping alpha data... last step fingers cross..."<<vcl_endl;
      this->swap_data(id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),newDataSize,numTrees,blk_copy,tree_sizes);
      
      //clean aux memory
      delete blk_copy; 
      delete tree_sizes; 
      
    }
    else
    {
      //----- OTHERWISE USE OLD METHOD (ranodmly distributed blocks into buffers)-----------
      vcl_cout<<"Refining using random algo"<<vcl_endl;
      //get id
      boxm2_block_id id = blk_iter->first;

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = cache_->get_block(id);
      bocl_mem* blk_ptrs  = cache_->get_loaded_tree_ptrs();
      bocl_mem* mem_ptrs  = cache_->get_loaded_mem_ptrs();
      bocl_mem* blks_per  = cache_->get_loaded_trees_per_buffer();

      //data
      bocl_mem* alpha     = cache_->get_data<BOXM2_ALPHA>(id);
      bocl_mem* mog;
      if (data_type_=="16bit") {
         vcl_cout<<"gett mog 8bit"<<vcl_endl;
         mog = cache_->get_data<BOXM2_MOG3_GREY_16>(id);
      }
      else {
         vcl_cout<<"gett mog 8bit"<<vcl_endl;
         mog = cache_->get_data<BOXM2_MOG3_GREY>(id);
      }
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
      refine_kernel_.set_arg( lookup_ );
      refine_kernel_.set_arg( prob_thresh_ );
      refine_kernel_.set_arg( cl_output_ );

      refine_kernel_.set_local_arg( 16*sizeof(cl_uchar) );
      refine_kernel_.set_local_arg( sizeof(cl_uchar16) );
      refine_kernel_.set_local_arg( sizeof(cl_uchar16) );

      //set workspace
      boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      vcl_size_t lThreads[] = {1, 1};
      vcl_size_t gThreads[] = {info_buffer->num_buffer, 1};

      //execute kernel
      refine_kernel_.execute( (*command_queue_), 2, lThreads, gThreads);
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
  }

  //clean up camera, lookup_arr, img_dim_buff
  delete[] output_arr;
  delete[] lookup_arr;
  delete[] prob_buff;
  delete lookup_;
  delete prob_thresh_;
  delete cl_output_;

  //record total time
  total_time_ = (float) total.all();
  return true;
}


//: Runs last kernel, swaps copy
void boxm2_opencl_refine_process::swap_data(boxm2_block_id id,
                                            vcl_string type,
                                            int newDataSize,
                                            int numTrees,
                                            bocl_mem* blk_copy,
                                            bocl_mem* tree_sizes)
{
  //1. check to make sure a kernel exists to swap datatype of this size
  if (refine_datas_.find(boxm2_data_info::datasize(type)) == refine_datas_.end())
  {
    vcl_cout<<"boxm2_opencl_refine::Kernel for swapping datatype: "
            <<type<<" and size "<<boxm2_data_info::datasize(type)
            <<" is not compiled."<<vcl_endl;
    return;
  }

  //get bocl_mem data independent of CPU pointer
  bocl_mem* dat = cache_->get_data(id, type);

  //get a new data pointer (with newSize), will create CPU buffer and GPU buffer
  vcl_cout<<type<<" new data size is: "<<newDataSize<<vcl_endl;
  int dataBytes = boxm2_data_info::datasize(type) * newDataSize;
  bocl_mem* new_dat = new bocl_mem((*context_), NULL, dataBytes, "new data buffer " + type);
  new_dat->create_buffer(CL_MEM_READ_WRITE, (*command_queue_));  

  //grab the block out of the cache as well
  bocl_mem* blk = cache_->get_block(id);
  bocl_mem* blk_info = cache_->loaded_block_info();

  //prob_thresh buffer
  bool* is_alpha_buffer = new bool[1];
  if (type == boxm2_data_traits<BOXM2_ALPHA>::prefix())
    (*is_alpha_buffer) = true;
  else
    (*is_alpha_buffer) = false; 
  bocl_mem is_alpha((*context_), is_alpha_buffer, sizeof(cl_bool), "is_alpha buffer");
  is_alpha.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //make it a reference so the destructor isn't called at the end...
  bocl_kernel* kern = refine_datas_[boxm2_data_info::datasize(type)];
  kern->set_arg( blk_info );
  kern->set_arg( blk );
  kern->set_arg( blk_copy );
  kern->set_arg( tree_sizes );
  kern->set_arg( dat );
  kern->set_arg( new_dat );
  kern->set_arg( prob_thresh_ );
  kern->set_arg( &is_alpha ); 
  kern->set_arg( lookup_ );
  kern->set_arg( cl_output_ );
  kern->set_local_arg( 16*sizeof(cl_uchar) );
  kern->set_local_arg( sizeof(cl_uchar16) );
  kern->set_local_arg( sizeof(cl_uchar16) );
  
  //set workspace
  vcl_size_t lThreads[] = {1, 1};
  vcl_size_t gThreads[] = {numTrees, 1};

  //execute kernel
  kern->execute( (*command_queue_), 2, lThreads, gThreads);
  clFinish( (*command_queue_));
  kern->clear_args();

  ////write the data to buffer
  cache_->deep_replace_data(id, type, new_dat); 
  if(type == boxm2_data_traits<BOXM2_ALPHA>::prefix()) {
    vcl_cout<<"Writing refined trees."<<vcl_endl;
    blk->read_to_buffer(*command_queue_);
  }
  
  //prin tout alphas for debugging...
  
  
  //clean up DAT
  delete[] is_alpha_buffer;
}

//: Refines the block copy and returns blk_copy and tree_sizes bocl_mem pointers
void boxm2_opencl_refine_process::refine_block_copy(boxm2_block_id id,
                                                    int numTrees,
                                                    bocl_mem* blk_copy,
                                                    bocl_mem* tree_sizes)
{
  //write the image values to the buffer
  vul_timer transfer;
  bocl_mem* blk       = cache_->get_block(id);
  bocl_mem* alpha     = cache_->get_data<BOXM2_ALPHA>(id);
  bocl_mem* blk_info  = cache_->loaded_block_info();
  transfer_time_ += (float) transfer.all();

  //set first kernel args
  refine_trees_.set_arg( blk_info );
  refine_trees_.set_arg( blk );
  refine_trees_.set_arg( blk_copy );
  refine_trees_.set_arg( alpha );
  refine_trees_.set_arg( tree_sizes );
  refine_trees_.set_arg( prob_thresh_ );
  refine_trees_.set_arg( lookup_ );
  refine_trees_.set_arg( cl_output_ );
  refine_trees_.set_local_arg( 16*sizeof(cl_uchar) );
  refine_trees_.set_local_arg( sizeof(cl_uchar16) );
  refine_trees_.set_local_arg( sizeof(cl_uchar16) );

  //set workspace
  vcl_size_t lThreads[] = {1, 1};
  vcl_size_t gThreads[] = {numTrees, 1};

  //execute kernel
  refine_trees_.execute( (*command_queue_), 2, lThreads, gThreads);
  clFinish(*command_queue_);
  gpu_time_ += refine_trees_.exec_time();

  //clear render kernel args so it can reset em on next execution
  refine_trees_.clear_args();
}


bool boxm2_opencl_refine_process::clean()
{
  return true;
}

