#include "boxm2_opencl_cache.h"
//:
// \file


//: scnee/device constructor
boxm2_opencl_cache::boxm2_opencl_cache(boxm2_scene_sptr scene, 
                                       bocl_device_sptr device) 
{
  // by default try to create an LRU cache
  boxm2_lru_cache::create(scene); 
  cpu_cache_ = boxm2_cache::instance(); 
  
  context_ = &device->context(); 
  
  //create command queue... make sure this isn't destructed at the end of the function...
  int status = 0;
  cl_command_queue queue = clCreateCommandQueue(*context_,
                                                *device->device_id(),
                                                CL_QUEUE_PROFILING_ENABLE,
                                                &status);
  if (!check_val(status,CL_SUCCESS,"boxm2_opencl_cache:: failed in command queue creation" + error_to_string(status)))
      return;
  queue_ = &queue;   
  
  loaded_ = boxm2_block_id(-1,-1,-1); 
  cached_block_ = 0;
  block_info_ = 0;
  tree_ptrs_ = 0;
  trees_per_buffer_ = 0;
  mem_ptrs_ = 0;
  scene_ = scene;
}

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

bool boxm2_opencl_cache::clear_cache()
{
  delete cached_block_;     cached_block_=0;
  delete block_info_;       block_info_=0;
  delete tree_ptrs_;        tree_ptrs_=0;
  delete trees_per_buffer_; trees_per_buffer_=0;
  delete mem_ptrs_;         mem_ptrs_=0;

  // clean up loaded data
  vcl_map<vcl_string, bocl_mem*>::iterator iter;
  for (iter=cached_data_.begin(); iter!=cached_data_.end(); ++iter) {
    bocl_mem* dat = iter->second;
    delete dat;
  }
  cached_data_.clear();
  loaded_data_.clear();

  return true;
}

//: realization of abstract "get_block(block_id)"
bocl_mem* boxm2_opencl_cache::get_block(boxm2_block_id id)
{
  if (cached_block_ && loaded_ == id)
    return cached_block_;

  //clean up...
  delete cached_block_; cached_block_ = 0;
  //this->clear_cache();

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
     delete buff;
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
#if 0
  delete tree_ptrs_;
  tree_ptrs_ = 0;
#endif

  boxm2_block* loaded = cpu_cache_->get_block(loaded_);
  boxm2_array_2d<int>& tree_ptrs = loaded->tree_ptrs();
  tree_ptrs_ = new bocl_mem(*context_, tree_ptrs.begin(), tree_ptrs.size()*sizeof(int), "2d tree_ptrs buffer");
  tree_ptrs_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  return tree_ptrs_;
}

bocl_mem* boxm2_opencl_cache::get_loaded_trees_per_buffer()
{
#if 0
  delete trees_per_buffer_;
  trees_per_buffer_ = 0;
#endif

  boxm2_block* loaded = cpu_cache_->get_block(loaded_);
  boxm2_array_1d<unsigned short>& trees_per_buffer = loaded->trees_in_buffers();
  trees_per_buffer_ = new bocl_mem(*context_, trees_per_buffer.begin(), trees_per_buffer.size()*sizeof(int), "1d trees_per_buffer buffer");
  trees_per_buffer_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  return trees_per_buffer_;
}

bocl_mem* boxm2_opencl_cache::get_loaded_mem_ptrs()
{
#if 0
  delete mem_ptrs_;
  mem_ptrs_ = 0;
#endif
  boxm2_block* loaded = cpu_cache_->get_block(loaded_);

  typedef vnl_vector_fixed<unsigned short, 2> ushort2;
  boxm2_array_1d<ushort2>& mem_ptrs = loaded->mem_ptrs();
  mem_ptrs_ = new bocl_mem(*context_, mem_ptrs.begin(), mem_ptrs.size()*sizeof(int), "1d mem_ptrs buffer");
  mem_ptrs_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  return mem_ptrs_;
}


//: Get data generic
// Possible issue: if num_bytes is greater than 0, should it then always initialize a new data object?
bocl_mem* boxm2_opencl_cache::get_data(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes)
{
  //make sure that the data is in the
  if (loaded_data_[type] == id)
    return cached_data_[type];

  //otherwise get the data block from cpu cache
  //this statment is: does cached_data_ contain(type)?
  if ( cached_data_.find(type) != cached_data_.end())
  {
#ifdef DEBUG
    vcl_cout<<"ocl_cache release memory for :"<<type<<vcl_endl;
#endif
    //release existing memory
    bocl_mem* toDelete = cached_data_[type];
    delete toDelete;
    cached_data_[type] = 0;
  }

  //create new memory
  boxm2_data_base* data_base = cpu_cache_->get_data_base(id,type,num_bytes);
  if (num_bytes > 0 && data_base->buffer_length() != num_bytes )
  {
    vcl_cout<<"GRABBING "<<type<<" that doesn't match input size of "<<num_bytes<<vcl_endl;
  }
  loaded_data_[type] = id;
  bocl_mem* data = new bocl_mem(*context_, data_base->data_buffer(), data_base->buffer_length(), type);
  data->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  cached_data_[type] = data;

  return data;
}

//: Does a soft delete of data - removes it from the cache but it stays allocated
//  This will have to NOT decrement the total bytes allocated counter or just
//  simply move the data to have a different type
void boxm2_opencl_cache::remove_data(boxm2_block_id id, vcl_string type)
{
  //make sure that the data is in the
  if (loaded_data_[type] == id) {
    //loaded_data_[type] = 0;
    //cached_data_[type] = 0;
    vcl_map<vcl_string, boxm2_block_id>::iterator liter = loaded_data_.find(type);
    loaded_data_.erase(liter);
    vcl_map<vcl_string, bocl_mem* >::iterator citer = cached_data_.find(type);
    cached_data_.erase(citer);
  }
}

//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_opencl_cache const& scene){}
void vsl_b_write(vsl_b_ostream& os, const boxm2_opencl_cache* &p){}
void vsl_b_write(vsl_b_ostream& os, boxm2_opencl_cache_sptr& sptr){} 
void vsl_b_write(vsl_b_ostream& os, boxm2_opencl_cache_sptr const& sptr){}

//: Binary load boxm2_cache  from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache &scene){}
void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache* p){}
void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache_sptr& sptr){}
void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache_sptr const& sptr){}