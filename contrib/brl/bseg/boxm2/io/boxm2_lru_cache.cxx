#include "boxm2_lru_cache.h"
//:
// \file
#include <boxm2/boxm2_block_metadata.h>
#include <vcl_sstream.h>

//: PUBLIC create method, for creating singleton instance of boxm2_cache
void boxm2_lru_cache::create(boxm2_scene_sptr scene, BOXM2_IO_FS_TYPE fs_type)
{
  if (boxm2_cache::exists())
  {
    if(boxm2_cache::instance_->get_scene()->xml_path() == scene->xml_path())
    {
      vcl_cout << "boxm2_lru_cache:: boxm2_cache singleton already created" << vcl_endl;
    }
    else
    {
      vcl_cout << "boxm2_lru_cache:: destroying current cache and creating new one: " << scene->data_path() << vcl_endl;
      instance_ = new boxm2_lru_cache(scene, fs_type);
      destroyer_.set_singleton(instance_);
    }
  }
  else {
    instance_ = new boxm2_lru_cache(scene, fs_type);
    destroyer_.set_singleton(instance_);
  }
}

//: constructor, set the directory path
boxm2_lru_cache::boxm2_lru_cache(boxm2_scene_sptr scene, BOXM2_IO_FS_TYPE fs_type) : boxm2_cache(scene,fs_type)
{
  scene_dir_ = scene->data_path();
}

//: destructor flushes the memory for currently ongoing asynchronous requests
boxm2_lru_cache::~boxm2_lru_cache()
{
  // save the data and delete
  for (vcl_map<vcl_string, vcl_map<boxm2_block_id, boxm2_data_base*> >::iterator iter = cached_data_.begin();
       iter != cached_data_.end(); iter++)
  {
    for (vcl_map<boxm2_block_id, boxm2_data_base*>::iterator it = iter->second.begin(); it != iter->second.end(); it++) {
      boxm2_block_id id = it->first;
#if 0 // Currently causing some blocks to not save
      if (!it->second->read_only_) {
        boxm2_sio_mgr::save_block_data_base(scene_dir_, it->first, it->second, iter->first);
      }
#endif
      // now throw it away
      delete it->second;
    }
    iter->second.clear();
  }

  for (vcl_map<boxm2_block_id, boxm2_block*>::iterator iter = cached_blocks_.begin();
       iter != cached_blocks_.end(); iter++)
  {
    boxm2_block_id id = iter->first;
#if 0 // Currently causing some blocks to not save
    if (!iter->second->read_only())
      boxm2_sio_mgr::save_block(scene_dir_, iter->second);
#endif
    delete iter->second;
  }
}

//: delete all the memory
//  Caution: make sure to call write to disk methods not to loose writable data
void boxm2_lru_cache::clear_cache()
{
  // delete
  for (vcl_map<vcl_string, vcl_map<boxm2_block_id, boxm2_data_base*> >::iterator iter = cached_data_.begin();
       iter != cached_data_.end(); iter++)
  {
    for (vcl_map<boxm2_block_id, boxm2_data_base*>::iterator it = iter->second.begin(); it != iter->second.end(); it++)
      delete it->second;
    iter->second.clear();
  }
  cached_data_.clear();

  for (vcl_map<boxm2_block_id, boxm2_block*>::iterator iter = cached_blocks_.begin();
       iter != cached_blocks_.end(); iter++)
    delete iter->second;
  cached_blocks_.clear();
}

//: realization of abstract "get_block(block_id)"
boxm2_block* boxm2_lru_cache::get_block(boxm2_block_id id)
{
  boxm2_block_metadata data = scene_->get_block_metadata(id);

  // then look for the block you're requesting
  if ( cached_blocks_.find(id) != cached_blocks_.end() )
  {
#ifdef DEBUG
    vcl_cout<<"CACHE HIT :)"<<vcl_endl;
#endif
    return cached_blocks_[id];
  }

#ifdef DEBUG
  vcl_cout<<"Cache miss :("<<vcl_endl;
#endif
  // otherwise load it from disk with blocking and update cache
  boxm2_block* loaded = boxm2_sio_mgr::load_block(scene_dir_, id, data, filesystem_);

  // if the block is null then initialize an empty one
  if (!loaded && scene_->block_exists(id)) {
    vcl_cout<<"boxm2_lru_cache::initializing empty block "<<id<<vcl_endl;

    loaded = new boxm2_block(data);
  }

  // update cache before returning the block
  cached_blocks_[id] = loaded;
  return loaded;
}

//: get data by type and id
boxm2_data_base* boxm2_lru_cache::get_data_base(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes, bool read_only)
{
  // grab a reference to the map of cached_data_
  vcl_map<boxm2_block_id, boxm2_data_base*>& data_map =
    this->cached_data_map(type);

  // then look for the block you're requesting
  vcl_map<boxm2_block_id, boxm2_data_base*>::iterator iter = data_map.find(id);
  if ( iter != data_map.end() )
  {
    // congrats you've found the data block in cache, update cache and return block
    if (!read_only)  // write-enable is enforced
      iter->second->enable_write();
    return iter->second;
  }

  // grab from disk
  boxm2_data_base* loaded = boxm2_sio_mgr::load_block_data_generic(scene_dir_, id, type, filesystem_);
  boxm2_block_metadata data = scene_->get_block_metadata(id);

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
    vcl_cout<<"boxm2_lru_cache::initializing empty data "<<id
            <<" type: "<<type
            <<" to size: "<<num_bytes<<" bytes"<<vcl_endl;
    loaded = new boxm2_data_base(new char[num_bytes], num_bytes, id, read_only);
    loaded->set_default_value(type, data);
  }
  else {
    // otherwise it's a miss, load sync from disk, update cache
    // loaded = boxm2_sio_mgr::load_block_data_generic(scene_dir_, id, type, filesystem_);
    if (!loaded && scene_->block_exists(id)) {
      vcl_cout<<"boxm2_lru_cache::initializing empty data "<<id<<" type: "<<type<<vcl_endl;
      loaded = new boxm2_data_base(data, type, read_only);
    }
  }

  // update data map
  data_map[id] = loaded;
  return loaded;
}

//: returns a data_base pointer which is initialized to the default value of the type.
//  If a block for this type exists on the cache, it is removed and replaced with the new one.
//  This method does not check whether a block of this type already exists on the disk nor writes it to the disk
boxm2_data_base* boxm2_lru_cache::get_data_base_new(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes, bool read_only)
{
  boxm2_data_base* block_data;
  if (num_bytes > 0)   {
    boxm2_block_metadata data = scene_->get_block_metadata(id);
    // requesting a specific number of bytes,
    //vcl_cout<<"boxm2_lru_cache::initializing empty data "<<id
    //        <<" type: "<<type
    //        <<" to size: "<<num_bytes<<" bytes"<<vcl_endl;
    vcl_cout<<id<<" init empty "<<type<<vcl_endl;
    block_data = new boxm2_data_base(new char[num_bytes], num_bytes, id, read_only);
    block_data->set_default_value(type, data);
  }
  else {
    // initialize an empty block
    //vcl_cout<<"boxm2_lru_cache::initializing empty data "<<id<<" type: "<<type<<vcl_endl;
    vcl_cout<<id<<" init empty "<<type<<vcl_endl;
    boxm2_block_metadata data = scene_->get_block_metadata(id);
    // the following constructor also sets the default values
    block_data = new boxm2_data_base(data, type, read_only);
  }

  // grab a reference to the map of cached_data_
  vcl_map<boxm2_block_id, boxm2_data_base*>& data_map = this->cached_data_map(type);

  // then look for the block you're requesting
  vcl_map<boxm2_block_id, boxm2_data_base*>::iterator iter = data_map.find(id);
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
void boxm2_lru_cache::remove_data_base(boxm2_block_id id, vcl_string type)
{
  // grab a reference to the map of cached_data_
  vcl_map<boxm2_block_id, boxm2_data_base*>& data_map =
    this->cached_data_map(type);
  // then look for the block you're requesting
  vcl_map<boxm2_block_id, boxm2_data_base*>::iterator rem = data_map.find(id);
  if ( rem != data_map.end() )
  {
    // found the block,
    boxm2_data_base* litter = data_map[id];

    if (!litter->read_only_) {
      // save it
      vcl_cout<<"boxm2_lru_cache::remove_data_base "<<type<<':'<<id<<"; saving to disk"<<vcl_endl;
      boxm2_sio_mgr::save_block_data_base(scene_dir_, id, litter, type);
    }
    else
      vcl_cout<<"boxm2_lru_cache::remove_data_base "<<type<<':'<<id<<"; not saving to disk"<<vcl_endl;
    // now throw it away
    delete litter;
    data_map.erase(rem);
  }
}

//: replaces data in the cache with one here
void boxm2_lru_cache::replace_data_base(boxm2_block_id id, vcl_string type, boxm2_data_base* replacement)
{
  // grab a reference to the map of cached_data_
  vcl_map<boxm2_block_id, boxm2_data_base*>& data_map =
    this->cached_data_map(type);

  // find old data base and copy it's read_only/write status
  vcl_map<boxm2_block_id, boxm2_data_base*>::iterator rem = data_map.find(id);
  if ( rem != data_map.end() )
  {
    // found the block,
    boxm2_data_base* litter = data_map[id];
    replacement->read_only_ = litter->read_only_;
#if 0 // don't need to write on a replace data...
    if (!litter->read_only_) {
      boxm2_sio_mgr::save_block_data_base(scene_dir_, id, litter, type);
    }
#endif
    // now throw it away
    delete litter;
    data_map.erase(rem);
  }

  // this->remove_data_base(id, type);
  // put the new one in there
  data_map[id] = replacement;
}

//: helper method returns a reference to correct data map (ensures one exists)
vcl_map<boxm2_block_id, boxm2_data_base*>& boxm2_lru_cache::cached_data_map(vcl_string prefix)
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

//: helper method says whether or not block id is valid
bool boxm2_lru_cache::is_valid_id(boxm2_block_id id)
{
  // use scene here to determine if this id is valid
  return scene_->block_exists(id);
}


//: Summarizes this cache's data
vcl_string boxm2_lru_cache::to_string()
{
  vcl_stringstream stream;
  stream << "boxm2_lru_cache:: scene dir="<<scene_dir_<<'\n'
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
    stream<< "\n  data: "<<data_type<<' ';
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

//: dumps all data onto disk
void boxm2_lru_cache::write_to_disk()
{
   // save the data and delete
  for (vcl_map<vcl_string, vcl_map<boxm2_block_id, boxm2_data_base*> >::iterator iter = cached_data_.begin();
       iter != cached_data_.end(); iter++)
  {
    for (vcl_map<boxm2_block_id, boxm2_data_base*>::iterator it = iter->second.begin(); it != iter->second.end(); it++) {
      boxm2_block_id id = it->first;
      // if (!it->second->read_only_)
        boxm2_sio_mgr::save_block_data_base(scene_dir_, it->first, it->second, iter->first);
    }
  }

  for (vcl_map<boxm2_block_id, boxm2_block*>::iterator iter = cached_blocks_.begin();
       iter != cached_blocks_.end(); iter++)
  {
    boxm2_block_id id = iter->first;
    // if (!iter->second->read_only())
    boxm2_sio_mgr::save_block(scene_dir_, iter->second);
  }
}

//: shows elements in cache
vcl_ostream& operator<<(vcl_ostream &s, boxm2_lru_cache& scene)
{
  return s << scene.to_string();
}

