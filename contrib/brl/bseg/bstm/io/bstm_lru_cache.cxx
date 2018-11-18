#include <iostream>
#include <sstream>
#include "bstm_lru_cache.h"
//:
// \file
#include <bstm/bstm_block_metadata.h>
#include <bstm/io/bstm_sio_mgr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: PUBLIC create method, for creating singleton instance of bstm_cache
void bstm_lru_cache::create(const bstm_scene_sptr& scene)
{
  if (bstm_cache::exists())
  {
    if(bstm_cache::instance_->get_scene()->xml_path() == scene->xml_path())
    {
      std::cout << "bstm_lru_cache:: boxm2_cache singleton already created" << std::endl;
    }
    else
    {
      std::cout << "bstm_lru_cache:: destroying current cache and creating new one: " << scene->data_path() << std::endl;
      instance_ = new bstm_lru_cache(scene);
      destroyer_.set_singleton(instance_);
    }
  }
  else {
    instance_ = new bstm_lru_cache(scene);
    destroyer_.set_singleton(instance_);
  }
}

//: constructor, set the directory path
bstm_lru_cache::bstm_lru_cache(const bstm_scene_sptr& scene) : bstm_cache(scene)
{
  scene_dir_ = scene->data_path();
}

//: destructor flushes the memory
bstm_lru_cache::~bstm_lru_cache()
{
  // save the data and delete
  for (auto & iter : cached_data_)
  {
    for (auto it = iter.second.begin(); it != iter.second.end(); it++) {
      bstm_block_id id = it->first;
#if 0 // Currently causing some blocks to not save
      if (!it->second->read_only_) {
        bstm_sio_mgr::save_block_data_base(scene_dir_, it->first, it->second, iter->first);
      }
#endif
      // now throw it away
      delete it->second;
    }
    iter.second.clear();
  }

  for (auto & cached_block : cached_blocks_)
  {
    bstm_block_id id = cached_block.first;
#if 0 // Currently causing some blocks to not save
    if (!iter->second->read_only())
      bstm_sio_mgr::save_block(scene_dir_, iter->second);
#endif
    delete cached_block.second;
  }

  for (auto & cached_time_block : cached_time_blocks_)
  {
    bstm_block_id id = cached_time_block.first;
#if 0 // Currently causing some blocks to not save
    if (!iter->second->read_only())
      bstm_sio_mgr::save_time_block(scene_dir_, iter->second);
#endif
    delete cached_time_block.second;
  }
}

//: delete all the memory
//  Caution: make sure to call write to disk methods not to loose writable data
void bstm_lru_cache::clear_cache()
{
  // delete
  for (auto & iter : cached_data_)
  {
    for (auto it = iter.second.begin(); it != iter.second.end(); it++)
      delete it->second;
    iter.second.clear();
  }
  cached_data_.clear();

  for (auto & cached_block : cached_blocks_)
    delete cached_block.second;
  cached_blocks_.clear();

  for (auto & cached_time_block : cached_time_blocks_)
    delete cached_time_block.second;
  cached_time_blocks_.clear();
}

//: realization of abstract "get_block(block_id)"
bstm_block* bstm_lru_cache::get_block(bstm_block_id id)
{
  bstm_block_metadata data = scene_->get_block_metadata(id);

  // then look for the block you're requesting
  if ( cached_blocks_.find(id) != cached_blocks_.end() )
  {
    return cached_blocks_[id];
  }
  // otherwise load it from disk with blocking and update cache
  bstm_block* loaded = bstm_sio_mgr::load_block(scene_dir_, id, data);

  // if the block is null then initialize an empty one
  if (!loaded && scene_->block_exists(id)) {
    std::cout<<"bstm_lru_cache::initializing empty block "<<id<<std::endl;

    loaded = new bstm_block(data);
  }

  // update cache before returning the block
  cached_blocks_[id] = loaded;
  return loaded;
}

//: realization of abstract "get_time_block(block_id)"
bstm_time_block* bstm_lru_cache::get_time_block(bstm_block_id id)
{
  bstm_block_metadata data = scene_->get_block_metadata(id);

  // then look for the block you're requesting
  if ( cached_time_blocks_.find(id) != cached_time_blocks_.end() )
  {
    return cached_time_blocks_[id];
  }
  // otherwise load it from disk with blocking and update cache
  bstm_time_block* loaded = bstm_sio_mgr::load_time_block(scene_dir_, id, data);

  // if the block is null then initialize an empty one
  if (!loaded && scene_->block_exists(id)) {
    std::cout<<"bstm_lru_cache::initializing empty time block "<<id<<std::endl;

    loaded = new bstm_time_block(data);
  }

  // update cache before returning the block
  cached_time_blocks_[id] = loaded;
  return loaded;
}

//: get data by type and id
bstm_data_base* bstm_lru_cache::get_data_base(bstm_block_id id, std::string type, std::size_t num_bytes, bool read_only)
{
  // grab a reference to the map of cached_data_
  std::map<bstm_block_id, bstm_data_base*>& data_map = this->cached_data_map(type);

  // then look for the block you're requesting
  auto iter = data_map.find(id);
  if ( iter != data_map.end() )
  {
    if (!read_only)  // write-enable is enforced
      iter->second->enable_write();

    if(num_bytes == 0 || iter->second->buffer_length() == num_bytes ) // congrats you've found the data block in cache, update cache and return block
      return iter->second;
    else                                                              // congrats you've found the data block in cache, but it isn't the size you wanted, so delete it.
    {
      delete iter->second;
      data_map.erase(iter);
    }
  }

  // grab from disk
  bstm_data_base* loaded = bstm_sio_mgr::load_block_data_generic(scene_dir_, id, type);
  bstm_block_metadata data = scene_->get_block_metadata(id);

  // if num_bytes is greater than zero, then you're guaranteed to return a data size with that many bytes
  if (num_bytes > 0) {
    // if loaded from disk is good and it matches size, you found it, return
    if (loaded && loaded->buffer_length()==num_bytes) {
      // update data map
      data_map[id] = loaded;
      if (!read_only)  // write-enable is enforced
        loaded->enable_write();
      return loaded;
    }

    // requesting a specific number of bytes, and not found it on disk
    std::cout<<"bstm_lru_cache::initializing empty data "<<id
            <<" type: "<<type
            <<" to size: "<<num_bytes<<" bytes"<<std::endl;
    loaded = new bstm_data_base(new char[num_bytes], num_bytes, id, read_only);
    loaded->set_default_value(type, data);
  }
  else {
    // otherwise it's a miss, load sync from disk, update cache
    // loaded = bstm_sio_mgr::load_block_data_generic(scene_dir_, id, type);
    if (!loaded && scene_->block_exists(id)) {
      std::cout<<"bstm_lru_cache::initializing empty data "<<id<<" type: "<<type<<std::endl;
      loaded = new bstm_data_base(data, type, read_only);
    }
  }

  // update data map
  data_map[id] = loaded;
  return loaded;
}

//: returns a data_base pointer which is initialized to the default value of the type.
//  If a block for this type exists on the cache, it is removed and replaced with the new one.
//  This method does not check whether a block of this type already exists on the disk nor writes it to the disk
bstm_data_base* bstm_lru_cache::get_data_base_new(bstm_block_id id, std::string type, std::size_t num_bytes, bool read_only)
{
  bstm_data_base* block_data;
  if (num_bytes > 0)   {
    bstm_block_metadata data = scene_->get_block_metadata(id);
    // requesting a specific number of bytes,
    //std::cout<<"bstm_lru_cache::initializing empty data "<<id
    //        <<" type: "<<type
    //        <<" to size: "<<num_bytes<<" bytes"<<std::endl;
    std::cout<<id<<" init empty "<<type<<std::endl;
    block_data = new bstm_data_base(new char[num_bytes], num_bytes, id, read_only);
    block_data->set_default_value(type, data);
  }
  else {
    // initialize an empty block
    //std::cout<<"bstm_lru_cache::initializing empty data "<<id<<" type: "<<type<<std::endl;
    std::cout<<id<<" init empty "<<type<<std::endl;
    bstm_block_metadata data = scene_->get_block_metadata(id);
    // the following constructor also sets the default values
    block_data = new bstm_data_base(data, type, read_only);
  }

  // grab a reference to the map of cached_data_
  std::map<bstm_block_id, bstm_data_base*>& data_map = this->cached_data_map(type);

  // then look for the block you're requesting
  auto iter = data_map.find(id);
  if ( iter != data_map.end() )
  {
    // congrats you've found the data block in cache, now throw it away
    delete iter->second;
    data_map.erase(iter);
  }

  // now store the block in the cache
  data_map[id] = block_data;
  return block_data;
}

//: removes data from this cache (may or may not write to disk first)
void bstm_lru_cache::remove_data_base(bstm_block_id id, std::string type)
{
  // grab a reference to the map of cached_data_
  std::map<bstm_block_id, bstm_data_base*>& data_map =
    this->cached_data_map(type);
  // then look for the block you're requesting
  auto rem = data_map.find(id);
  if ( rem != data_map.end() )
  {
    // found the block,
    bstm_data_base* litter = data_map[id];

    if (!litter->read_only_) {
      // save it
      std::cout<<"bstm_lru_cache::remove_data_base "<<type<<':'<<id<<"; saving to disk"<<std::endl;
      bstm_sio_mgr::save_block_data_base(scene_dir_, id, litter, type);
    }
    else
      std::cout<<"bstm_lru_cache::remove_data_base "<<type<<':'<<id<<"; not saving to disk"<<std::endl;
    // now throw it away
    delete litter;
    data_map.erase(rem);
  }
}

//: replaces data in the cache with one here
void bstm_lru_cache::replace_data_base(bstm_block_id id, std::string type, bstm_data_base* replacement)
{
  // grab a reference to the map of cached_data_
  std::map<bstm_block_id, bstm_data_base*>& data_map = this->cached_data_map(type);

  // find old data base and copy it's read_only/write status
  auto rem = data_map.find(id);
  if ( rem != data_map.end() )
  {
    // found the block,
    bstm_data_base* litter = data_map[id];
    replacement->read_only_ = litter->read_only_;
    // now throw it away
    delete litter;
    data_map.erase(rem);
  }
  else
    std::cout << "Couldn't find " << type << " buffer in cache..." << std::endl;

  // this->remove_data_base(id, type);
  // put the new one in there
  data_map[id] = replacement;
}

void bstm_lru_cache::replace_time_block(bstm_block_id id, bstm_time_block* replacement)
{
  bstm_block_metadata data = scene_->get_block_metadata(id);

  // look for the block you're requesting
  auto rem = cached_time_blocks_.find(id);
  if (rem != cached_time_blocks_.end() )
  {
    //found time block, now delete it
    bstm_time_block* litter = cached_time_blocks_[id];
    delete litter;
    cached_time_blocks_.erase(rem);
  }
  else
     std::cerr << "Time block with id " << id << " not found!" << std::endl;

  //save the new time block
  cached_time_blocks_[id] = replacement;
}

//: helper method returns a reference to correct data map (ensures one exists)
std::map<bstm_block_id, bstm_data_base*>& bstm_lru_cache::cached_data_map(const std::string& prefix)
{
  // if map for this particular data type doesn't exist, initialize it
  if ( cached_data_.find(prefix) == cached_data_.end() )
  {
    std::map<bstm_block_id, bstm_data_base*> dmap;
    cached_data_[prefix] = dmap;
  }

  // grab a reference to the map of cached_data_ and return it
  std::map<bstm_block_id, bstm_data_base*>& data_map = cached_data_[prefix];
  return data_map;
}

//: helper method says whether or not block id is valid
bool bstm_lru_cache::is_valid_id(const bstm_block_id& id)
{
  // use scene here to determine if this id is valid
  return scene_->block_exists(id);
}


//: Summarizes this cache's data
std::string bstm_lru_cache::to_string()
{
  std::stringstream stream;
  stream << "bstm_lru_cache:: scene dir="<<scene_dir_<<'\n'
         << "  blocks: ";
  std::map<bstm_block_id, bstm_block*>::iterator blk_iter;
  for (blk_iter = cached_blocks_.begin(); blk_iter != cached_blocks_.end(); ++blk_iter) {
    bstm_block_id id = blk_iter->first;
    stream << '(' << id /* << ',' << blk_iter->second */ << ")  ";
  }

  stream << "\n time blocks: ";
  std::map<bstm_block_id, bstm_time_block*>::iterator time_blk_iter;
  for (time_blk_iter = cached_time_blocks_.begin(); time_blk_iter != cached_time_blocks_.end(); ++time_blk_iter) {
    bstm_block_id id = time_blk_iter->first;
    stream << '(' << id /* << ',' << blk_iter->second */ << ")  ";
  }


  std::map<std::string, std::map<bstm_block_id, bstm_data_base*> >::iterator dat_iter;
  for (dat_iter = cached_data_.begin(); dat_iter != cached_data_.end(); ++dat_iter)
  {
    std::string data_type = dat_iter->first;
    stream<< "\n  data: "<<data_type<<' ';
    std::map<bstm_block_id, bstm_data_base*> dmap = dat_iter->second;
    std::map<bstm_block_id, bstm_data_base*>::iterator it;
    for (it = dmap.begin(); it != dmap.end(); ++it)
    {
      bstm_block_id id = it->first;
      stream<< '(' << id /*<< ',' <<it->second */<< ")  ";
    }
  }
  stream << std::endl;
  return stream.str();
}

//: dumps all data onto disk
void bstm_lru_cache::write_to_disk()
{
   // save the data and delete
  for (auto & iter : cached_data_)
  {
    for (auto it = iter.second.begin(); it != iter.second.end(); it++) {
      bstm_block_id id = it->first;
      // if (!it->second->read_only_)
        bstm_sio_mgr::save_block_data_base(scene_dir_, it->first, it->second, iter.first);
    }
  }

  for (auto & cached_block : cached_blocks_)
  {
    bstm_block_id id = cached_block.first;
    // if (!iter->second->read_only())
    bstm_sio_mgr::save_block(scene_dir_, cached_block.second);
  }

  for (auto & cached_time_block : cached_time_blocks_)
  {
    bstm_block_id id = cached_time_block.first;
    // if (!iter->second->read_only())
    bstm_sio_mgr::save_time_block(scene_dir_, cached_time_block.second);
  }
}

//: shows elements in cache
std::ostream& operator<<(std::ostream &s, bstm_lru_cache& scene)
{
  return s << scene.to_string();
}
