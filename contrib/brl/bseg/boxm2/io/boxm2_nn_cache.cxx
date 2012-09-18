#include "boxm2_nn_cache.h"
#include <boxm2/boxm2_block_metadata.h>
#include <vcl_sstream.h>
//:
// \file

//: constructor, set the directory path
boxm2_nn_cache::boxm2_nn_cache(boxm2_scene* scene, BOXM2_IO_FS_TYPE fs)
: boxm2_cache(scene, fs)
{
  scene_dir_ = scene->data_path();
}

//: destructor flushes the memory for currently ongoing asynchronous requests
boxm2_nn_cache::~boxm2_nn_cache()
{
  this->finish_async_blocks();

#if 0
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

    // go through individual map and delete
    for (db_i = dmap.begin(); db_i != dmap.end(); ++db_i) {
      boxm2_data_base* dat = (*db_i).second;
      if (dat) {
        vcl_cout<<"Deleting data: "<<dat->block_id()<<vcl_endl;
        delete dat;
      }
    }
  }
#endif // 0
}

//: realization of abstract "get_block(block_id)"
boxm2_block* boxm2_nn_cache::get_block(boxm2_block_id id)
{
  // first thing to do is to load all async requests into the cache
  this->finish_async_blocks();

  // then look for the block you're requesting
  if ( cached_blocks_.find(id) != cached_blocks_.end() )
  {
    // congrats you've found the block in cache, now update cache and return block
#ifdef DEBUG
    vcl_cout<<"CACHE HIT :)"<<vcl_endl;
#endif
    this->update_block_cache(cached_blocks_[id]);
    return cached_blocks_[id];
  }
#ifdef DEBUG
  vcl_cout<<"Cache miss :("<<vcl_endl;
#endif
  // otherwise load it from disk with blocking and update cache
  boxm2_block* loaded = boxm2_sio_mgr::load_block(scene_dir_, id);

  // if the block is null then initialize an empty one
  if (!loaded && scene_->block_exists(id)) {
    vcl_cout<<"boxm2_nn_cache::initializing empty block "<<id<<vcl_endl;
    boxm2_block_metadata data = scene_->get_block_metadata(id);
    loaded = new boxm2_block(data);
  }

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
#ifdef DEBUG
  vcl_cout<<"update block cache around: "<<center<<vcl_endl;
#endif

  // find neighbors in x,y plane (i,j)
  vcl_vector<boxm2_block_id> neighbor_list = this->get_neighbor_list(center);

  // initialize new cache with existing neighbor ptrs
  vcl_map<boxm2_block_id, boxm2_block*> new_cache;

  // find neighbors in the cache already, store 'em
  for (unsigned int i=0; i<neighbor_list.size(); ++i)
  {
    boxm2_block_id id = neighbor_list[i];

    // if cached_blocks_ has this neighbor, add it to new cache (delete from old)
    if ( cached_blocks_.find(id) != cached_blocks_.end() )
    {
      new_cache[id] = cached_blocks_[id];
      cached_blocks_.erase(id);
    }
    else if ( !scene_->block_on_disk(id) ) // otherwise initialize it right here
    {
      vcl_cout<<"boxm2_nn_cache::initializing empty block "<<id<<vcl_endl;
      boxm2_block_metadata data = scene_->get_block_metadata(id);
      boxm2_block* loaded = new boxm2_block(data);
      new_cache[id] = loaded;
    }
    else { // send an async request for this block (if it's on disk)
      io_mgr_.load_block(scene_dir_, id);
    }
  }

  // only non-neighbors remain in existing cache, delete 'em
  vcl_map<boxm2_block_id, boxm2_block* >::iterator blk_i;
  for (blk_i = cached_blocks_.begin(); blk_i != cached_blocks_.end(); ++blk_i)
  {
    boxm2_block_id bid = blk_i->first;
    boxm2_block* d_blk = blk_i->second;
    if (bid != center && d_blk) {
#ifdef DEBUG
       vcl_cout<<"deleting "<<bid<<" from cache"<<vcl_endl;
#endif
       delete d_blk;
    }
  }
  cached_blocks_.clear();

  // store block passed in
  new_cache[blk->block_id()] = blk;

  // swap out cache
  cached_blocks_ = new_cache;
}


//: get data by type and id
boxm2_data_base* boxm2_nn_cache::get_data_base(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes, bool read_only)
{
  // first thing to do is to load all async requests into the cache
  this->finish_async_data(type);

  // grab a reference to the map of cached_data_
  vcl_map<boxm2_block_id, boxm2_data_base*>& data_map =
    this->cached_data_map(type);

  // then look for the block you're requesting
  if ( data_map.find(id) != data_map.end() )
  {
    // congrats you've found the data block in cache, update cache and return block
#ifdef DEBUG
    vcl_cout<<"DATA CACHE HIT :) for "<<type<<vcl_endl;
#endif
    this->update_data_base_cache(data_map[id], type);
    return data_map[id];
  }

  // otherwise it's a miss, load sync from disk, update cache
#ifdef DEBUG
  vcl_cout<<"Cache miss :( for "<<type<<vcl_endl;
#endif
  boxm2_data_base* loaded = boxm2_sio_mgr::load_block_data_generic(scene_dir_, id, type);
  if (!loaded && scene_->block_exists(id)) {
    vcl_cout<<"boxm2_nn_cache::initializing empty data "<<id<<" type: "<<type<<vcl_endl;
    boxm2_block_metadata data = scene_->get_block_metadata(id);
    loaded = new boxm2_data_base(data, type);
  }
  if (!read_only) this->update_data_base_cache(loaded, type);
  return loaded;
}

//: returns a data_base pointer which is initialized to the default value of the type.
//  If a block for this type exists on the cache, it is removed and replaced with the new one.
//  This method does not check whether a block of this type already exists on the disk nor writes it to the disk
boxm2_data_base* boxm2_nn_cache::get_data_base_new(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes, bool read_only)
{
  vcl_cout<<"BOXM2_DUMB_CACHE::get_data_base_new not implemented"<<vcl_endl;
  return 0;
}

void boxm2_nn_cache::remove_data_base(boxm2_block_id, vcl_string type)
{
  vcl_cout<<"BOXM2_DUMB_CACHE::remove_data_base not implemented"<<vcl_endl;
}

void boxm2_nn_cache::replace_data_base(boxm2_block_id id, vcl_string type, boxm2_data_base* replacement)
{
  vcl_cout<<"BOXM2_DUMB_CACHE::replace_data_base not implemented"<<vcl_endl;
}


//: update data cache by type
void boxm2_nn_cache::update_data_base_cache(boxm2_data_base* dat, vcl_string data_type)
{
  // grab a reference to the map of cached_data
  vcl_map<boxm2_block_id, boxm2_data_base*>& data_map =
    this->cached_data_map(data_type);

  // determine the center
  boxm2_block_id center = dat->block_id();
  // vcl_cout<<"update block cache around: "<<center<<vcl_endl;

  // find neighbors in x,y plane (i,j)
  vcl_vector<boxm2_block_id> neighbor_list = this->get_neighbor_list(center);

  // initialize new cache with existing neighbor ptrs
  vcl_map<boxm2_block_id, boxm2_data_base*> new_cache;

  // find neighbors in the cache already, store 'em
  for (unsigned int i=0; i<neighbor_list.size(); ++i)
  {
    boxm2_block_id id = neighbor_list[i];

    // if cached_blocks_ has this neighbor, add it to new cache (delete from old)
    if ( data_map.find(id) != data_map.end() )
    {
      new_cache[id] = data_map[id];
      data_map.erase(id);
    }
    else if ( !scene_->data_on_disk(id, data_type) ) // send an async request for this block
    {
      vcl_cout<<"boxm2_nn_cache::initializing empty data "<<id<<" type: "<<data_type<<vcl_endl;
      boxm2_block_metadata data = scene_->get_block_metadata(id);
      boxm2_data_base* loaded = new boxm2_data_base(data, data_type);
      new_cache[id] = loaded;
    }
    else
    {
      io_mgr_.load_block_data_generic(scene_dir_, id, data_type);
    }
  }

  // only non-neighbors remain in existing cache, delete 'em
  vcl_map<boxm2_block_id, boxm2_data_base*>::iterator blk_i;
  for (blk_i = data_map.begin(); blk_i != data_map.end(); ++blk_i)
  {
    boxm2_block_id   bid = blk_i->first;
    boxm2_data_base* blk = blk_i->second;
    if (bid != center && blk) {
#ifdef DEBUG
       vcl_cout<<"deleting "<<bid<<" from cache"<<vcl_endl;
#endif
       delete blk;
    }
  }
  data_map.clear();

  // store block passed in
  new_cache[dat->block_id()] = dat;

  // swap out cache
  data_map = new_cache;
}

void boxm2_nn_cache::finish_async_blocks()
{
  // get async block list and push it into the cache
  typedef vcl_map<boxm2_block_id, boxm2_block*> maptype;
  maptype lmap = io_mgr_.get_loaded_blocks();
  maptype::iterator iter;
  for (iter = lmap.begin(); iter != lmap.end(); ++iter)
  {
    // if this block doesn't exist in the cache put it in (otherwise delete it)
    if ( cached_blocks_.find(iter->first) == cached_blocks_.end() )
      cached_blocks_[iter->first] = iter->second;
    else
      if (iter->second) delete iter->second;
  }
}


//: finish async data
void boxm2_nn_cache::finish_async_data(vcl_string data_type)
{
  // grab a reference to the map of cached_data_
  vcl_map<boxm2_block_id, boxm2_data_base*>& data_map =
    this->cached_data_map(data_type);

  // get async block list and push it into the cache
  vcl_map<boxm2_block_id, boxm2_data_base*> lmap = io_mgr_.get_loaded_data_generic(data_type);
  vcl_map<boxm2_block_id, boxm2_data_base*>::iterator iter;
  for (iter = lmap.begin(); iter != lmap.end(); ++iter)
  {
    // if this block doesn't exist in the cache put it in (otherwise delete it)
    if ( data_map.find(iter->first) == data_map.end() ) {
      boxm2_block_id asid = iter->first;
      data_map[iter->first] = iter->second;
    }
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

  // grab a reference to the map of cached_data_ and return it
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
  // use scene here to determine if this id is valid
  return scene_->block_exists(id);
}


//: Summarizes this cache's data
vcl_string boxm2_nn_cache::to_string()
{
  vcl_stringstream stream;
  stream << "boxm2_nn_cache:: scene dir="<<scene_dir_<<'\n'
         << "  blocks: ";
  vcl_map<boxm2_block_id, boxm2_block*>::iterator blk_iter;
  for (blk_iter = cached_blocks_.begin(); blk_iter != cached_blocks_.end(); ++blk_iter) {
    boxm2_block_id id = blk_iter->first;
    stream << '(' << id /* << ',' << blk_iter->second */ << ")  ";
  }

  vcl_map<vcl_string, vcl_map<boxm2_block_id, boxm2_data_base*> >::iterator dat_iter;
  for (dat_iter = cached_data_.begin(); dat_iter != cached_data_.end(); ++dat_iter)
  {
    vcl_string data_type = dat_iter->first;
    stream<< '\n' << "  data: "<<data_type<<' ';
    vcl_map<boxm2_block_id, boxm2_data_base*> dmap = dat_iter->second;
    vcl_map<boxm2_block_id, boxm2_data_base*>::iterator it;
    for (it = dmap.begin(); it != dmap.end(); ++it)
    {
      boxm2_block_id id = it->first;
      stream<< '(' << id /*<< ',' <<it->second */<< ")  ";
    }
  }
  return stream.str();
}

//: shows elements in cache
vcl_ostream& operator<<(vcl_ostream &s, boxm2_nn_cache& scene)
{
  s << scene.to_string();
  return s;
}

