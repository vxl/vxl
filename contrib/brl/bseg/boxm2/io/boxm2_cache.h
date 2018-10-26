#ifndef boxm2_cache_h_
#define boxm2_cache_h_
//:
// \file
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/io/boxm2_asio_mgr.h>
#include <boxm2/io/boxm2_sio_mgr.h>

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
//: top level cache (abstract) class
// - handles all block io, from both the cache/marshaller and the disk and hanldes multiple scenes
// Multiple scenes are stored as a map of ids and scene smart pointers
// \todo needs some notion of scene size (number of blocks in each dimension ...)
//
//  SOMETHING to discuss is whether generic blocks should be passed from cache
//  or the specific templated blocks (as is implemented below).
//  Either way, one of the two will have to cast from generic to templated.

class boxm2_cache;
typedef vbl_smart_ptr<boxm2_cache> boxm2_cache_sptr;

class boxm2_cache: public vbl_ref_count
{
 public:
  void set_filesystem(BOXM2_IO_FS_TYPE fs_type) { filesystem_ = fs_type; }

  //: Use this instead of constructor
  static boxm2_cache_sptr instance();
  static bool         exists() { return boxm2_cache::instance_!=nullptr; }

  //: returns block pointer to block specified by ID
  virtual boxm2_block* get_block(boxm2_scene_sptr & scene, boxm2_block_id id) = 0;

  //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
  virtual boxm2_data_base* get_data_base(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type, std::size_t num_bytes=0, bool read_only = true) = 0;

  //: returns a data_base pointer which is initialized to the default value of the type.
  //  If a block for this type exists on the cache, it is removed and replaced with the new one.
  //  This method does not check whether a block of this type already exists on the disk nor writes it to the disk
  virtual boxm2_data_base* get_data_base_new(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type=nullptr, std::size_t num_bytes=0, bool read_only = true) = 0;

  //: removes data from this cache (may or may not write to disk first)
  //  Note that this function does not delete the memory, just removes it from the cache
  //  and puts it in the garbage vector
  virtual void remove_data_base(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type, bool write_out=true) = 0;

  virtual void replace_data_base(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type, boxm2_data_base* replacement)=0;

  //: returns data pointer to data specified by ID and data_type
  template <boxm2_data_type T>
  boxm2_data<T>* get_data(boxm2_scene_sptr & scene, boxm2_block_id id, std::size_t num_bytes=0, bool read_only=true);

  //: dumps writeable data onto disk
  // -- pure virtual method; see specialisations
  //: write all data
  virtual void write_to_disk() = 0;
  //: write only data for scene
  virtual void write_to_disk(boxm2_scene_sptr & scene) = 0;

  //: disable the write process
  // -- generic method: does not do anything; see specialisations
  virtual void disable_write() {}

  virtual bool add_scene(boxm2_scene_sptr & scene) = 0;

  virtual bool remove_scene(boxm2_scene_sptr & scene) = 0;
  //: delete all the memory
  // Caution: make sure to call write to disk methods not to loose writable data
  virtual void clear_cache() = 0;

  virtual std::vector<boxm2_scene_sptr> get_scenes() = 0;

 protected:

  //: hidden constructor
  //boxm2_cache(boxm2_scene_sptr scene, BOXM2_IO_FS_TYPE fs=LOCAL) : scenes_(1,scene), filesystem_(fs) {}
  boxm2_cache(BOXM2_IO_FS_TYPE fs=LOCAL) : filesystem_(fs) {}

  //: hidden destructor (protected so it cannot be called -- forces the class to be singleton)
  ~boxm2_cache() override = default;

  //: singleton instance of boxm2_cache
  static boxm2_cache_sptr instance_;

  //: boxm2_scene needs to be around to initialized uninitialized blocks
  //std::vector<boxm2_scene_sptr> scenes_;

  //: boxm2_asio_manager handles asio requests
  boxm2_asio_mgr io_mgr_;

  BOXM2_IO_FS_TYPE filesystem_;
};

//: returns a boxm2_data<T>* from the cache
//  This is a work around for the lack of support of virtual templated functions
template <boxm2_data_type T>
boxm2_data<T>* boxm2_cache::get_data(boxm2_scene_sptr & scene , boxm2_block_id id, std::size_t num_bytes, bool read_only)
{
  boxm2_data_base* base = this->get_data_base(scene, id, boxm2_data_traits<T>::prefix(), num_bytes, read_only);
  return static_cast<boxm2_data<T>* >(base);
}

//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_cache const& scene);
//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream& os, const boxm2_cache* &p);
//: Binary write boxm2_cache smart pointer to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_cache_sptr& sptr);
//: Binary write boxm2_cache smart pointer to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_cache_sptr const& sptr);

//: Binary load boxm2_cache  from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_cache &scene);
//: Binary load boxm2_cache  from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_cache* p);
//: Binary load boxm2_cache smart pointer from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_cache_sptr& sptr);
//: Binary load boxm2_cache smart pointer from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_cache_sptr const& sptr);

#endif // boxm2_cache_h_
