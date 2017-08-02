#ifndef bstm_multi_space_time_cache_h_
#define bstm_multi_space_time_cache_h_
//:
// \file space_time_cache.h
// \base class for a generic block cache templated on block type. Closely based
// off of boxm2_cache.h.
// \author Raphael Kargon
// \date August 1st, 2017

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

#include <bstm/basic/bstm_block_id.h>
#include <bstm/bstm_data_traits.h>
#include <bstm_multi/space_time_data.h>
#include <bstm_multi/space_time_data_base.h>
#include <bstm_multi/space_time_scene.h>
#include <bstm_multi/io/space_time_sio_mgr.h>

//: top level cache (abstract) class
// - handles all block io, from both the cache/marshaller and the disk and
// hanldes multiple scenes
// Multiple scenes are stored as a map of ids and scene smart pointers
// \todo needs some notion of scene size (number of blocks in each dimension
// ...)
//
//  SOMETHING to discuss is whether generic blocks should be passed from cache
//  or the specific templated blocks (as is implemented below).
//  Either way, one of the two will have to cast from generic to templated.

template <typename Block> class space_time_cache : public vbl_ref_count {
public:
  // typedef space_time_cache<Id,Block> self_t;
  typedef vbl_smart_ptr<space_time_cache> cache_sptr;
  typedef vbl_smart_ptr<space_time_scene<Block> > scene_sptr;
  //: Use this instead of constructor
  static cache_sptr instance() {
    if (!instance_)
      std::cerr
          << "warning: space_time_cache:: instance has not been created\n";
    return instance_;
  }

  static bool exists() { return space_time_cache::instance_; }

  //: returns block pointer to block specified by ID
  virtual Block *get_block(scene_sptr &scene, bstm_block_id id) = 0;

  //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS
  // CANNOT BE VIRTUAL)
  virtual space_time_data_base *get_data_base(scene_sptr &scene,
                                              bstm_block_id id,
                                              std::string type,
                                              std::size_t num_bytes = 0,
                                              bool read_only = true) = 0;

  //: returns a data_base pointer which is initialized to the default
  // value of the type. If a block for this type exists on the cache,
  // it is removed and replaced with the new one.  This method does not
  // check whether a block of this type already exists on the disk nor
  // writes it to the disk
  virtual space_time_data_base *get_data_base_new(scene_sptr &scene,
                                                  bstm_block_id id,
                                                  std::string type = 0,
                                                  std::size_t num_bytes = 0,
                                                  bool read_only = true) = 0;

  //: removes data from this cache (may or may not write to disk
  //  first) Note that this function does not delete the memory, just
  //  removes it from the cache and puts it in the garbage vector
  virtual void remove_data_base(scene_sptr &scene,
                                bstm_block_id id,
                                std::string type,
                                bool write_out = true) = 0;

  virtual void replace_data_base(scene_sptr &scene,
                                 bstm_block_id id,
                                 std::string type,
                                 space_time_data_base *replacement) = 0;

  //: returns data pointer to data specified by ID and data_type
  template <bstm_data_type T>
  space_time_data<T> *get_data(scene_sptr &scene,
                               bstm_block_id id,
                               std::size_t num_bytes = 0,
                               bool read_only = true);

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
  // Caution: make sure to call write to disk methods not to loose writable data
  virtual void clear_cache() = 0;

  virtual std::vector<scene_sptr> get_scenes() = 0;

protected:
  //: hidden constructor
  space_time_cache() {}
  //: hidden destructor (protected so it cannot be called -- forces the class to
  // be singleton)
  virtual ~space_time_cache() {}

  //: singleton instance of space_time_cache
  static cache_sptr instance_;

  //: bstm_multi_scene needs to be around to initialized uninitialized blocks
  // std::vector<scene_sptr> scenes_;

  //: bstm_multi_asio_manager handles asio requests
  space_time_sio_mgr<Block> io_mgr_;
};

//: returns a bstm_multi_data<T>* from the cache
//  This is a work around for the lack of support of virtual templated functions
template <typename Block>
template <bstm_data_type T>
space_time_data<T> *space_time_cache<Block>::get_data(scene_sptr &scene,
                                                      bstm_block_id id,
                                                      std::size_t num_bytes,
                                                      bool read_only) {
  space_time_data_base *base = this->get_data_base(
      scene, id, bstm_data_traits<T>::prefix(), num_bytes, read_only);
  return static_cast<space_time_data<T> *>(base);
}

// //: Binary write space_time_cache  to stream
// void vsl_b_write(vsl_b_ostream& os, space_time_cache const& scene);
// //: Binary write space_time_cache  to stream
// void vsl_b_write(vsl_b_ostream& os, const space_time_cache* &p);
// //: Binary write space_time_cache smart pointer to stream
// void vsl_b_write(vsl_b_ostream& os, space_time_cache_sptr& sptr);
// //: Binary write space_time_cache smart pointer to stream
// void vsl_b_write(vsl_b_ostream& os, space_time_cache_sptr const& sptr);

// //: Binary load space_time_cache  from stream.
// void vsl_b_read(vsl_b_istream& is, space_time_cache &scene);
// //: Binary load space_time_cache  from stream.
// void vsl_b_read(vsl_b_istream& is, space_time_cache* p);
// //: Binary load space_time_cache smart pointer from stream.
// void vsl_b_read(vsl_b_istream& is, space_time_cache_sptr& sptr);
// //: Binary load space_time_cache smart pointer from stream.
// void vsl_b_read(vsl_b_istream& is, space_time_cache_sptr const& sptr);

#endif // bstm_multi_space_time_cache_h_
