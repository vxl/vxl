#include "boxm2_opencl_cache.h"
//:
// \file


//: constructor
boxm2_opencl_cache::boxm2_opencl_cache(boxm2_cache* cpu_cache,
                                       cl_context* context,
                                       cl_command_queue* queue,
                                       boxm2_scene* scene)
{
  cpu_cache_ = cpu_cache;
  context_ = context;
  queue_ = queue;
  loaded_ = boxm2_block_id(-1, -1, -1);
  cached_block_ = 0;
  block_info_ = 0;
  tree_ptrs_ = 0;
  trees_per_buffer_ = 0;
  mem_ptrs_ = 0; 
  scene_ = scene;
}


//: destructor
boxm2_opencl_cache::~boxm2_opencl_cache()
{
#if 0
  // clean up block
  delete cached_block_;

  // clean up loaded data
  vcl_map<vcl_string, boxm2_data_base* >::iterator iter;
  for (iter=cached_data_.begin(); iter!=cached_data_.end(); ++iter) {
    delete (*iter).second;
  }
#endif // 0
}

//: realization of abstract "get_block(block_id)"
bocl_mem* boxm2_opencl_cache::get_block(boxm2_block_id id)
{
  if (cached_block_ && loaded_ == id)
    return cached_block_;

  //clean up...
  delete cached_block_;
  cached_block_ = 0;

  //otherwise load it from disk with blocking
  loaded_ = id;
  boxm2_block* loaded = cpu_cache_->get_block(id);
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;
  boxm2_array_3d<uchar16>& trees = loaded->trees();
  cached_block_ = new bocl_mem(*context_, trees.data_block(), trees.size()*sizeof(uchar16), "3d trees buffer");
  cached_block_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  //store the metadata
  if (block_info_) {
     boxm2_scene_info* buff = (boxm2_scene_info*) block_info_->cpu_buffer();
     if ( buff ) delete buff;
     delete block_info_;
  }
  boxm2_scene_info* info_buffer = scene_->get_blk_metadata(loaded_);
  info_buffer->num_buffer = loaded->num_buffers();
  info_buffer->tree_buffer_length = loaded->tree_buff_length();
  info_buffer->data_buffer_length = 65536;
  block_info_ = new bocl_mem(*context_, info_buffer, sizeof(boxm2_scene_info), "scene info buffer");
  block_info_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  return cached_block_;
}

//: get tree_ptrs returns the tree pointers for the block currently in the cache
bocl_mem* boxm2_opencl_cache::get_loaded_tree_ptrs()
{  
  if( tree_ptrs_ ) delete tree_ptrs_; 
  tree_ptrs_ = 0;
 
  boxm2_block* loaded = cpu_cache_->get_block(loaded_); 
  boxm2_array_2d<int>& tree_ptrs = loaded->tree_ptrs(); 
  tree_ptrs_ = new bocl_mem(*context_, tree_ptrs.begin(), tree_ptrs.size()*sizeof(int), "2d tree_ptrs buffer");
  tree_ptrs_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  return tree_ptrs_; 
}

bocl_mem* boxm2_opencl_cache::get_loaded_trees_per_buffer()
{
  if( trees_per_buffer_ ) delete trees_per_buffer_; 
  trees_per_buffer_ = 0;
 
  boxm2_block* loaded = cpu_cache_->get_block(loaded_); 
  boxm2_array_1d<ushort>& trees_per_buffer = loaded->trees_in_buffers(); 
  trees_per_buffer_ = new bocl_mem(*context_, trees_per_buffer.begin(), trees_per_buffer.size()*sizeof(int), "1d trees_per_buffer buffer");
  trees_per_buffer_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  return trees_per_buffer_; 
}
bocl_mem* boxm2_opencl_cache::get_loaded_mem_ptrs()
{
  if( mem_ptrs_ ) delete mem_ptrs_; 
  mem_ptrs_ = 0;
  boxm2_block* loaded = cpu_cache_->get_block(loaded_); 
  
  typedef vnl_vector_fixed<unsigned short, 2> ushort2; 
  boxm2_array_1d<ushort2>& mem_ptrs = loaded->mem_ptrs(); 
  mem_ptrs_ = new bocl_mem(*context_, mem_ptrs.begin(), mem_ptrs.size()*sizeof(int), "1d mem_ptrs buffer");
  mem_ptrs_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  return mem_ptrs_; 
}

