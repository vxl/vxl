#ifndef bstm_multi_block_cache_h_
#define bstm_multi_block_cache_h_
//:
// \file block_cache.h
//
// \base class for a generic block cache templated on block and scene type.
// Closely based off of boxm2_cache.h.
//
// \author Raphael Kargon
// \date 01 Aug 2017

#include <iostream>
#include <cstddef>
#include <string>
#include <vector>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bstm/bstm_data_traits.h>
#include <bstm_multi/block_data.h>
#include <bstm_multi/block_data_base.h>
#include <bstm_multi/io/block_sio_mgr.h>

//: top level cache (abstract) class
// - handles all block io, from both the cache/marshaller and the disk and
// hanldes multiple scenes
// Multiple scenes are stored as a map of ids and scene smart pointers
// \todo needs some notion of scene size (number of blocks in each dimension
// ...)

template <typename Scene, typename Block>
class block_cache : public vbl_ref_count {
public:
  typedef typename Block::id_t block_id_t;
  typedef vbl_smart_ptr<block_cache> cache_sptr;
  typedef vbl_smart_ptr<Scene> scene_sptr;
  //: Use this instead of constructor
  static cache_sptr instance() {
    if (!instance_)
      std::cerr << "warning: block_cache:: instance has not been created\n";
    return instance_;
  }

  static bool exists() { return block_cache::instance_; }

  //: returns block pointer to block specified by ID
  virtual Block *get_block(scene_sptr &scene, block_id_t id) = 0;

  //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS
  // CANNOT BE VIRTUAL)
  virtual block_data_base *get_data_base(scene_sptr &scene,
                                         block_id_t id,
                                         std::string type,
                                         std::size_t num_bytes = 0,
                                         bool read_only = true) = 0;

  //: returns a data_base pointer which is initialized to the default
  // value of the type. If a block for this type exists on the cache,
  // it is removed and replaced with the new one.  This method does not
  // check whether a block of this type already exists on the disk nor
  // writes it to the disk
  virtual block_data_base *get_data_base_new(scene_sptr &scene,
                                             block_id_t id,
                                             std::string type = nullptr,
                                             std::size_t num_bytes = 0,
                                             bool read_only = true) = 0;

  //: removes data from this cache (may or may not write to disk
  //  first) Note that this function does not delete the memory, just
  //  removes it from the cache and puts it in the garbage vector
  virtual void remove_data_base(scene_sptr &scene,
                                block_id_t id,
                                std::string type,
                                bool write_out = true) = 0;

  virtual void replace_data_base(scene_sptr &scene,
                                 block_id_t id,
                                 std::string type,
                                 block_data_base *replacement) = 0;

  //: returns data pointer to data specified by ID and data_type
  //  This is a work around for the lack of support of virtual templated
  //  functions
  template <bstm_data_type T>
  block_data<T> *get_data(scene_sptr &scene,
                          block_id_t id,
                          std::size_t num_bytes = 0,
                          bool read_only = true) {
    block_data_base *base = this->get_data_base(
        scene, id, bstm_data_traits<T>::prefix(), num_bytes, read_only);
    return static_cast<block_data<T> *>(base);
  }

  //: dumps writeable data onto disk
  // -- pure virtual method; see specialisations
  //: write all data
  virtual void write_to_disk() = 0;
  //: write only data for scene
  virtual void write_to_disk(scene_sptr &scene) = 0;

  //: disable the write process
  // -- generic method: does not do anything; see specialisations
  virtual void disable_write() {}

  virtual bool add_scene(scene_sptr &scene) = 0;

  virtual bool remove_scene(scene_sptr &scene) = 0;
  //: delete all the memory
  // Caution: make sure to call write to disk methods not to lose writable data
  virtual void clear_cache() = 0;

  virtual std::vector<scene_sptr> get_scenes() = 0;

protected:
  //: hidden constructor
  block_cache() = default;
  //: hidden destructor (protected so it cannot be called -- forces the class to
  // be singleton)
  ~block_cache() override = default;

  //: singleton instance of block_cache
  static cache_sptr instance_;

  //: bstm_multi_scene needs to be around to initialized uninitialized blocks
  // std::vector<scene_sptr> scenes_;

  //: bstm_multi_asio_manager handles asio requests
  block_sio_mgr<Block> io_mgr_;
};

// define static members
template <typename Scene, typename Block>
vbl_smart_ptr<block_cache<Scene, Block> > block_cache<Scene, Block>::instance_ =
    nullptr;

#endif // bstm_multi_block_cache_h_
