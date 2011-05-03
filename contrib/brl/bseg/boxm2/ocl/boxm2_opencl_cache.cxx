#include "boxm2_opencl_cache.h"
#include "vnl/vnl_random.h"
//:
// \file


//: scene/device constructor
boxm2_opencl_cache::boxm2_opencl_cache(boxm2_scene_sptr scene,
                                       bocl_device_sptr device,
                                       int maxBlocks)
{
  maxBlocksInCache = maxBlocks;

  // by default try to create an LRU cache
  boxm2_lru_cache::create(scene);
  cpu_cache_ = boxm2_cache::instance();

  device_ = device;
  context_ = &device->context();

  //create command queue... make sure this isn't destructed at the end of the function...
  int status = 0;
  q_ = clCreateCommandQueue(*context_,
                            *device->device_id(),
                            CL_QUEUE_PROFILING_ENABLE,
                            &status);
  if (!check_val(status,CL_SUCCESS,"boxm2_opencl_cache:: failed in command queue creation" + error_to_string(status)))
    return;

  queue_ = &q_;
  block_info_ = 0;
  scene_ = scene;
}

//: sets a context in case the default context off the device is old
// (this  is used for the tableau's shared cl_gl context)
void boxm2_opencl_cache::set_context(cl_context& context)
{
  context_ = &context;
  int status = clReleaseCommandQueue(q_);
  if (!check_val(status,CL_SUCCESS,"boxm2_opencl_cache::set_context failed to release old queue" + error_to_string(status)))
    return;

  q_ = clCreateCommandQueue(*context_,
                            *device_->device_id(),
                            CL_QUEUE_PROFILING_ENABLE,
                            &status);
  if (!check_val(status,CL_SUCCESS,"boxm2_opencl_cache::set_context failed in command queue creation" + error_to_string(status)))
    return;
  queue_ = &q_;
}

bool boxm2_opencl_cache::clear_cache()
{
  //delete stored block info
  if (block_info_) {
    boxm2_scene_info* buff = (boxm2_scene_info*) block_info_->cpu_buffer();
    delete buff;
    delete block_info_;
    block_info_=0;
  }

  //delete blocks in cache
  vcl_map<boxm2_block_id, bocl_mem*>::iterator blks;
  for (blks=cached_blocks_.begin(); blks!=cached_blocks_.end(); ++blks)
  {
    bocl_mem* toDelete = blks->second;
    delete toDelete;
  }
  cached_blocks_.clear();

  //delete data from each cache
  vcl_map<vcl_string, vcl_map<boxm2_block_id, bocl_mem*> >::iterator datas;
  for (datas=cached_data_.begin(); datas!=cached_data_.end(); ++datas)
  {
    vcl_map<boxm2_block_id, bocl_mem*>& data_map = datas->second;
    vcl_map<boxm2_block_id, bocl_mem*>::iterator data_blks;
    for (data_blks=data_map.begin(); data_blks!=data_map.end(); ++data_blks)
    {
      bocl_mem* toDelete = data_blks->second;
      delete toDelete;
    }
    data_map.clear();
  }
  cached_data_.clear();
  return true;
}

//: realization of abstract "get_block(block_id)"
bocl_mem* boxm2_opencl_cache::get_block(boxm2_block_id id)
{
  //then look for the block you're requesting
  if ( cached_blocks_.find(id) != cached_blocks_.end() ) {
    //load block info
    boxm2_block* loaded = cpu_cache_->get_block(id);
    if (block_info_) {
       boxm2_scene_info* buff = (boxm2_scene_info*) block_info_->cpu_buffer();
       delete buff;
       delete block_info_;
    }
    boxm2_scene_info* info_buffer = scene_->get_blk_metadata(id);
    info_buffer->num_buffer = loaded->num_buffers();
    info_buffer->tree_buffer_length = loaded->tree_buff_length();
    info_buffer->data_buffer_length = 65536;
    block_info_ = new bocl_mem(*context_, info_buffer, sizeof(boxm2_scene_info), "scene info buffer");
    block_info_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
    return cached_blocks_[id];
  }

  //check to see which block to kick out
  if ( cached_blocks_.size() >= (unsigned int)maxBlocksInCache ) {
    vnl_random rand;
    unsigned rIdx = rand.lrand32(0, cached_blocks_.size()-1);
    vcl_map<boxm2_block_id, bocl_mem*>::iterator iter = cached_blocks_.begin();
    for (unsigned int i=0; i<rIdx; ++i) ++iter;
    bocl_mem* toDelete = iter->second;
    delete toDelete;
    cached_blocks_.erase(iter);
  }

  //otherwise load it from disk with blocking
  boxm2_block* loaded = cpu_cache_->get_block(id);
  boxm2_array_3d<uchar16>& trees = loaded->trees();
  bocl_mem* blk = new bocl_mem(*context_, trees.data_block(), trees.size()*sizeof(uchar16), "3d trees buffer " + id.to_string() );
  blk->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  //store the requested block in the cache
  cached_blocks_[id] = blk;

  //////////////////////////////////////////////////////
  //load block info
  if (block_info_) {
    boxm2_scene_info* buff = (boxm2_scene_info*) block_info_->cpu_buffer();
    delete buff;
    delete block_info_;
  }
  boxm2_scene_info* info_buffer = scene_->get_blk_metadata(id);
  info_buffer->num_buffer = loaded->num_buffers();
  info_buffer->tree_buffer_length = loaded->tree_buff_length();
  info_buffer->data_buffer_length = 65536;
  block_info_ = new bocl_mem(*context_, info_buffer, sizeof(boxm2_scene_info), "scene info buffer");
  block_info_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  //////////////////////////////////////////////////////

  return blk;
}

bocl_mem* boxm2_opencl_cache::get_block_info(boxm2_block_id id)
{
  //clean up
  if (block_info_) {
    boxm2_scene_info* buff = (boxm2_scene_info*) block_info_->cpu_buffer();
    delete buff;
    delete block_info_;
  }

  //get block info from scene/block
  boxm2_scene_info* info_buffer = scene_->get_blk_metadata(id);
  boxm2_block* blk = cpu_cache_->get_block(id);
  info_buffer->num_buffer = blk->num_buffers();
  info_buffer->tree_buffer_length = blk->tree_buff_length();
  info_buffer->data_buffer_length = 65536;
  block_info_ = new bocl_mem(*context_, info_buffer, sizeof(boxm2_scene_info), "scene info buffer");
  block_info_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  return block_info_;
}

//: Get data generic
// Possible issue: if num_bytes is greater than 0, should it then always initialize a new data object?
bocl_mem* boxm2_opencl_cache::get_data(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes)
{
  //grab a reference to the map of cached_data_
  vcl_map<boxm2_block_id, bocl_mem*>& data_map =
    this->cached_data_map(type);

  //then look for the block you're requesting
  vcl_map<boxm2_block_id, bocl_mem*>::iterator iter = data_map.find(id);
  if ( iter != data_map.end() ) {
#if 0
    //mem currently stored
    bocl_mem* mem = data_map[id];
    if ( num_bytes > 0 && mem->num_bytes() != num_bytes )
    {
      //if the data size doesn't match, match it.
      bocl_mem* data = new bocl_mem(*context_, NULL, num_bytes, type);
      data->create_buffer(CL_MEM_READ_WRITE);
      this->deep_replace_data(id,type,data);
      data->zero_gpu_buffer(*queue_);

      delete mem;
      data_map.erase(iter);
      data_map[id] = data;
      return data;
    }
#endif // 0
    return iter->second;
  }

  //make sure the map has a legal number of data blocks
  if ( data_map.size() >= (unsigned int)maxBlocksInCache ) {
    vnl_random rand;
    unsigned rIdx = rand.lrand32(0, data_map.size()-1);
    vcl_map<boxm2_block_id, bocl_mem*>::iterator riter = data_map.begin();
    for (unsigned int i=0; i<rIdx; ++i) ++riter;
    bocl_mem* toDelete = riter->second;
    delete toDelete;
    data_map.erase(riter);
  }

  //data block isn't found...
  boxm2_data_base* data_base = cpu_cache_->get_data_base(id,type,num_bytes);
  if (num_bytes > 0 && data_base->buffer_length() != num_bytes )
  {
    bocl_mem* data = new bocl_mem(*context_, NULL, num_bytes, type);
    data->create_buffer(CL_MEM_READ_WRITE);
    this->deep_replace_data(id,type,data);
    data->zero_gpu_buffer(*queue_);
    data_map[id] = data;
    return data;
  }
  bocl_mem* data = new bocl_mem(*context_, data_base->data_buffer(), data_base->buffer_length(), type);
  data->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  data_map[id] = data;
  return data;
}

//: Deep data replace.
// This replaces not only the data in the GPU cache, but
// in the cpu cache as well (by creating a new one)
void boxm2_opencl_cache::deep_replace_data(boxm2_block_id id, vcl_string type, bocl_mem* mem)
{
  //instantiate new data block
  vcl_size_t numDataBytes = mem->num_bytes();
  boxm2_data_base* newData = new boxm2_data_base(new char[numDataBytes], numDataBytes, id);

  //write bocl_mem data to cpu buffer
  mem->set_cpu_buffer((void*) newData->data_buffer());
  mem->read_to_buffer( *queue_ );

  //do deep replace
  cpu_cache_->replace_data_base(id, type, newData);

  //now replace the mem in the GPU cache.. first delete existing
  vcl_map<boxm2_block_id, bocl_mem*>& data_map = this->cached_data_map(type);
  vcl_map<boxm2_block_id, bocl_mem*>::iterator iter = data_map.find(id);
  if ( iter != data_map.end()) {
    //release existing memory
    bocl_mem* toDelete = iter->second;
    delete toDelete;
    data_map.erase(iter);
  }
  data_map[id] = mem;
}


//: helper method returns a reference to correct data map (ensures one exists)
vcl_map<boxm2_block_id, bocl_mem*>& boxm2_opencl_cache::cached_data_map(vcl_string prefix)
{
  // if map for this particular data type doesn't exist, initialize it
  if ( cached_data_.find(prefix) == cached_data_.end() )
  {
    vcl_map<boxm2_block_id, bocl_mem*> dmap;
    cached_data_[prefix] = dmap;
  }

  //grab a reference to the map of cached_data_ and return it
  vcl_map<boxm2_block_id, bocl_mem*>& data_map = cached_data_[prefix];
  return data_map;
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
