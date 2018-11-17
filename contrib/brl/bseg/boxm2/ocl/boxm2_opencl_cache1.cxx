#include <utility>

#include "boxm2_opencl_cache1.h"
//:
// \file

//: scene/device constructor
boxm2_opencl_cache1::boxm2_opencl_cache1(const boxm2_scene_sptr& scene,
                                       const bocl_device_sptr& device,
                                       unsigned int maxBlocks)
: scene_(scene), maxBlocksInCache(maxBlocks), bytesInCache_(0), block_info_(nullptr), device_(device)
{
  // store max bytes allowed in cache - use only 80 percent of the memory
  maxBytesInCache_ = (unsigned long) (device->info().total_global_memory_ * 0.7);

  // by default try to create an LRU cache
  boxm2_lru_cache1::create(scene);
  cpu_cache_ = boxm2_cache1::instance();

  // store the device pointer and context associated with the device
  context_ = &device->context();

  // create command queue... make sure this isn't destructed at the end of the function...
  int status = 0;
  q_ = clCreateCommandQueue(*context_,
                            *device->device_id(),
                            CL_QUEUE_PROFILING_ENABLE,
                            &status);
  if (!check_val(status,CL_SUCCESS,"boxm2_opencl_cache1:: failed in command queue creation " + error_to_string(status)))
    return;
  queue_ = &q_;
}

//: sets a context in case the default context off the device is old
// (this  is used for the tableau's shared cl_gl context)
void boxm2_opencl_cache1::set_context(cl_context& context)
{
  context_ = &context;
  int status = clReleaseCommandQueue(q_);
  if (!check_val(status,CL_SUCCESS,"boxm2_opencl_cache1::set_context failed to release old queue " + error_to_string(status)))
    return;

  q_ = clCreateCommandQueue(*context_,
                            *device_->device_id(),
                            CL_QUEUE_PROFILING_ENABLE,
                            &status);
  if (!check_val(status,CL_SUCCESS,"boxm2_opencl_cache1::set_context failed in command queue creation " + error_to_string(status)))
    return;
  queue_ = &q_;
}

bool boxm2_opencl_cache1::clear_cache()
{
  // delete stored block info
  if (block_info_) {
    auto* buff = (boxm2_scene_info*) block_info_->cpu_buffer();
    delete buff;
    delete block_info_;
    block_info_=nullptr;
  }

  // delete blocks in cache
  std::map<boxm2_block_id, bocl_mem*>::iterator blks;
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
  std::map<std::string, std::map<boxm2_block_id, bocl_mem*> >::iterator datas;
  for (datas=cached_data_.begin(); datas!=cached_data_.end(); ++datas)
  {
    std::map<boxm2_block_id, bocl_mem*>& data_map = datas->second;
    std::map<boxm2_block_id, bocl_mem*>::iterator data_blks;
    for (data_blks=data_map.begin(); data_blks!=data_map.end(); ++data_blks)
    {
      bocl_mem* toDelete = data_blks->second;
      //toDelete->read_to_buffer( *queue_ );
      bytesInCache_ -= toDelete->num_bytes();
      boxm2_block_id bid = data_blks->first;
#ifdef DEBUG
      std::cout<<"Deleting data type: "<<datas->first<<" id "<<bid<<"..size: "<<toDelete->num_bytes()<<std::endl;
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

//: calls clFinish so halts cpp code
bool boxm2_opencl_cache1::finish_queue() {
  cl_int status = clFinish(*queue_);
  check_val(status, MEM_FAILURE, "release memory FAILED: " + error_to_string(status));
  return true;
}

void boxm2_opencl_cache1::shallow_remove_block(const boxm2_block_id& id)
{
  // delete blocks in cache
  auto iter = cached_blocks_.find(id);
  if (iter != cached_blocks_.end()) {
    bocl_mem* toDelete = iter->second;
    bytesInCache_ -= toDelete->num_bytes();
    delete toDelete;
    cached_blocks_.erase(iter);
  }
}

//: check if max_bytes_in_cache is hit and call clear_cache() if necessary
bool boxm2_opencl_cache1::clear_cache_if_necessary()
{
  if (bytesInCache_ >= maxBytesInCache_)
    return clear_cache();
  else return true;
}

std::size_t boxm2_opencl_cache1::bytes_in_cache()
{
  // count up bytes in cache (like clearing, but no deleting...
  std::size_t count = 0;

  std::map<boxm2_block_id, bocl_mem*>::iterator blks;
  for (blks=cached_blocks_.begin(); blks!=cached_blocks_.end(); ++blks)
  {
    bocl_mem* curr = blks->second;
    count += curr->num_bytes();
  }

  // count boxm2_data mem sizes
  std::map<std::string, std::map<boxm2_block_id, bocl_mem*> >::iterator datas;
  for (datas=cached_data_.begin(); datas!=cached_data_.end(); ++datas)
  {
    std::map<boxm2_block_id, bocl_mem*>& data_map = datas->second;
    std::map<boxm2_block_id, bocl_mem*>::iterator data_blks;
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

//: realization of abstract "get_block(block_id)"
bocl_mem* boxm2_opencl_cache1::get_block(const boxm2_block_id& id)
{
  //requesting block pushes it to the front of the list
  this->lru_push_front(id);

  // then look for the block you're requesting
  if ( cached_blocks_.find(id) != cached_blocks_.end() ) {
    // load block info
    boxm2_block* loaded = cpu_cache_->get_block(id);
    if (block_info_) {
       auto* buff = (boxm2_scene_info*) block_info_->cpu_buffer();
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
  const boxm2_array_3d<uchar16>& trees = loaded->trees();
  std::size_t toLoadSize = trees.size()*sizeof(uchar16);
  unsigned long totalBytes = this->bytes_in_cache() + toLoadSize;
  if (totalBytes > maxBytesInCache_) {

#ifdef DEBUG
    std::cout<<"Loading Block "<<id<<" uses "<<totalBytes<<" out of  "<<maxBytesInCache_<<std::endl
            <<"    removing... ";
#endif
    while ( this->bytes_in_cache()+toLoadSize > maxBytesInCache_ && !cached_blocks_.empty() )
    {
      boxm2_block_id lru_id;
      if (!this->lru_remove_last(lru_id)) {
         std::cerr << "ERROR: boxm2_opencl_cache1::get_block(): lru is empty" << std::endl;
         return (bocl_mem*)nullptr;
      }
#ifdef DEBUG
      std::cout<<lru_id<<" ... ";
#endif
      if (lru_id == id)
        std::cout<<"boxm2_opencl_cache1:: Single Block Size is too big for GPU RAM"<<std::endl;
    }
#ifdef DEBUG
    std::cout<<std::endl;
#endif
  }
 auto* data_block = const_cast<uchar16*>(trees.data_block());
  // otherwise load it from disk with blocking
  bocl_mem* blk = new bocl_mem(*context_, data_block, toLoadSize, "3d trees buffer " + id.to_string() );
  blk->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  bytesInCache_ += blk->num_bytes();

  // store the requested block in the cache
  cached_blocks_[id] = blk;

  //////////////////////////////////////////////////////
  // load block info
  if (block_info_) {
    auto* buff = (boxm2_scene_info*) block_info_->cpu_buffer();
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

bocl_mem* boxm2_opencl_cache1::get_block_info(const boxm2_block_id& id)
{
  // clean up
  if (block_info_) {
    auto* buff = (boxm2_scene_info*) block_info_->cpu_buffer();
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
bocl_mem* boxm2_opencl_cache1::get_data(const boxm2_block_id& id, const std::string& type, std::size_t num_bytes, bool read_only)
{
  //push id to front of LRU list
  this->lru_push_front(id);

  // grab a reference to the map of cached_data_
  std::map<boxm2_block_id, bocl_mem*>& data_map =
    this->cached_data_map(type);

  // then look for the block you're requesting
  auto iter = data_map.find(id);
  if ( iter != data_map.end() ) {
    return iter->second;
  }

  // load data into CPU cache and check size to see if GPU cache needs cleaning
  boxm2_data_base* data_base = cpu_cache_->get_data_base(id,type,num_bytes,read_only);
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
    while ( this->bytes_in_cache()+toLoadSize > maxBytesInCache_ && !data_map.empty() )
    {
      boxm2_block_id lru_id;
      if(!this->lru_remove_last(lru_id)) {
         std::cerr << "ERROR: boxm2_opencl_cache1::get_data() : lru is empty" << std::endl;
         return (bocl_mem*)nullptr;
      }
#ifdef DEBUG
      std::cout<<lru_id<<" ... ";
#endif
      if (lru_id == id)
        std::cout<<"boxm2_opencl_cache1:: Single Block Size is too big for GPU RAM"<<std::endl;
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
bocl_mem* boxm2_opencl_cache1::get_data_new(const boxm2_block_id& id, const std::string& type, std::size_t num_bytes, bool read_only)
{
  //push id to front of LRU list
  this->lru_push_front(id);

  // grab a reference to the map of cached_data_
  std::map<boxm2_block_id, bocl_mem*>& data_map = this->cached_data_map(type);

  // then look for the block you're requesting, if found, delete it.
  auto iter = data_map.find(id);
  if ( iter != data_map.end() ) {
    delete iter->second;
    data_map.erase(iter);
  }

  // load new data into CPU cache and check size to see if GPU cache needs cleaning
  boxm2_data_base* data_base = cpu_cache_->get_data_base_new(id,type,num_bytes,read_only);
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
      boxm2_block_id lru_id;
      if (!this->lru_remove_last(lru_id)) {
         std::cerr << "ERROR: boxm2_opencl_cache1::get_data_new() : lru is empty " << std::endl;
         return (bocl_mem*)nullptr;
      }
#ifdef DEBUG
      std::cout<<lru_id<<" ... ";
#endif
      if (lru_id == id)
        std::cout<<"boxm2_opencl_cache1:: Single Block Size is too big for GPU RAM"<<std::endl;
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
bocl_mem* boxm2_opencl_cache1::alloc_mem(std::size_t num_bytes, void* cpu_buff, std::string id)
{
  std::size_t totalBytes = this->bytes_in_cache()+num_bytes;
  if (totalBytes > maxBytesInCache_) {
#ifdef DEBUG
    std::cout<<"OCL cache alloc mem "<<" uses "<<totalBytes<<" out of  "<<maxBytesInCache_<<std::endl
            <<"    removing... ";
#endif
    while ( this->bytes_in_cache()+num_bytes > maxBytesInCache_ )
    {
      boxm2_block_id lru_id;
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

void boxm2_opencl_cache1::free_mem_pool()
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
void boxm2_opencl_cache1::unref_mem(bocl_mem* mem)
{
  auto iter = mem_pool_.find(mem);
  if (iter != mem_pool_.end()){
    mem_pool_.erase(iter);
  }
}

void boxm2_opencl_cache1::free_mem(bocl_mem* mem)
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
void boxm2_opencl_cache1::deep_replace_data(const boxm2_block_id& id, const std::string& type, bocl_mem* mem, bool read_only)
{
  // instantiate new data block
  std::size_t numDataBytes = mem->num_bytes();

  //TODO: figure out consistent scheme to make this read_only or read_write
  boxm2_data_base* newData = new boxm2_data_base(new char[numDataBytes], numDataBytes, id, read_only);

  // write bocl_mem data to cpu buffer
  mem->set_cpu_buffer((void*) newData->data_buffer());
  mem->read_to_buffer( *queue_ );

  // do deep replace
  cpu_cache_->replace_data_base(id, type, newData);

  // now replace the mem in the GPU cache.. first delete existing
  std::map<boxm2_block_id, bocl_mem*>& data_map = this->cached_data_map(type);
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
void boxm2_opencl_cache1::deep_remove_data(const boxm2_block_id& id, const std::string& type, bool  /*write_out*/)
{
  //find the data in this map
  std::map<boxm2_block_id, bocl_mem*>& data_map = this->cached_data_map(type);
  auto iter = data_map.find(id);
  if ( iter != data_map.end() ) {
    // release existing memory

    bocl_mem* toDelete = iter->second;
     this->unref_mem(toDelete);
    delete toDelete;
    data_map.erase(iter);
  }

  //remove from cpu_cache_
  cpu_cache_->remove_data_base(id, type);
#if 0
  remove from lru_order
  std::list<boxm2_block_id>::iterator loc = std::find(lru_order_.begin(), lru_order_.end(), id);
  if (loc != lru_order_.end())
    lru_order_.erase(loc);
#endif
}

//: shallow_remove_data removes data with id and type from ocl cache only
void boxm2_opencl_cache1::shallow_remove_data(const boxm2_block_id& id, std::string type)
{
  //find the data in this map
  std::map<boxm2_block_id, bocl_mem*>& data_map = this->cached_data_map(std::move(type));
  auto iter = data_map.find(id);
  if ( iter != data_map.end() ) {
    // release existing memory
    bocl_mem* toDelete = iter->second;
    bytesInCache_ -= toDelete->num_bytes();
    delete toDelete;
    data_map.erase(iter);
  }
}

//: helper method, \returns a reference to correct data map (ensures one exists)
std::map<boxm2_block_id, bocl_mem*>& boxm2_opencl_cache1::cached_data_map(const std::string& prefix)
{
  // if map for this particular data type doesn't exist, initialize it
  if ( cached_data_.find(prefix) == cached_data_.end() )
  {
    std::map<boxm2_block_id, bocl_mem*> dmap;
    cached_data_[prefix] = dmap;
  }

  // grab a reference to the map of cached_data_ and return it
  std::map<boxm2_block_id, bocl_mem*>& data_map = cached_data_[prefix];
  return data_map;
}

//: helper method to insert into LRU list
void boxm2_opencl_cache1::lru_push_front( const boxm2_block_id& id )
{
  //serach for it in the list, if it's there, delete it
  std::list<boxm2_block_id>::iterator iter;
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
bool boxm2_opencl_cache1::lru_remove_last(boxm2_block_id &lru_id)
{
  //grab and remove last element
  if (lru_order_.empty()) {
     std::cerr << "ERROR: boxm2_opencl_cache1::lru_remove_last() : LRU is empty " << std::endl;
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
    std::cout<<"boxm2_opencl_cache1::lru_remove_last failed to find last element of list"<<std::endl;
  }

  //now look for data to delete
  std::map<std::string, std::map<boxm2_block_id, bocl_mem*> >::iterator datas;
  for (datas=cached_data_.begin(); datas!=cached_data_.end(); ++datas)
  {
    std::string data_type = datas->first;
    std::map<boxm2_block_id, bocl_mem*>& data_map = datas->second;
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

std::string boxm2_opencl_cache1::to_string()
{
  std::stringstream s;
  s << "MB in cache: " << (std::size_t) this->bytes_in_cache()/1024.0f/1024.0f<<'\n'
    << "boxm2_opencl_cache1::order: ";
  std::list<boxm2_block_id>::iterator iter;
  for (iter=lru_order_.begin(); iter!=lru_order_.end(); ++iter)
  {
    boxm2_block_id id = (*iter);
    s << id << ' ';
  }
  s << std::endl;
  return s.str();
}

// === Dummy (empty) instantiations for binary I/O
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_opencl_cache1 const&  /*scene*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, const boxm2_opencl_cache1* & /*p*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_opencl_cache1_sptr&  /*sptr*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_opencl_cache1_sptr const&  /*sptr*/) {}

void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_opencl_cache1 & /*scene*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_opencl_cache1*  /*p*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_opencl_cache1_sptr&  /*sptr*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_opencl_cache1_sptr const&  /*sptr*/) {}
