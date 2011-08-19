#include "boxm2_opencl_cache.h"
#include "vnl/vnl_random.h"
#include "vcl_sstream.h"
//:
// \file


//: scene/device constructor
boxm2_opencl_cache::boxm2_opencl_cache(boxm2_scene_sptr scene,
                                       bocl_device_sptr device,
                                       unsigned int maxBlocks)
: scene_(scene), maxBlocksInCache(maxBlocks), bytesInCache_(0), block_info_(0), device_(device)
{
  // store max bytes allowed in cache - use only 80 percent of the memory
  maxBytesInCache_ = (unsigned long) (device->info().total_global_memory_ * .85);

  // by default try to create an LRU cache
  boxm2_lru_cache::create(scene);
  cpu_cache_ = boxm2_cache::instance();

  // store the device pointer and context associated with the device
  context_ = &device->context();

  // create command queue... make sure this isn't destructed at the end of the function...
  int status = 0;
  q_ = clCreateCommandQueue(*context_,
                            *device->device_id(),
                            CL_QUEUE_PROFILING_ENABLE,
                            &status);
  if (!check_val(status,CL_SUCCESS,"boxm2_opencl_cache:: failed in command queue creation" + error_to_string(status)))
    return;
  queue_ = &q_;
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
  // delete stored block info
  if (block_info_) {
    boxm2_scene_info* buff = (boxm2_scene_info*) block_info_->cpu_buffer();
    delete buff;
    delete block_info_;
    block_info_=0;
  }

  // delete blocks in cache
  vcl_map<boxm2_block_id, bocl_mem*>::iterator blks;
  for (blks=cached_blocks_.begin(); blks!=cached_blocks_.end(); ++blks)
  {
    bocl_mem* toDelete = blks->second;
    bytesInCache_ -= toDelete->num_bytes();
#ifdef DEBUG
    vcl_cout<<"Deleting block: "<<toDelete->id()<<"...size: "<<toDelete->num_bytes()<<vcl_endl;
#endif
    delete toDelete;
  }
  cached_blocks_.clear();

  // delete data from each cache
  vcl_map<vcl_string, vcl_map<boxm2_block_id, bocl_mem*> >::iterator datas;
  for (datas=cached_data_.begin(); datas!=cached_data_.end(); ++datas)
  {
    vcl_map<boxm2_block_id, bocl_mem*>& data_map = datas->second;
    vcl_map<boxm2_block_id, bocl_mem*>::iterator data_blks;
    for (data_blks=data_map.begin(); data_blks!=data_map.end(); ++data_blks)
    {
      bocl_mem* toDelete = data_blks->second;
      bytesInCache_ -= toDelete->num_bytes();
      boxm2_block_id bid = data_blks->first;
#ifdef DEBUG
      vcl_cout<<"Deleting data type: "<<datas->first<<" id "<<bid<<"..size: "<<toDelete->num_bytes()<<vcl_endl;
#endif
      delete toDelete;
    }
    data_map.clear();
  }
  cached_data_.clear();

  //clear LRU list
  lru_order_.clear(); 

  // notify exceptional case
  return true;
}

long boxm2_opencl_cache::bytes_in_cache()
{
  // count up bytes in cache (like clearing, but no deleting...
  long count = 0;

  vcl_map<boxm2_block_id, bocl_mem*>::iterator blks;
  for (blks=cached_blocks_.begin(); blks!=cached_blocks_.end(); ++blks)
  {
    bocl_mem* curr = blks->second;
    count += curr->num_bytes();
  }

  // delete data from each cache
  vcl_map<vcl_string, vcl_map<boxm2_block_id, bocl_mem*> >::iterator datas;
  for (datas=cached_data_.begin(); datas!=cached_data_.end(); ++datas)
  {
    vcl_map<boxm2_block_id, bocl_mem*>& data_map = datas->second;
    vcl_map<boxm2_block_id, bocl_mem*>::iterator data_blks;
    for (data_blks=data_map.begin(); data_blks!=data_map.end(); ++data_blks)
    {
      bocl_mem* curr = data_blks->second;
      count += curr->num_bytes();
    }
  }
  return count;
}

//: realization of abstract "get_block(block_id)"
bocl_mem* boxm2_opencl_cache::get_block(boxm2_block_id id)
{
  //requesting block pushes it to the front of the list
  this->lru_push_front(id); 
  
  // then look for the block you're requesting
  if ( cached_blocks_.find(id) != cached_blocks_.end() ) {
    // load block info
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

  // check to see which block to kick out
  // grab block from CPU cache and see if the GPU cache needs some cleaning
  boxm2_block* loaded = cpu_cache_->get_block(id);
  boxm2_array_3d<uchar16>& trees = loaded->trees();
  vcl_size_t toLoadSize = trees.size()*sizeof(uchar16);
  while ( this->bytes_in_cache()+toLoadSize > maxBytesInCache_ && !cached_blocks_.empty() )
  {
    vcl_cout<<"Bytes in cache: "<<bytesInCache_<<" max bytes in cache! "<<maxBytesInCache_<<vcl_endl;
    boxm2_block_id lru_id = this->lru_remove_last(); 
    if(lru_id == id) vcl_cout<<"boxm2_opencl_cache:: Single Block Size is too big for GPU RAM "<<vcl_endl;
  }

  // otherwise load it from disk with blocking
  bocl_mem* blk = new bocl_mem(*context_, trees.data_block(), toLoadSize, "3d trees buffer " + id.to_string() );
  blk->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  bytesInCache_ += blk->num_bytes();

  // store the requested block in the cache
  cached_blocks_[id] = blk;

  //////////////////////////////////////////////////////
  // load block info
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
  // clean up
  if (block_info_) {
    boxm2_scene_info* buff = (boxm2_scene_info*) block_info_->cpu_buffer();
    delete buff;
    delete block_info_;
  }

  // get block info from scene/block
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
// Possible issue: if \p num_bytes is greater than 0, should it then always initialize a new data object?
bocl_mem* boxm2_opencl_cache::get_data(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes, bool read_only)
{
  //push id to front of LRU list
  this->lru_push_front(id); 
  
  // grab a reference to the map of cached_data_
  vcl_map<boxm2_block_id, bocl_mem*>& data_map =
    this->cached_data_map(type);

  // then look for the block you're requesting
  vcl_map<boxm2_block_id, bocl_mem*>::iterator iter = data_map.find(id);
  if ( iter != data_map.end() ) {
    return iter->second;
  }

  // load data into CPU cache and check size to see if GPU cache needs cleaning
  boxm2_data_base* data_base = cpu_cache_->get_data_base(id,type,num_bytes,read_only);
  vcl_size_t toLoadSize;
  if (num_bytes > 0 && data_base->buffer_length() != num_bytes )
    toLoadSize = num_bytes;
  else
    toLoadSize = data_base->buffer_length();

  // make enough space by kicking out blocks
  while ( this->bytes_in_cache()+toLoadSize > maxBytesInCache_ && !data_map.empty() ) // was: data_map.size() >= maxBlocksInCache
  {
    vcl_cout<<"Bytes in cache: "<<bytesInCache_<<" max bytes in cache! "<<maxBytesInCache_<<vcl_endl;
    boxm2_block_id lru_id = this->lru_remove_last(); 
    if(lru_id == id) vcl_cout<<"boxm2_opencl_cache:: Single Block Size is too big for GPU RAM "<<vcl_endl;
  }

  // data block isn't found...
  // if num bytes is specified and the data doesn't match, create empty buffer
  if (num_bytes > 0 && data_base->buffer_length() != num_bytes )
  {
    bocl_mem* data = new bocl_mem(*context_, NULL, num_bytes, type);
    data->create_buffer(CL_MEM_READ_WRITE);
    this->deep_replace_data(id,type,data,read_only);
    data->zero_gpu_buffer(*queue_);
    data_map[id] = data;
    bytesInCache_ += data->num_bytes();
    return data;
  }

  // otherwise initialize buffer from CPU cache and return
  bocl_mem* data = new bocl_mem(*context_, data_base->data_buffer(), data_base->buffer_length(), type);
  data->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bytesInCache_ += data->num_bytes();
  data_map[id] = data;
  return data;
}

//: Deep data replace.
// This replaces not only the data in the GPU cache, but
// in the cpu cache as well (by creating a new one)
void boxm2_opencl_cache::deep_replace_data(boxm2_block_id id, vcl_string type, bocl_mem* mem, bool read_only)
{
  // instantiate new data block
  vcl_size_t numDataBytes = mem->num_bytes();

  //TODO: figure out consistent scheme to make this read_only or read_write
  boxm2_data_base* newData = new boxm2_data_base(new char[numDataBytes], numDataBytes, id, read_only);

  // write bocl_mem data to cpu buffer
  mem->set_cpu_buffer((void*) newData->data_buffer());
  mem->read_to_buffer( *queue_ );

  // do deep replace
  cpu_cache_->replace_data_base(id, type, newData);

  // now replace the mem in the GPU cache.. first delete existing
  vcl_map<boxm2_block_id, bocl_mem*>& data_map = this->cached_data_map(type);
  vcl_map<boxm2_block_id, bocl_mem*>::iterator iter = data_map.find(id);
  if ( iter != data_map.end() ) {
    // release existing memory
    bocl_mem* toDelete = iter->second;
    delete toDelete;
    data_map.erase(iter);
  }
  data_map[id] = mem;
}


//: helper method, \returns a reference to correct data map (ensures one exists)
vcl_map<boxm2_block_id, bocl_mem*>& boxm2_opencl_cache::cached_data_map(vcl_string prefix)
{
  // if map for this particular data type doesn't exist, initialize it
  if ( cached_data_.find(prefix) == cached_data_.end() )
  {
    vcl_map<boxm2_block_id, bocl_mem*> dmap;
    cached_data_[prefix] = dmap;
  }

  // grab a reference to the map of cached_data_ and return it
  vcl_map<boxm2_block_id, bocl_mem*>& data_map = cached_data_[prefix];
  return data_map;
}

//: helper method to insert into LRU list
void boxm2_opencl_cache::lru_push_front( boxm2_block_id id )
{
  //serach for it in the list, if it's there, delete it
  vcl_list<boxm2_block_id>::iterator iter;
  for(iter=lru_order_.begin(); iter!=lru_order_.end(); ++iter) {
    if( *iter == id ) {
      lru_order_.erase(iter); 
      break;
    }
  }
  
  //push to front of list
  lru_order_.push_front(id); 
}

//: helper to remove all data and block memory by ID
boxm2_block_id boxm2_opencl_cache::lru_remove_last() 
{
  vcl_cout<<"LRU CACHE STATE: \n"<<this->to_string()<<vcl_endl;
  
  //grab and remove last element
  boxm2_block_id lru_id = lru_order_.back();
  lru_order_.pop_back(); 
   
  // then look for the block to delete
  vcl_map<boxm2_block_id, bocl_mem*>::iterator blk = cached_blocks_.find(lru_id);
  if ( blk != cached_blocks_.end() ) {
    bocl_mem* toDelete = blk->second;
    bytesInCache_ -= toDelete->num_bytes();
    cached_blocks_.erase(blk);
    delete toDelete;
  }
  else {
    vcl_cout<<"boxm2_opencl_cache::lru_remove_last failed to find last element of list"<<vcl_endl;
  }
  
  //now look for data to delete
  vcl_map<vcl_string, vcl_map<boxm2_block_id, bocl_mem*> >::iterator datas;
  for (datas=cached_data_.begin(); datas!=cached_data_.end(); ++datas)
  {
    vcl_string data_type = datas->first; 
    vcl_map<boxm2_block_id, bocl_mem*>& data_map = datas->second;
    vcl_map<boxm2_block_id, bocl_mem*>::iterator dat = data_map.find(lru_id);
    if ( dat != data_map.end() ) {
      bocl_mem* toDelete = dat->second;
      bytesInCache_ -= toDelete->num_bytes();
      data_map.erase(dat); 
      delete toDelete; 
    }
  }
  
  return lru_id; 
}


vcl_string boxm2_opencl_cache::to_string() {
  vcl_stringstream s; 
  s << "boxm2_opencl_cache::order: "; 
  vcl_list<boxm2_block_id>::iterator iter; 
  for(iter=lru_order_.begin(); iter!=lru_order_.end(); ++iter)
  {
    boxm2_block_id id = (*iter); 
    s << id << " ";
  }
  s << vcl_endl;
  return s.str(); 
}

// === Dummy (empty) instantiations for binary I/O

void vsl_b_write(vsl_b_ostream& os, boxm2_opencl_cache const& scene) {}
void vsl_b_write(vsl_b_ostream& os, const boxm2_opencl_cache* &p) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_opencl_cache_sptr& sptr) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_opencl_cache_sptr const& sptr) {}

void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache &scene) {}
void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache* p) {}
void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache_sptr& sptr) {}
void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache_sptr const& sptr) {}
