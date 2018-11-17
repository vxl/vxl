#include <utility>

#include "bstm_opencl_cache.h"
//:
// \file
//#define DEBUG

//: scene/device constructor
bstm_opencl_cache
::bstm_opencl_cache(const bstm_scene_sptr& scene,
                    const bocl_device_sptr& device) :
  scene_(scene),
  bytesInCache_(0),
  block_info_(nullptr),
  block_info_t_(nullptr),
  device_(device)
{
  // store max bytes allowed in cache - use only 80 percent of the memory
  unsigned long total_global_mem =  device->info().total_global_memory_;
  if(device->info().addr_bits_ == 32 && total_global_mem > (cl_ulong)4096000000)
    total_global_mem =   (cl_ulong)4096000000;

  maxBytesInCache_ = (unsigned long) (total_global_mem * .9);

  std::cout << "Opencl cache max bytes in cache: " << maxBytesInCache_ << std::endl;
  // by default try to create an LRU cache
  bstm_lru_cache::create(scene);
  cpu_cache_ = bstm_cache::instance();

  // store the device pointer and context associated with the device
  context_ = &device->context();

  // create command queue... make sure this isn't destructed at the end of the function...
  int status = 0;
  q_ = clCreateCommandQueue(*context_,
                            *device->device_id(),
                            CL_QUEUE_PROFILING_ENABLE,
                            &status);
  if (!check_val(status,CL_SUCCESS,"bstm_opencl_cache:: failed in command queue creation " + error_to_string(status)))
    return;
  queue_ = &q_;
}

//: sets a context in case the default context off the device is old
// (this  is used for the tableau's shared cl_gl context)
void
bstm_opencl_cache
::set_context(cl_context& context)
{
  context_ = &context;
  int status = clReleaseCommandQueue(q_);
  if (!check_val(status,CL_SUCCESS,"bstm_opencl_cache::set_context failed to release old queue " + error_to_string(status)))
    return;

  q_ = clCreateCommandQueue(*context_,
                            *device_->device_id(),
                            CL_QUEUE_PROFILING_ENABLE,
                            &status);
  if (!check_val(status,CL_SUCCESS,"bstm_opencl_cache::set_context failed in command queue creation " + error_to_string(status)))
    return;
  queue_ = &q_;
}

bool
bstm_opencl_cache
::clear_cache()
{
  // delete stored block info
  if (block_info_) {
    auto* buff = (bstm_scene_info*) block_info_->cpu_buffer();
    delete buff;
    delete block_info_;
    block_info_=nullptr;
  }

  if (block_info_t_) {
    auto* buff = (bstm_scene_info*) block_info_t_->cpu_buffer();
    delete buff;
    delete block_info_t_;
    block_info_t_=nullptr;
  }


  // delete blocks in cache
  std::map<bstm_block_id, bocl_mem*>::iterator blks;
  // delete time blocks in cache
  for (blks=cached_time_blocks_.begin(); blks!=cached_time_blocks_.end(); ++blks)
  {
    bocl_mem* toDelete = blks->second;
    //toDelete->read_to_buffer( *queue_ );
    bytesInCache_ -= toDelete->num_bytes();
#ifdef DEBUG
    std::cout<<"Deleting time block: "<<toDelete->id()<<"...size: "<<toDelete->num_bytes()<<std::endl;
#endif
    delete toDelete;
  }
  cached_time_blocks_.clear();


  for (blks=cached_blocks_.begin(); blks!=cached_blocks_.end(); ++blks)
  {
    bocl_mem* toDelete = blks->second;
    //toDelete->read_to_buffer( *queue_ );
    bytesInCache_ -= toDelete->num_bytes();
#ifdef DEBUG
    std::cout<<"Deleting block: "<<toDelete->id()<<"...size: "<<toDelete->num_bytes()<<std::endl;
#endif
    delete toDelete;
  }
  cached_blocks_.clear();



  // delete data from each cache
  std::map<std::string, std::map<bstm_block_id, bocl_mem*> >::iterator datas;
  for (datas=cached_data_.begin(); datas!=cached_data_.end(); ++datas)
  {
    std::map<bstm_block_id, bocl_mem*>& data_map = datas->second;
    std::map<bstm_block_id, bocl_mem*>::iterator data_blks;
    for (data_blks=data_map.begin(); data_blks!=data_map.end(); ++data_blks)
    {
      bocl_mem* toDelete = data_blks->second;
      //toDelete->read_to_buffer( *queue_ );
      bytesInCache_ -= toDelete->num_bytes();
      bstm_block_id bid = data_blks->first;
#ifdef DEBUG
      std::cout<<"Deleting data type: "<<datas->first<<" id "<<bid<<"..size: "<<toDelete->num_bytes()<<std::endl;
#endif
      delete toDelete;
    }
    data_map.clear();
#ifdef DEBUG
      std::cout<<"Deleted data type: "<<datas->first <<std::endl;
#endif
  }
  cached_data_.clear();

  //clear LRU list
  lru_order_.clear();

  // notify exceptional case
  return true;
}

std::size_t
bstm_opencl_cache
::bytes_in_cache()
{
  // count up bytes in cache (like clearing, but no deleting...
  std::size_t count = 0;

  std::map<bstm_block_id, bocl_mem*>::iterator blks;
  for (blks=cached_blocks_.begin(); blks!=cached_blocks_.end(); ++blks)
  {
    bocl_mem* curr = blks->second;
    count += curr->num_bytes();
  }

  for (blks=cached_time_blocks_.begin(); blks!=cached_time_blocks_.end(); ++blks)
  {
    bocl_mem* curr = blks->second;
    count += curr->num_bytes();
  }


  // count bstm_data mem sizes
  std::map<std::string, std::map<bstm_block_id, bocl_mem*> >::iterator datas;
  for (datas=cached_data_.begin(); datas!=cached_data_.end(); ++datas)
  {
    std::map<bstm_block_id, bocl_mem*>& data_map = datas->second;
    std::map<bstm_block_id, bocl_mem*>::iterator data_blks;
    for (data_blks=data_map.begin(); data_blks!=data_map.end(); ++data_blks)
    {
      bocl_mem* curr = data_blks->second;
      count += curr->num_bytes();
    }
  }

  //count mem pool sizes
  std::map<bocl_mem*, std::size_t>::iterator mems;
  for (mems=mem_pool_.begin(); mems!=mem_pool_.end(); ++mems)
  {
    bocl_mem* curr = mems->first;
    count += curr->num_bytes();
  }
  return count;
}

std::size_t
bstm_opencl_cache
::bytes_in_mem_pool()
{
  // count up bytes in cache (like clearing, but no deleting...
  std::size_t count = 0;

  //count mem pool sizes
  std::map<bocl_mem*, std::size_t>::iterator mems;
  for (mems=mem_pool_.begin(); mems!=mem_pool_.end(); ++mems)
  {
    bocl_mem* curr = mems->first;
    count += curr->num_bytes();
  }
  return count;
}

//: realization of abstract "get_block(block_id)"
bocl_mem*
bstm_opencl_cache
::get_block(const bstm_block_id& id)
{
  //requesting block pushes it to the front of the list
  this->lru_push_front(id);

  // then look for the block you're requesting
  if ( cached_blocks_.find(id) != cached_blocks_.end() ) {
    // load block info
    bstm_block* loaded = cpu_cache_->get_block(id);
    if (block_info_) {
       auto* buff = (bstm_scene_info*) block_info_->cpu_buffer();
       delete buff;
       delete block_info_;
    }
    bstm_scene_info* info_buffer = populate_scene_info( scene_->get_block_metadata(id));
    info_buffer->tree_buffer_length = loaded->tree_buff_length();
    info_buffer->data_buffer_length = 65536;
    block_info_ = new bocl_mem(*context_, info_buffer, sizeof(bstm_scene_info), "scene info buffer");
    block_info_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
    return cached_blocks_[id];
  }

  // check to see which block to kick out
  // grab block from CPU cache and see if the GPU cache needs some cleaning
  bstm_block* loaded = cpu_cache_->get_block(id);
  boxm2_array_3d<uchar16>& trees = loaded->trees();
  std::size_t toLoadSize = trees.size()*sizeof(uchar16);
  unsigned long totalBytes = this->bytes_in_cache() + toLoadSize;
  if (totalBytes > maxBytesInCache_) {

#ifdef DEBUG
    std::cout<<"Loading Block "<<id<<" uses "<<totalBytes<<" out of  "<<maxBytesInCache_<<std::endl
            <<"    removing... ";
#endif
    while ( this->bytes_in_cache()+toLoadSize > maxBytesInCache_ && !cached_blocks_.empty() )
    {
      bstm_block_id lru_id;
      if (!this->lru_remove_last(lru_id)) {
         std::cerr << "ERROR: bstm_opencl_cache::get_block(): lru is empty" << std::endl;
         return (bocl_mem*)nullptr;
      }
#ifdef DEBUG
      std::cout<<lru_id<<" ... ";
#endif
      if (lru_id == id)
        std::cout<<"bstm_opencl_cache:: Single Block Size is too big for GPU RAM"<<std::endl;
    }
#ifdef DEBUG
    std::cout<<std::endl;
#endif
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
    auto* buff = (bstm_scene_info*) block_info_->cpu_buffer();
    delete buff;
    delete block_info_;
  }
  bstm_scene_info* info_buffer = populate_scene_info( scene_->get_block_metadata(id));
  info_buffer->tree_buffer_length = loaded->tree_buff_length();
  info_buffer->data_buffer_length = 65536;
  block_info_ = new bocl_mem(*context_, info_buffer, sizeof(bstm_scene_info), "scene info buffer");
  block_info_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  //////////////////////////////////////////////////////

  return blk;
}


//: realization of abstract "get_block(block_id)"
bocl_mem*
bstm_opencl_cache
::get_time_block(const bstm_block_id& id)
{
  //requesting block pushes it to the front of the list
  this->lru_push_front(id);

  // then look for the block you're requesting
  if ( cached_time_blocks_.find(id) != cached_time_blocks_.end() ) {
    // load block info
    bstm_time_block* loaded = cpu_cache_->get_time_block(id);
    if (block_info_t_) {
       auto* buff = (bstm_scene_info*) block_info_t_->cpu_buffer();
       delete buff;
       delete block_info_t_;
    }
    bstm_scene_info* info_buffer = populate_scene_info( scene_->get_block_metadata(id));
    info_buffer->tree_buffer_length = loaded->tree_buff_length();
    info_buffer->data_buffer_length = 65536;
    block_info_t_ = new bocl_mem(*context_, info_buffer, sizeof(bstm_scene_info), "scene info buffer");
    block_info_t_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
    return cached_time_blocks_[id];
  }

  // check to see which block to kick out
  // grab block from CPU cache and see if the GPU cache needs some cleaning
  bstm_time_block* loaded = cpu_cache_->get_time_block(id);
  boxm2_array_1d<uchar8>& trees = loaded->time_trees();
  std::size_t toLoadSize = trees.size()*sizeof(uchar8);
  unsigned long totalBytes = this->bytes_in_cache() + toLoadSize;
  if (totalBytes > maxBytesInCache_) {
    while ( this->bytes_in_cache()+toLoadSize > maxBytesInCache_ && !cached_blocks_.empty() )
    {
      bstm_block_id lru_id;
      if (!this->lru_remove_last(lru_id)) {
         std::cerr << "ERROR: bstm_opencl_cache::get_block(): lru is empty" << std::endl;
         return (bocl_mem*)nullptr;
      }
      if (lru_id == id)
        std::cout<<"bstm_opencl_cache:: Single Block Size is too big for GPU RAM"<<std::endl;
    }
  }

  // otherwise load it from disk with blocking
  bocl_mem* blk = new bocl_mem(*context_, trees.begin(), toLoadSize, "1d trees buffer " + id.to_string() );
  blk->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  bytesInCache_ += blk->num_bytes();

  // store the requested block in the cache
  cached_time_blocks_[id] = blk;

  //////////////////////////////////////////////////////
  // load block info
  if (block_info_t_) {
    auto* buff = (bstm_scene_info*) block_info_t_->cpu_buffer();
    delete buff;
    delete block_info_t_;
  }
  bstm_scene_info* info_buffer = populate_scene_info( scene_->get_block_metadata(id));
  info_buffer->tree_buffer_length = loaded->tree_buff_length();
  info_buffer->data_buffer_length = 65536;
  block_info_t_ = new bocl_mem(*context_, info_buffer, sizeof(bstm_scene_info), "scene info buffer");
  block_info_t_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  //////////////////////////////////////////////////////

  return blk;
}


bocl_mem*
bstm_opencl_cache
::get_block_info(const bstm_block_id& id)
{
  // clean up
  if (block_info_) {
    auto* buff = (bstm_scene_info*) block_info_->cpu_buffer();
    delete buff;
    delete block_info_;
  }

  // get block info from scene/block
  bstm_scene_info* info_buffer = populate_scene_info( scene_->get_block_metadata(id) );
  bstm_block* blk = cpu_cache_->get_block(id);
  info_buffer->tree_buffer_length = blk->tree_buff_length();
  info_buffer->data_buffer_length = 65536;
  block_info_ = new bocl_mem(*context_, info_buffer, sizeof(bstm_scene_info), "scene info buffer");
  block_info_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  return block_info_;
}

//: Get data generic
// Possible issue: if \p num_bytes is greater than 0, should it then always initialize a new data object?
bocl_mem*
bstm_opencl_cache
::get_data(const bstm_block_id& id, const std::string& type, std::size_t num_bytes, bool read_only)
{
  //push id to front of LRU list
  this->lru_push_front(id);

  // grab a reference to the map of cached_data_
  std::map<bstm_block_id, bocl_mem*>& data_map = this->cached_data_map(type);

  // then look for the block you're requesting
  auto iter = data_map.find(id);
  if ( iter != data_map.end() ) {
    if(num_bytes == 0 || iter->second->num_bytes() == num_bytes) // congrats you've found the data block in cache, update cache and return block
      return iter->second;
    else                                                        // congrats you've found the data block in cache, but it isn't the size you wanted, so delete it.
    {
      delete iter->second;
      data_map.erase(iter);
    }
  }

  // load data into CPU cache and check size to see if GPU cache needs cleaning
  bstm_data_base* data_base = cpu_cache_->get_data_base(id,type,num_bytes,read_only);

  std::size_t toLoadSize;
  if (num_bytes > 0 && data_base->buffer_length() != num_bytes )
    toLoadSize = num_bytes;
  else
    toLoadSize = data_base->buffer_length();

  // make enough space by kicking out blocks
  std::size_t totalBytes = this->bytes_in_cache() + toLoadSize;
  if (totalBytes > maxBytesInCache_) {
#ifdef DEBUG
    std::cout<<"Loading data "<<id<<" type "<<type<<" uses "<<totalBytes<<" out of  "<<maxBytesInCache_<<std::endl
            <<"    removing... ";
#endif
    while ( this->bytes_in_cache()+toLoadSize*2 > maxBytesInCache_ && !data_map.empty() )
    {
      bstm_block_id lru_id;
      if(!this->lru_remove_last(lru_id)) {
         std::cerr << "ERROR: bstm_opencl_cache::get_data() : lru is empty" << std::endl;
         return (bocl_mem*)nullptr;
      }
#ifdef DEBUG
      std::cout<<lru_id<<" ... ";
#endif
      if (lru_id == id)
        std::cout<<"bstm_opencl_cache:: Single Block Size is too big for GPU RAM"<<std::endl;
    }
#ifdef DEBUG
    std::cout<<std::endl;
#endif
  }

  // data block isn't found...
  // if num bytes is specified and the data doesn't match, create empty buffer
  if (num_bytes > 0 && data_base->buffer_length() != num_bytes )
  {
    bocl_mem* data = new bocl_mem(*context_, nullptr, num_bytes, type);
    data->create_buffer(CL_MEM_READ_WRITE);
    this->deep_replace_data(id,type,data,read_only);
    //data->zero_gpu_buffer(*queue_);
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


//: Get data new generic
// Possible issue: if \p num_bytes is greater than 0, should it then always initialize a new data object?
bocl_mem*
bstm_opencl_cache
::get_data_new(const bstm_block_id& id, const std::string& type, std::size_t num_bytes, bool read_only)
{
  //push id to front of LRU list
  this->lru_push_front(id);

  // grab a reference to the map of cached_data_
  std::map<bstm_block_id, bocl_mem*>& data_map = this->cached_data_map(type);

  // then look for the block you're requesting, if found, delete it.
  auto iter = data_map.find(id);
  if ( iter != data_map.end() ) {
    delete iter->second;
    data_map.erase(iter);
  }

  // load new data into CPU cache and check size to see if GPU cache needs cleaning
  bstm_data_base* data_base = cpu_cache_->get_data_base_new(id,type,num_bytes,read_only);
  std::size_t toLoadSize = data_base->buffer_length();

  // make enough space by kicking out blocks
  std::size_t totalBytes = this->bytes_in_cache() + toLoadSize;
  if (totalBytes > maxBytesInCache_) {
#ifdef DEBUG
    std::cout<<"Loading data "<<id<<" type "<<type<<" uses "<<totalBytes<<" out of  "<<maxBytesInCache_<<std::endl
            <<"    removing... ";
#endif
    while ( this->bytes_in_cache()+toLoadSize > maxBytesInCache_ && !data_map.empty() )
    {
      bstm_block_id lru_id;
      if (!this->lru_remove_last(lru_id)) {
         std::cerr << "ERROR: bstm_opencl_cache::get_data_new() : lru is empty " << std::endl;
         return (bocl_mem*)nullptr;
      }
#ifdef DEBUG
      std::cout<<lru_id<<" ... ";
#endif
      if (lru_id == id)
        std::cout<<"bstm_opencl_cache:: Single Block Size is too big for GPU RAM"<<std::endl;
    }
#ifdef DEBUG
    std::cout<<std::endl;
#endif
  }

  // initialize buffer from CPU cache and return
  bocl_mem* data = new bocl_mem(*context_, data_base->data_buffer(), data_base->buffer_length(), type);
  data->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bytesInCache_ += data->num_bytes();
  data_map[id] = data;
  return data;
}


//----------------------------------------------------------------------------
// Methods for opencl objects not in the CPU cache (images, some aux data)
//---------------------------------------------------------------------------
//: returns a flat bocl_mem of a certain size
bocl_mem*
bstm_opencl_cache
::alloc_mem(std::size_t num_bytes, void* cpu_buff, std::string id)
{
  std::size_t totalBytes = this->bytes_in_cache()+num_bytes;
  if (totalBytes > maxBytesInCache_) {
#ifdef DEBUG
    std::cout<<"OCL cache alloc mem "<<" uses "<<totalBytes<<" out of  "<<maxBytesInCache_<<std::endl
            <<"    removing... ";
#endif
    while ( this->bytes_in_cache()+num_bytes > maxBytesInCache_ )
    {
      bstm_block_id lru_id;
      if (!this->lru_remove_last(lru_id)) {
         std::cerr << "ERROR: lru empty. unable to alloc buffer of requested size. " << std::endl;
         return (bocl_mem*)nullptr;
      }
#ifdef DEBUG
      std::cout<<lru_id<<" ... ";
#endif
    }
#ifdef DEBUG
    std::cout<<std::endl;
#endif
  }

  //allocate mem
  bocl_mem* data = new bocl_mem(*context_, cpu_buff, num_bytes, std::move(id));
  mem_pool_[data] = num_bytes;
  return data;
}

void
bstm_opencl_cache
::free_mem_pool()
{
  std::map<bocl_mem*,std::size_t>::iterator iter;
  for (iter = mem_pool_.begin(); iter!=mem_pool_.end(); ++iter){
    bocl_mem* toDelete = iter->first;
    delete toDelete;
    mem_pool_.erase(iter);
  }
  mem_pool_.clear();
}

//removes mem from pool, but still keeps it allocated
void
bstm_opencl_cache
::unref_mem(bocl_mem* mem)
{
  auto iter = mem_pool_.find(mem);
  if (iter != mem_pool_.end()){
    mem_pool_.erase(iter);
  }
}

void
bstm_opencl_cache
::free_mem(bocl_mem* mem)
{
  this->unref_mem(mem);
  delete mem;
}


//------------------------------------------------------------------------------
// deep replace/remove, removing or replacing the buffer in the cpu cache as well
//------------------------------------------------------------------------------
//: Deep data replace.
// This replaces not only the data in the GPU cache, but
// in the cpu cache as well (by creating a new one)
void
bstm_opencl_cache
::deep_replace_data(const bstm_block_id& id, const std::string& type, bocl_mem* mem, bool read_only)
{
  // instantiate new data block
  std::size_t numDataBytes = mem->num_bytes();

  //TODO: figure out consistent scheme to make this read_only or read_write
  bstm_data_base* newData = new bstm_data_base(new char[numDataBytes], numDataBytes, id, read_only);

  // write bocl_mem data to cpu buffer
  mem->set_cpu_buffer((void*) newData->data_buffer());
  mem->read_to_buffer( *queue_ );

  // do deep replace
  cpu_cache_->replace_data_base(id, type, newData);

  // now replace the mem in the GPU cache.. first delete existing
  std::map<bstm_block_id, bocl_mem*>& data_map = this->cached_data_map(type);
  auto iter = data_map.find(id);
  if ( iter != data_map.end() ) {
    // release existing memory
    bocl_mem* toDelete = iter->second;
    delete toDelete;
    data_map.erase(iter);
  }
  data_map[id] = mem;
}

//: deep remove data, removes from ocl cache as well
void
bstm_opencl_cache
::deep_remove_data(const bstm_block_id& id, const std::string& type, bool  /*write_out*/)
{
  //find the data in this map
  std::map<bstm_block_id, bocl_mem*>& data_map = this->cached_data_map(type);
  auto iter = data_map.find(id);
  if ( iter != data_map.end() ) {
    // release existing memory
    bocl_mem* toDelete = iter->second;
    delete toDelete;
    data_map.erase(iter);
  }

  //remove from cpu_cache_
  cpu_cache_->remove_data_base(id, type);
#if 0
  remove from lru_order
  std::list<bstm_block_id>::iterator loc = std::find(lru_order_.begin(), lru_order_.end(), id);
  if (loc != lru_order_.end())
    lru_order_.erase(loc);
#endif
}

//: shallow_remove_data removes data with id and type from ocl cache only
void
bstm_opencl_cache
::shallow_remove_data(const bstm_block_id& id, std::string type)
{
  //find the data in this map
  std::map<bstm_block_id, bocl_mem*>& data_map = this->cached_data_map(std::move(type));
  auto iter = data_map.find(id);
  if ( iter != data_map.end() ) {
    // release existing memory
    bocl_mem* toDelete = iter->second;
    delete toDelete;
    data_map.erase(iter);
  }
}

//: helper method, \returns a reference to correct data map (ensures one exists)
std::map<bstm_block_id, bocl_mem*>&
bstm_opencl_cache
::cached_data_map(const std::string& prefix)
{
  // if map for this particular data type doesn't exist, initialize it
  if ( cached_data_.find(prefix) == cached_data_.end() )
  {
    std::map<bstm_block_id, bocl_mem*> dmap;
    cached_data_[prefix] = dmap;
  }

  // grab a reference to the map of cached_data_ and return it
  std::map<bstm_block_id, bocl_mem*>& data_map = cached_data_[prefix];
  return data_map;
}

//: helper method to insert into LRU list
void
bstm_opencl_cache
::lru_push_front( const bstm_block_id& id )
{
  //serach for it in the list, if it's there, delete it
  std::list<bstm_block_id>::iterator iter;
  for (iter=lru_order_.begin(); iter!=lru_order_.end(); ++iter) {
    if ( *iter == id ) {
      lru_order_.erase(iter);
      break;
    }
  }

  //push to front of list
  lru_order_.push_front(id);
}

//: helper to remove all data and block memory by ID
bool
bstm_opencl_cache
::lru_remove_last(bstm_block_id &lru_id)
{
  //grab and remove last element
  if (lru_order_.empty()) {
     std::cerr << "ERROR: bstm_opencl_cache::lru_remove_last() : LRU is empty " << std::endl;
     return false;
  }
  lru_id = lru_order_.back();
  lru_order_.pop_back();

  // then look for the block to delete
  auto blk = cached_blocks_.find(lru_id);
  if ( blk != cached_blocks_.end() ) {
    bocl_mem* toDelete = blk->second;
    //toDelete->read_to_buffer( *queue_ );
    bytesInCache_ -= toDelete->num_bytes();
    cached_blocks_.erase(blk);
    delete toDelete;
  }
  else {
    std::cout<<"bstm_opencl_cache::lru_remove_last failed to find last element of list"<<std::endl;
  }


  // then look for the time block to delete
  blk = cached_time_blocks_.find(lru_id);
  if ( blk != cached_time_blocks_.end() ) {
    bocl_mem* toDelete = blk->second;
    //toDelete->read_to_buffer( *queue_ );
    bytesInCache_ -= toDelete->num_bytes();
    cached_time_blocks_.erase(blk);
    delete toDelete;
  }
  else {
    std::cout<<"bstm_opencl_cache::lru_remove_last failed to find last element of list (time block) "<<std::endl;
  }

  //now look for data to delete
  std::map<std::string, std::map<bstm_block_id, bocl_mem*> >::iterator datas;
  for (datas=cached_data_.begin(); datas!=cached_data_.end(); ++datas)
  {
    std::string data_type = datas->first;
    std::map<bstm_block_id, bocl_mem*>& data_map = datas->second;
    auto dat = data_map.find(lru_id);
    if ( dat != data_map.end() ) {
      bocl_mem* toDelete = dat->second;
      //toDelete->read_to_buffer( *queue_ );
      bytesInCache_ -= toDelete->num_bytes();
      data_map.erase(dat);
      delete toDelete;
    }
  }

  return true;
}

std::string
bstm_opencl_cache
::to_string()
{
  std::stringstream s;
  s << "MB in cache: " << (std::size_t) this->bytes_in_cache()/1024.0f/1024.0f<<'\n'
    << "bstm_opencl_cache::order: ";
  std::list<bstm_block_id>::iterator iter;
  for (iter=lru_order_.begin(); iter!=lru_order_.end(); ++iter)
  {
    bstm_block_id id = (*iter);
    s << id << ' ';
  }
  s << std::endl;
  return s.str();
}

// === Dummy (empty) instantiations for binary I/O
void vsl_b_write(vsl_b_ostream&  /*os*/, bstm_opencl_cache const&  /*scene*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, const bstm_opencl_cache* & /*p*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, bstm_opencl_cache_sptr&  /*sptr*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, bstm_opencl_cache_sptr const&  /*sptr*/) {}

void vsl_b_read(vsl_b_istream&  /*is*/, bstm_opencl_cache & /*scene*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, bstm_opencl_cache*  /*p*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, bstm_opencl_cache_sptr&  /*sptr*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, bstm_opencl_cache_sptr const&  /*sptr*/) {}
