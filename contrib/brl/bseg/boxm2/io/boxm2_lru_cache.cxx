#include <sstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include "boxm2_lru_cache.h"
//:
// \file
#include <boxm2/boxm2_block_metadata.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_data_traits.h>
//: PUBLIC create method, for creating singleton instance of boxm2_cache
void boxm2_lru_cache::create(const boxm2_scene_sptr& scene, BOXM2_IO_FS_TYPE fs_type)
{
  if (boxm2_cache::exists()){
  }else{
    instance_ = new boxm2_lru_cache(scene, fs_type);
  }
}

//: constructor, set the directory path
boxm2_lru_cache::boxm2_lru_cache(const boxm2_scene_sptr& scene, BOXM2_IO_FS_TYPE fs_type) : boxm2_cache(fs_type)
{
  cached_blocks_[scene] = std::map<boxm2_block_id, boxm2_block*>();
  cached_data_[scene] = std::map<std::string, std::map<boxm2_block_id, boxm2_data_base*> >();
}


//: return list of scenes with data in the cache
std::vector<boxm2_scene_sptr> boxm2_lru_cache::get_scenes()
{
  std::set<boxm2_scene_sptr > scenes;
  for (std::map< boxm2_scene_sptr, std::map<boxm2_block_id, boxm2_block*>,ltstr1 >::const_iterator it=cached_blocks_.begin();
       it != cached_blocks_.end(); ++it) {
    scenes.insert(it->first);
  }
  // in case the cache has data, but not the block
  for (std::map< boxm2_scene_sptr, std::map<std::string, std::map<boxm2_block_id, boxm2_data_base*> >,ltstr1 >::const_iterator it=cached_data_.begin();
       it != cached_data_.end(); ++it) {
    scenes.insert(it->first);
  }
  return std::vector<boxm2_scene_sptr>(scenes.begin(), scenes.end());
}



//: destructor flushes the memory for currently ongoing asynchronous requests
boxm2_lru_cache::~boxm2_lru_cache()
{
    this->clear_cache();
}

//: delete all the memory
//  Caution: make sure to call write to disk methods not to loose writable data
void boxm2_lru_cache::clear_cache()
{
  auto scene_iter = cached_data_.begin();
  for(;scene_iter!=cached_data_.end(); scene_iter++)
  {
      std::map<std::string, std::map<boxm2_block_id, boxm2_data_base*> > &dmap = scene_iter->second;
      for (auto & iter : dmap)
      {
          for (auto it = iter.second.begin(); it != iter.second.end(); it++)
              delete it->second;

          iter.second.clear();
      }
      scene_iter->second.clear();
  }
  cached_data_.clear();
  auto scene_block_iter =cached_blocks_.begin();
  for(;scene_block_iter!=cached_blocks_.end(); scene_block_iter++)
  {

      for (auto & iter : scene_block_iter->second)
      {
          delete iter.second;
      }
      scene_block_iter->second.clear();
  }
  cached_blocks_.clear();


}

//: realization of abstract "get_block(block_id)"
boxm2_block* boxm2_lru_cache::get_block(boxm2_scene_sptr & scene, boxm2_block_id id)
{
  boxm2_block_metadata mdata = scene->get_block_metadata(id);

  //: add a scene
  if(cached_blocks_.find(scene) == cached_blocks_.end() ) {
    this->add_scene(scene);
  }
#if 0
    if(!this->add_scene(scene))
        return NULL;
#endif

  //: add a block
  if ( cached_blocks_[scene].find(id) == cached_blocks_[scene].end() )
  {
      boxm2_block* loaded = boxm2_sio_mgr::load_block(scene->data_path(), id, mdata );

      // if the block is null then initialize an empty one
      if (!loaded && scene->block_exists(id)) {
          std::cout<<"boxm2_lru_cache::initializing empty block "<<id<<std::endl;
          loaded = new boxm2_block(mdata);
      }
      // update cache before returning the block
      cached_blocks_[scene][id] = loaded;
  }
  return cached_blocks_[scene][id];
}

//: get data by type and id
boxm2_data_base* boxm2_lru_cache::get_data_base(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type, std::size_t num_bytes, bool read_only)
{
  // grab a reference to the map of cached_data_
  std::map<boxm2_block_id, boxm2_data_base*>& data_map =this->cached_data_map(scene, type);
  std::size_t data_size  = boxm2_data_info::datasize(type);
  if (!scene->block_exists(id)){
    return nullptr;
  }
  boxm2_block* blk = this->get_block(scene,id);
  unsigned n_cells = blk->num_cells();
  std::size_t byte_length = n_cells * data_size; // override the num_bytes passed parameter
  // then look for the block you're requesting
  auto iter = data_map.find(id);
  if ( iter != data_map.end() )
  {
    // congrats you've found the data block in cache, update cache and return block
    if (!read_only)  // write-enable is enforced
      iter->second->enable_write();
    return iter->second;
  }

  // grab from disk
  boxm2_data_base* loaded = boxm2_sio_mgr::load_block_data_generic(scene->data_path(), id, type, filesystem_);
  boxm2_block_metadata data = scene->get_block_metadata(id);

  // if num_bytes is greater than zero, then you're guaranteed to return a data size with that many bytes
  if (num_bytes > 0) {
    if (num_bytes != byte_length){
      std::stringstream ss;
      ss<<"Attempting to retrieve "<<num_bytes<<" bytes for datatype " << type <<" when actual buffer size should be "<<byte_length;
      throw std::runtime_error(ss.str());
      loaded = nullptr;
      return loaded;
    }
    // if loaded from disk is good and it matches size, you found it, return
    if (loaded && loaded->buffer_length() == byte_length) {
      // update data map
      data_map[id] = loaded;
      if (!read_only)  // write-enable is enforced
        loaded->enable_write();
      return loaded;
    }

    // requesting a specific number of bytes, and not found it on disk
    std::cout<<"boxm2_lru_cache::initializing empty data "<<id
            <<" type: "<<type
            <<" to size: "<< byte_length <<" bytes"<<std::endl;
    loaded = new boxm2_data_base(new char[byte_length], byte_length, id, read_only);
    loaded->set_default_value(type, data);
  }
  else {
    // otherwise it's a miss, load sync from disk, update cache
    if (!loaded && scene->block_exists(id)) {
      std::cout<<"boxm2_lru_cache::initializing empty data "<<id<<" type: "<<type<<std::endl;
      loaded = new boxm2_data_base(new char[byte_length], byte_length, id, read_only);
      loaded->set_default_value(type, data);
    }
  }

  // update data map
  data_map[id] = loaded;
  return loaded;


}

//: returns a data_base pointer which is initialized to the default value of the type.
//  If a block for this type exists on the cache, it is removed and replaced with the new one.
//  This method does not check whether a block of this type already exists on the disk nor writes it to the disk
boxm2_data_base* boxm2_lru_cache::get_data_base_new(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type, std::size_t num_bytes, bool read_only)
{
  boxm2_data_base* block_data;
  if (num_bytes > 0)   {
    boxm2_block_metadata data = scene->get_block_metadata(id);
    // requesting a specific number of bytes,
    //std::cout<<"boxm2_lru_cache::initializing empty data "<<id
    //        <<" type: "<<type
    //        <<" to size: "<<num_bytes<<" bytes"<<std::endl;
    block_data = new boxm2_data_base(new char[num_bytes], num_bytes, id, read_only);
    block_data->set_default_value(type, data);
  }
  else {
    // initialize an empty data
    //std::cout<<"boxm2_lru_cache::initializing empty data "<<id<<" type: "<<type<<std::endl;

    boxm2_block_metadata data = scene->get_block_metadata(id);
    // the following constructor also sets the default values
    block_data = new boxm2_data_base(data, type, read_only);
  }
  // grab a reference to the map of cached_data_
  std::map<boxm2_block_id, boxm2_data_base*>& data_map = this->cached_data_map(scene, type);


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
void boxm2_lru_cache::remove_data_base(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type, bool write_out)
{
  // grab a reference to the map of cached_data_
  std::map<boxm2_block_id, boxm2_data_base*>& data_map = this->cached_data_map(scene,type);
  // then look for the block you're requesting
  auto rem = data_map.find(id);
  if ( rem != data_map.end() )
  {
    // found the block,
    boxm2_data_base* litter = data_map[id];
    //if (!litter->read_only_) {
      // save it
      //std::cout<<"boxm2_lru_cache::remove_data_base "<<scene->xml_path()<<" type "<<type<<':'<<id<<"; saving to disk"<<std::endl;
    if (write_out)
        boxm2_sio_mgr::save_block_data_base(scene->data_path(), id, litter, type);
    //}
    //else
    //  std::cout<<"boxm2_lru_cache::remove_data_base "<<type<<':'<<id<<"; not saving to disk"<<std::endl;
    // now throw it away
    delete litter;
    data_map.erase(rem);
  }
}

//: replaces data in the cache with one here
void boxm2_lru_cache::replace_data_base(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type, boxm2_data_base* replacement)
{
  // grab a reference to the map of cached_data_
  std::map<boxm2_block_id, boxm2_data_base*>& data_map =
    this->cached_data_map(scene, type);

  // find old data base and copy it's read_only/write status
  auto rem = data_map.find(id);
  if ( rem != data_map.end() )
  {
    // found the block,
    boxm2_data_base* litter = data_map[id];
    replacement->read_only_ = litter->read_only_;
    // now throw it away
    delete litter;
    data_map.erase(rem);
  }

  // this->remove_data_base(id, type);
  // put the new one in there
  data_map[id] = replacement;

}

//: helper method returns a reference to correct data map (ensures one exists)
std::map<boxm2_block_id, boxm2_data_base*>& boxm2_lru_cache::cached_data_map(boxm2_scene_sptr & scene, const std::string& prefix)
{
  // if map for this particular data type doesn't exist, initialize it
  if ( cached_data_.find(scene) == cached_data_.end() )
  {
      std::map<std::string,std::map<boxm2_block_id, boxm2_data_base*> > scene_dmap;
      std::map<boxm2_block_id, boxm2_data_base*> dmap;
      scene_dmap[prefix] = dmap;
      cached_data_[scene] = scene_dmap;
  }
  if(cached_data_[scene].find(prefix) == cached_data_[scene].end())
  {
      std::map<boxm2_block_id, boxm2_data_base*> dmap;
      cached_data_[scene][prefix] = dmap;
  }
  // grab a reference to the map of cached_data_ and return it
  std::map<boxm2_block_id, boxm2_data_base*>& data_map = cached_data_[scene][prefix];
  return data_map;

}

//: helper method says whether or not block id is valid
bool boxm2_lru_cache::is_valid_id(boxm2_scene_sptr & scene, const boxm2_block_id& id)
{
  // use scene here to determine if this id is valid
  return scene->block_exists(id);

}


//: Summarizes this cache's data
std::string boxm2_lru_cache::to_string()
{

    std::stringstream stream;
    auto scene_block_iter =cached_blocks_.begin();
    auto scene_data_iter =cached_data_.begin();

  for(;scene_block_iter!=cached_blocks_.end(); scene_block_iter++,scene_data_iter++)
  {
      stream << "boxm2_lru_cache:: scene dir="<<scene_block_iter->first->data_path()<<'\n'
          << "  blocks: ";
      auto blk_iter = scene_block_iter->second.begin();
      for (; blk_iter != scene_block_iter->second.end(); ++blk_iter) {
          boxm2_block_id id = blk_iter->first;
          stream << '(' << id  << ")  ";
      }

      auto dat_iter = scene_data_iter->second.begin();
      for (; dat_iter != scene_data_iter->second.end(); ++dat_iter)
      {
          std::string data_type = dat_iter->first;
          stream<< "\n  data: "<<data_type<<' ';
          std::map<boxm2_block_id, boxm2_data_base*> dmap = dat_iter->second;
          std::map<boxm2_block_id, boxm2_data_base*>::iterator it;
          for (it = dmap.begin(); it != dmap.end(); ++it)
          {
              boxm2_block_id id = it->first;
              stream<< '(' << id << ")  ";
          }
      }
  }
  return stream.str();

}

//: dumps all data onto disk
void boxm2_lru_cache::write_to_disk()
{
   // save the data and delete
  auto scene_iter =cached_data_.begin();
  for(;scene_iter!=cached_data_.end(); scene_iter++)
  {
      std::map<std::string, std::map<boxm2_block_id, boxm2_data_base*> > &dmap = scene_iter->second;
      for (auto & iter : dmap)
      {
          for (auto it = iter.second.begin(); it != iter.second.end(); it++) {
              boxm2_block_id id = it->first;
              boxm2_sio_mgr::save_block_data_base(scene_iter->first->data_path(), it->first, it->second, iter.first);
          }
      }
  }
  auto scene_block_iter =cached_blocks_.begin();
  for(;scene_block_iter!=cached_blocks_.end(); scene_block_iter++)
  {

      for (auto iter = scene_block_iter->second.begin();
          iter != scene_block_iter->second.end(); iter++)
      {
          boxm2_block_id id = iter->first;
          boxm2_sio_mgr::save_block(scene_block_iter->first->data_path(), iter->second);
      }
  }

}
//: dumps all data onto disk
void boxm2_lru_cache::write_to_disk(boxm2_scene_sptr & scene)
{
   // save the data and delete
  auto scene_iter =cached_data_.begin();
  for(;scene_iter!=cached_data_.end(); scene_iter++)
  {
    if(scene_iter->first->id() != scene->id())
      continue;
      std::map<std::string, std::map<boxm2_block_id, boxm2_data_base*> > &dmap = scene_iter->second;
      for (auto & iter : dmap)
      {
          for (auto it = iter.second.begin(); it != iter.second.end(); it++) {
              boxm2_block_id id = it->first;
              boxm2_sio_mgr::save_block_data_base(scene_iter->first->data_path(), it->first, it->second, iter.first);
          }
      }
  }
  auto scene_block_iter =cached_blocks_.begin();
  for(;scene_block_iter!=cached_blocks_.end(); scene_block_iter++)
  {
    if(scene_block_iter->first->id() != scene->id())
      continue;
      for (auto iter = scene_block_iter->second.begin();
          iter != scene_block_iter->second.end(); iter++)
      {
          boxm2_block_id id = iter->first;
          boxm2_sio_mgr::save_block(scene_block_iter->first->data_path(), iter->second);
      }
  }
}

//: add a new scene to the cache
bool boxm2_lru_cache::add_scene(boxm2_scene_sptr & scene)
{
    if(cached_blocks_.find(scene) == cached_blocks_.end() && cached_data_.find(scene) == cached_data_.end())
    {
        cached_blocks_[scene] = std::map<boxm2_block_id, boxm2_block*>();
        cached_data_[scene]   =   std::map<std::string, std::map<boxm2_block_id, boxm2_data_base*> >();
        return true;
    }
    else
    {
        std::cout<<"The scene Already exists "<<std::endl;
        return false;
    }
}

//: remove a scene from the cache
bool boxm2_lru_cache::remove_scene(boxm2_scene_sptr &  /*scene*/)
{
  // not allowed / implemented; return false
  return false;
}

//: shows elements in cache
std::ostream& operator<<(std::ostream &s, boxm2_lru_cache& scene)
{
  return s << scene.to_string();
}
