#include "boxm2_nn_cache.h"
//:
// \file

//: destructor flushes the memory for currently ongoing asynchronous requests
boxm2_nn_cache::~boxm2_nn_cache()
{
  this->finish_async_blocks();

  // clean up block
  vcl_map<boxm2_block_id, boxm2_block* >::iterator blk_i;
  for (blk_i=cached_blocks_.begin(); blk_i!=cached_blocks_.end(); ++blk_i) {
    boxm2_block* blk = (*blk_i).second;
    if (blk) {
        vcl_cout<<"Deleting block: "<<blk->block_id()<<vcl_endl;
      delete blk;
    }
  }

  // clean up loaded data
  vcl_map<vcl_string, vcl_map<boxm2_block_id, boxm2_data_base*> >::iterator dat_i;
  for (dat_i=cached_data_.begin(); dat_i!=cached_data_.end(); ++dat_i) {
    vcl_map<boxm2_block_id, boxm2_data_base*>& dmap = (*dat_i).second;
    vcl_map<boxm2_block_id, boxm2_data_base*>::iterator db_i;

    //go through individual map and delete
    for (db_i = dmap.begin(); db_i != dmap.end(); ++db_i) {
      boxm2_data_base* dat = (*db_i).second;
      if (dat) {
         vcl_cout<<"Deleting data: "<<dat->block_id()<<vcl_endl;
         delete dat;
      }
    }
  }
}

//: realization of abstract "get_block(block_id)"
boxm2_block* boxm2_nn_cache::get_block(boxm2_block_id id)
{
  //first thing to do is to load all async requests into the cache
  this->finish_async_blocks();

  //then look for the block you're requesting
  if ( cached_blocks_.find(id) != cached_blocks_.end() )
  {
    //congrats you've found the block in cache, now update cache and return block
    vcl_cout<<"CACHE HIT!"<<vcl_endl;
    this->update_block_cache(cached_blocks_[id]);
    return cached_blocks_[id];
  }

  vcl_cout<<"Cache miss :("<<vcl_endl;
  //other wise load it from disk with blocking and update cache
  boxm2_block* loaded = boxm2_sio_mgr::load_block(scene_dir_, id);
  this->update_block_cache(loaded);
  return loaded;
}

//: Update block cache given an already loaded, valid block pointer
// Idea is to determine its neighbors (in x,y plane)
// Put already loaded neighbors into new_cache (delete entry in old cache)
// Load missing neighbors asynchronously
// Delete non-neighbors that exist in the cache (don't delete the center block)
// Swap out the caches
void boxm2_nn_cache::update_block_cache(boxm2_block* blk)
{
  boxm2_block_id center = blk->block_id();
  vcl_cout<<"update block cache around: "<<center<<vcl_endl;

  //find neighbors in x,y plane (i,j)
  vcl_vector<boxm2_block_id> neighbor_list = this->get_neighbor_list(center); 

  // initialize new cache with existing neighbor ptrs
  vcl_map<boxm2_block_id, boxm2_block*> new_cache;

  //find neighbors in the cache already, store em
  for (unsigned int i=0; i<neighbor_list.size(); ++i)
  {
    boxm2_block_id id = neighbor_list[i];

    //if cached_blocks_ has this neighbor, add it to new cache (delete from old)
    if ( cached_blocks_.find(id) != cached_blocks_.end() )
    {
      new_cache[id] = cached_blocks_[id];
      cached_blocks_.erase(id);
    }
    else //send an async request for this block
    {
      io_mgr_.load_block(scene_dir_, id);
    }
  }

  //only non-neighbors remain in existing cache, delete em
  vcl_map<boxm2_block_id, boxm2_block* >::iterator blk_i;
  for (blk_i = cached_blocks_.begin(); blk_i != cached_blocks_.end(); ++blk_i)
  {
    boxm2_block_id bid = blk_i->first;
    boxm2_block* d_blk = blk_i->second;
    if (bid != center && d_blk) {
       vcl_cout<<"deleting "<<bid<<" from cache"<<vcl_endl;
       delete d_blk;
    }
  }
  cached_blocks_.clear();

  //store block passed in
  new_cache[blk->block_id()] = blk;

  //swap out cache
  cached_blocks_ = new_cache;
}

void boxm2_nn_cache::finish_async_blocks()
{
  // get async block list and push it into the cache
  typedef vcl_map<boxm2_block_id, boxm2_block*> maptype;
  maptype lmap = io_mgr_.get_loaded_blocks();
  maptype::iterator iter;
  for (iter = lmap.begin(); iter != lmap.end(); ++iter)
  {
    //if this block doesn't exist in the cache put it in (otherwise delete it)
    if ( cached_blocks_.find(iter->first) == cached_blocks_.end() )
      cached_blocks_[iter->first] = iter->second;
    else
      if (iter->second) delete iter->second;
  }
}


//: helper method returns a reference to correct data map (ensures one exists)
vcl_map<boxm2_block_id, boxm2_data_base*>& boxm2_nn_cache::cached_data_map(vcl_string prefix)
{
  // if map for this particular data type doesn't exist, initialize it
  if ( cached_data_.find(prefix) == cached_data_.end() )
  {
    vcl_map<boxm2_block_id, boxm2_data_base*> dmap;
    cached_data_[prefix] = dmap;
  }  
  
  //grab a reference to the map of cached_data_ and return it
  vcl_map<boxm2_block_id, boxm2_data_base*>& data_map = cached_data_[prefix]; 
  return data_map; 
}

//: returns a list of neighbors to center
vcl_vector<boxm2_block_id> boxm2_nn_cache::get_neighbor_list(boxm2_block_id center)
{
  vcl_vector<boxm2_block_id> neighbor_list;  
  for (int i=-1; i<=1; ++i) {
    for (int j=-1; j<=1; ++j) {
      boxm2_block_id id(center.i() + i, center.j() + j, center.k());
      if (this->is_valid_id(id) && id!=center) {
        neighbor_list.push_back(id);
      }
    }
  }
  return neighbor_list;
}

//: helper method says whether or not block id is valid
bool boxm2_nn_cache::is_valid_id(boxm2_block_id id)
{
  return  id.i() >= 0 &&
          id.j() >= 0 &&
          id.k() >= 0 &&
          id.i() < block_num_.x() &&
          id.j() < block_num_.y() &&
          id.k() < block_num_.z();
}

