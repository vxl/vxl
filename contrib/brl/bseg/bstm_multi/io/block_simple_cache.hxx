#include <iostream>
#include <cstddef>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include "block_simple_cache.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bstm/bstm_data_traits.h>
#include <bstm_multi/block_data_base.h>
#include <bstm_multi/io/block_sio_mgr.h>

//: PUBLIC create method, for creating singleton instance of boxm2_cache
template <typename Scene, typename Block>
void block_simple_cache<Scene, Block>::create(scene_sptr scene) {
  if (cache_base::exists()) {
  } else {
    cache_base::instance_ = new block_simple_cache(scene);
  }
}

//: constructor, set the directory path
template <typename Scene, typename Block>
block_simple_cache<Scene, Block>::block_simple_cache(scene_sptr scene)
    : cache_base() {
  // Default-initialize entries with empty maps
  cached_blocks_[scene];
  cached_data_[scene];
}

//: return list of scenes with data in the cache
template <typename Scene, typename Block>
std::vector<typename block_simple_cache<Scene, Block>::scene_sptr>
block_simple_cache<Scene, Block>::get_scenes() {
  std::set<scene_sptr> scenes;

  for (typename scene_block_map_t::const_iterator it = cached_blocks_.begin();
       it != cached_blocks_.end();
       ++it) {
    scenes.insert(it->first);
  }
  // in case the cache has data, but not the block
  for (typename scene_data_map_t::const_iterator it = cached_data_.begin();
       it != cached_data_.end();
       ++it) {
    scenes.insert(it->first);
  }
  return std::vector<scene_sptr>(scenes.begin(), scenes.end());
}

//: destructor flushes the memory for currently ongoing asynchronous requests
template <typename Scene, typename Block>
block_simple_cache<Scene, Block>::~block_simple_cache() {
  this->clear_cache();
}

//: delete all the memory
//  Caution: make sure to call write to disk methods not to loose writable data
template <typename Scene, typename Block>
void block_simple_cache<Scene, Block>::clear_cache() {
  // Clear all block data buffers
  typename scene_data_map_t::iterator scene_iter = cached_data_.begin();
  for (; scene_iter != cached_data_.end(); scene_iter++) {
    typename scene_data_map_t::mapped_type &dmap = scene_iter->second;
    for (typename scene_data_map_t::mapped_type::iterator iter = dmap.begin();
         iter != dmap.end();
         iter++) {
      for (typename id_data_map_t::iterator it = iter->second.begin();
           it != iter->second.end();
           it++)
        delete it->second;

      iter->second.clear();
    }
    scene_iter->second.clear();
  }
  cached_data_.clear();

  // Clear all blocks
  typename scene_block_map_t::iterator scene_block_iter =
      cached_blocks_.begin();
  for (; scene_block_iter != cached_blocks_.end(); scene_block_iter++) {
    for (typename id_block_map_t::iterator iter =
             scene_block_iter->second.begin();
         iter != scene_block_iter->second.end();
         iter++) {
      delete iter->second;
    }
    scene_block_iter->second.clear();
  }
  cached_blocks_.clear();
}

//: realization of abstract "get_block(block_id)"
template <typename Scene, typename Block>
Block *block_simple_cache<Scene, Block>::get_block(scene_sptr &scene,
                                                   block_id_t id) {
  block_metadata mdata = scene->get_block_metadata(id);

  //: add a scene
  if (cached_blocks_.count(scene) == 0) {
    this->add_scene(scene);
  }
#if 0
    if(!this->add_scene(scene))
        return NULL;
#endif

  //: add a block
  if (cached_blocks_[scene].count(id) == 0) {
    Block *loaded =
        block_sio_mgr<Block>::load_block(scene->data_path(), id, mdata);

    // if the block is null then initialize an empty one
    if (!loaded && scene->block_exists(id)) {
      std::cout << "block_simple_cache::initializing empty block " << id
               << std::endl;
      loaded = new Block(mdata);
    }
    // update cache before returning the block
    cached_blocks_[scene][id] = loaded;
  }
  return cached_blocks_[scene][id];
}

//: get data by type and id
template <typename Scene, typename Block>
block_data_base *
block_simple_cache<Scene, Block>::get_data_base(scene_sptr &scene,
                                                block_id_t id,
                                                std::string type,
                                                std::size_t num_bytes,
                                                bool read_only) {
  // grab a reference to the map of cached_data_
  id_data_map_t &data_map = this->cached_data_map(scene, type);
  std::size_t data_size = bstm_data_info::datasize(type);
  if (!scene->block_exists(id)) {
    return nullptr;
  }
  Block *blk = this->get_block(scene, id);
  // then look for the block you're requesting
  typename id_data_map_t::iterator iter = data_map.find(id);
  if (iter != data_map.end()) {
    // congrats you've found the data block in cache, update cache and return
    // block
    if (!read_only) // write-enable is enforced
      iter->second->enable_write();
    return iter->second;
  }

  // grab from disk
  block_data_base *loaded = block_sio_mgr<Block>::load_block_data_generic(
      scene->data_path(), id, type);
  block_metadata data = scene->get_block_metadata(id);

  // if num_bytes is greater than zero, then you're guaranteed to return a data
  // size with that many bytes
  if (num_bytes > 0) {
    // TODO check that loaded buffer size matches block's num_cells? Or at least
    // num_data? (those might be the same thing)

    // if loaded from disk is good and it matches size, you found it, return
    if (loaded && loaded->buffer_length() == num_bytes) {
      // update data map
      data_map[id] = loaded;
      if (!read_only) // write-enable is enforced
        loaded->enable_write();
      return loaded;
    }

    // requesting a specific number of bytes, and not found it on disk
    std::cout << "block_simple_cache::initializing empty data " << id
             << " type: " << type << " to size: " << num_bytes << " bytes"
             << std::endl;
    loaded = new block_data_base(new char[num_bytes], num_bytes, id, read_only);
    std::memset(loaded->data_buffer(), 0, num_bytes);
  } else {
    // otherwise it's a miss, load sync from disk, update cache
    if (!loaded && scene->block_exists(id)) {
      std::cout << "block_simple_cache::initializing empty data " << id
               << " type: " << type << std::endl;
      loaded =
          new block_data_base(new char[num_bytes], num_bytes, id, read_only);
      std::memset(loaded->data_buffer(), 0, num_bytes);
    }
  }

  // update data map
  data_map[id] = loaded;
  return loaded;
}

//: returns a data_base pointer which is initialized to the default value of the
// type.
//  If a block for this type exists on the cache, it is removed and replaced
//  with the new one.
//  This method does not check whether a block of this type already exists on
//  the disk nor writes it to the disk
template <typename Scene, typename Block>
block_data_base *
block_simple_cache<Scene, Block>::get_data_base_new(scene_sptr &scene,
                                                    block_id_t id,
                                                    std::string type,
                                                    std::size_t num_bytes,
                                                    bool read_only) {
  block_data_base *block_data;
  if (num_bytes > 0) {
    block_metadata data = scene->get_block_metadata(id);
    // requesting a specific number of bytes,
    // std::cout<<"block_simple_cache::initializing empty data "<<id
    //        <<" type: "<<type
    //        <<" to size: "<<num_bytes<<" bytes"<<std::endl;
    block_data =
        new block_data_base(new char[num_bytes], num_bytes, id, read_only);
    std::memset(block_data->data_buffer(), 0, num_bytes);
  } else {
    // initialize an empty data
    // std::cout<<"block_simple_cache::initializing empty data "<<id<<" type:
    // "<<type<<std::endl;

    block_metadata data = scene->get_block_metadata(id);
    // the following constructor also sets the default values
    block_data = new block_data_base(data, type, read_only);
  }
  // grab a reference to the map of cached_data_
  id_data_map_t &data_map = this->cached_data_map(scene, type);

  // then look for the block you're requesting
  typename id_data_map_t::iterator iter = data_map.find(id);
  if (iter != data_map.end()) {
    // congrats you've found the data block in cache, now throw it away
    delete iter->second;
    data_map.erase(iter);
  }

  // now store the block in the cache
  data_map[id] = block_data;
  return block_data;
}

//: removes data from this cache (may or may not write to disk first)
template <typename Scene, typename Block>
void block_simple_cache<Scene, Block>::remove_data_base(scene_sptr &scene,
                                                        block_id_t id,
                                                        std::string type,
                                                        bool write_out) {
  // grab a reference to the map of cached_data_
  id_data_map_t &data_map = this->cached_data_map(scene, type);
  // then look for the block you're requesting
  typename id_data_map_t::iterator rem = data_map.find(id);
  if (rem != data_map.end()) {
    // found the block,
    block_data_base *litter = data_map[id];
    // if (!litter->read_only_) {
    // save it
    // std::cout<<"block_simple_cache::remove_data_base "<<scene->xml_path()<<"
    // type
    // "<<type<<':'<<id<<"; saving to disk"<<std::endl;
    if (write_out)
      block_sio_mgr<Block>::save_block_data_base(
          scene->data_path(), id, litter, type);
    //}
    // else
    //  std::cout<<"block_simple_cache::remove_data_base "<<type<<':'<<id<<"; not
    //  saving to disk"<<std::endl;
    // now throw it away
    delete litter;
    data_map.erase(rem);
  }
}

//: replaces data in the cache with one here
template <typename Scene, typename Block>
void block_simple_cache<Scene, Block>::replace_data_base(
    scene_sptr &scene,
    block_id_t id,
    std::string type,
    block_data_base *replacement) {
  // grab a reference to the map of cached_data_
  id_data_map_t &data_map = this->cached_data_map(scene, type);

  // find old data base and copy it's read_only/write status
  typename id_data_map_t::iterator rem = data_map.find(id);
  if (rem != data_map.end()) {
    // found the block,
    block_data_base *litter = data_map[id];
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
template <typename Scene, typename Block>
typename block_simple_cache<Scene, Block>::id_data_map_t &
block_simple_cache<Scene, Block>::cached_data_map(scene_sptr &scene,
                                                  std::string prefix) {
  return cached_data_[scene][prefix];
}

//: helper method says whether or not block id is valid
template <typename Scene, typename Block>
bool block_simple_cache<Scene, Block>::is_valid_id(scene_sptr &scene,
                                                   block_id_t id) {
  // use scene here to determine if this id is valid
  return scene->block_exists(id);
}

//: Summarizes this cache's data
template <typename Scene, typename Block>
std::string block_simple_cache<Scene, Block>::to_string() {

  std::stringstream stream;
  typename scene_block_map_t::iterator scene_block_iter =
      cached_blocks_.begin();
  typename scene_data_map_t::iterator scene_data_iter = cached_data_.begin();

  for (; scene_block_iter != cached_blocks_.end();
       scene_block_iter++, scene_data_iter++) {
    stream << "block_simple_cache:: scene dir="
           << scene_block_iter->first->data_path() << '\n'
           << "  blocks: ";
    typename id_block_map_t::iterator blk_iter =
        scene_block_iter->second.begin();
    for (; blk_iter != scene_block_iter->second.end(); ++blk_iter) {
      stream << '(' << blk_iter->first << ")  ";
    }

    typename scene_data_map_t::mapped_type::iterator dat_iter =
        scene_data_iter->second.begin();
    for (; dat_iter != scene_data_iter->second.end(); ++dat_iter) {
      std::string data_type = dat_iter->first;
      stream << "\n  data: " << data_type << ' ';
      id_data_map_t dmap = dat_iter->second;
      typename id_data_map_t::iterator it;
      for (it = dmap.begin(); it != dmap.end(); ++it) {
        stream << '(' << it->first << ")  ";
      }
    }
  }
  return stream.str();
}

//: dumps all data onto disk
template <typename Scene, typename Block>
void block_simple_cache<Scene, Block>::write_to_disk() {
  // save the data and delete
  typename scene_data_map_t::iterator scene_iter = cached_data_.begin();
  for (; scene_iter != cached_data_.end(); scene_iter++) {
    typename scene_data_map_t::mapped_type &dmap = scene_iter->second;
    for (typename scene_data_map_t::mapped_type::iterator iter = dmap.begin();
         iter != dmap.end();
         iter++) {
      for (typename id_data_map_t::iterator it = iter->second.begin();
           it != iter->second.end();
           it++) {
        block_sio_mgr<Block>::save_block_data_base(
            scene_iter->first->data_path(), it->first, it->second, iter->first);
      }
    }
  }
  typename scene_block_map_t::iterator scene_block_iter =
      cached_blocks_.begin();
  for (; scene_block_iter != cached_blocks_.end(); scene_block_iter++) {

    for (typename id_block_map_t::iterator iter =
             scene_block_iter->second.begin();
         iter != scene_block_iter->second.end();
         iter++) {
      block_sio_mgr<Block>::save_block(scene_block_iter->first->data_path(),
                                       iter->second);
    }
  }
}

//: dumps all data onto disk
template <typename Scene, typename Block>
void block_simple_cache<Scene, Block>::write_to_disk(scene_sptr &scene) {
  // save the data buffers
  typename scene_data_map_t::iterator scene_iter = cached_data_.begin();
  for (; scene_iter != cached_data_.end(); scene_iter++) {
    if (scene_iter->first->id() != scene->id())
      continue;
    typename scene_data_map_t::mapped_type &dmap = scene_iter->second;
    for (typename scene_data_map_t::mapped_type::iterator iter = dmap.begin();
         iter != dmap.end();
         iter++) {
      for (typename id_data_map_t::iterator it = iter->second.begin();
           it != iter->second.end();
           it++) {
        block_sio_mgr<Block>::save_block_data_base(
            scene_iter->first->data_path(), it->first, it->second, iter->first);
      }
    }
  }

  // Save the blocks
  typename scene_block_map_t::iterator scene_block_iter =
      cached_blocks_.begin();
  for (; scene_block_iter != cached_blocks_.end(); scene_block_iter++) {
    if (scene_block_iter->first->id() != scene->id())
      continue;
    for (typename id_block_map_t::iterator iter =
             scene_block_iter->second.begin();
         iter != scene_block_iter->second.end();
         iter++) {
      block_sio_mgr<Block>::save_block(scene_block_iter->first->data_path(),
                                       iter->second);
    }
  }
}

//: add a new scene to the cache
template <typename Scene, typename Block>
bool block_simple_cache<Scene, Block>::add_scene(scene_sptr &scene) {
  if (cached_blocks_.count(scene) || cached_data_.count(scene)) {
    std::cout << "The scene Already exists " << std::endl;
    return false;
  } else {
    cached_blocks_[scene];
    cached_data_[scene];
    return true;
  }
}

//: remove a scene from the cache
template <typename Scene, typename Block>
bool block_simple_cache<Scene, Block>::remove_scene(scene_sptr &scene) {
  // not allowed / implemented; return false
  return false;
}
