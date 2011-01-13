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
#if 0 // cannot delete a void* !!!
     if ( block_info_->cpu_buffer() ) delete block_info_->cpu_buffer();
#endif
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
