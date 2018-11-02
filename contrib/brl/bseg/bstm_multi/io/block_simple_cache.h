#ifndef block_simple_cache_h_
#define block_simple_cache_h_

//:
// \file block_simple_cache.h
// \brief block_simple_cache is a singleton, derived from
// abstract class block_cache. Simply stores blocks and data buffers in a map:
// {scene -> {id -> block or data}}
//
// \author Raphael Kargon
// \date 03 Aug 2017

#include <iostream>
#include <cstddef>
#include <map>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bstm_multi/block_data_base.h>
#include <bstm_multi/io/block_cache.h>

//: A cache that keeps the most recently used blocks and data, while kicking out
// the least recently used blocks and data to make more room.
//  Currently just stores blocks in a map, not caring about space
template <typename Scene, typename Block>
class block_simple_cache : public block_cache<Scene, Block> {
public:
  typedef typename Block::metadata_t block_metadata;
  // "import" base class's typedefs
  typedef block_cache<Scene, Block> cache_base;
  using typename cache_base::scene_sptr;
  using typename cache_base::block_id_t;

  // convenience typedefs for various maps and whatnot
  typedef std::map<block_id_t, Block *> id_block_map_t;
  typedef std::map<scene_sptr, id_block_map_t> scene_block_map_t;
  typedef std::map<block_id_t, block_data_base *> id_data_map_t;
  typedef std::map<scene_sptr, std::map<std::string, id_data_map_t> >
      scene_data_map_t;

  //: create function used instead of constructor
  static void create(scene_sptr scene);

  //: returns block pointer to block specified by ID
  Block *get_block(scene_sptr &scene, block_id_t id) override;

  //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS
  // CANNOT BE VIRTUAL)
  block_data_base *get_data_base(scene_sptr &scene,
                                         block_id_t id,
                                         std::string type,
                                         std::size_t num_bytes = 0,
                                         bool read_only = true) override;

  //: returns a data_base pointer which is initialized to the default value of
  // the type.
  //  If a block for this type exists on the cache, it is removed and replaced
  //  with the new one.
  //  This method does not check whether a block of this type already exists on
  //  the disc nor writes it to the disc
  block_data_base *get_data_base_new(scene_sptr &scene,
                                             block_id_t id,
                                             std::string type,
                                             std::size_t num_bytes = 0,
                                             bool read_only = true) override;

  //: removes data from this cache (may or may not write to disk first)
  void remove_data_base(scene_sptr &scene,
                                block_id_t id,
                                std::string type,
                                bool write_out = true) override;

  //: replaces a database in the cache, deletes it
  void replace_data_base(scene_sptr &scene,
                                 block_id_t id,
                                 std::string type,
                                 block_data_base *replacement) override;

  //: dumps writeable data to disk
  void write_to_disk() override;

  //: dumps writeable data for specified scene to disk
  void write_to_disk(scene_sptr &scene) override;

  //: add a new scene to the cache
  bool add_scene(scene_sptr &scene) override;

  //: remove an existing scene from the cache
  bool remove_scene(scene_sptr &scene) override;

  //: to string method returns a string describing the cache's current state
  std::string to_string();

  //: delete all the memory, caution: make sure to call write to disc methods
  // not to loose writable data
  void clear_cache() override;

  //: return the list of scenes with any data in the cache
  std::vector<scene_sptr> get_scenes() override;

private:
  //: hidden constructor (private so it cannot be called -- forces the class to
  // be singleton)
  block_simple_cache(scene_sptr scene);

  //: hidden destructor (private so it cannot be called -- forces the class to
  // be singleton)
  ~block_simple_cache() override;

  //: keep a map of Block pointers (size will be limited to 9 blocks
  scene_block_map_t cached_blocks_;

  //: keeps one copy of each type of cached data
  scene_data_map_t cached_data_;

  // ---------Helper Methods --------------------------------------------------

  //: helper method returns a reference to correct data map (ensures one exists)
  id_data_map_t &cached_data_map(scene_sptr &scene, std::string prefix);

  //: helper method determines if this block is
  bool is_valid_id(scene_sptr &scene, block_id_t);
  // --------------------------------------------------------------------------
};

//: shows elements in cache
template <typename Scene, typename Block>
std::ostream &operator<<(std::ostream &s,
                        block_simple_cache<Scene, Block> &scene) {
  return s << scene.to_string();
}

#endif // block_simple_cache_h_
