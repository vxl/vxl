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
//: top level storage (abstract) class
// - handles all block io, from both the cache/marshaller and the disk
// TODO: needs some notion of scene size (number of blocks in each dimension ...)
//
//  SOMETHING to discuss is whether generic blocks should be passed from cache
//  or the specific templated blocks (as is implemented below).
//  Either way, one of the two will have to cast from generic to templated.

class boxm2_cache;
typedef vbl_smart_ptr<boxm2_cache> boxm2_cache_sptr;

class boxm2_cache_destroyer;

class boxm2_cache: public vbl_ref_count
{
 public:

  //: Use this instead of constructor
  static boxm2_cache_sptr instance();
  static bool         exists() { return boxm2_cache::instance_!=0; }

  //: the destructor instance to make sure memory is deallocated when the program exits
  static boxm2_cache_destroyer destroyer_;  // its not a pointer so C++ will make sure that it's destructor will be called
  friend class boxm2_cache_destroyer;

  //: returns block pointer to block specified by ID
  virtual boxm2_block* get_block(boxm2_block_id id) = 0;

  //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
  virtual boxm2_data_base* get_data_base(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes=0, bool read_only = true) = 0;

  //: removes data from this cache (may or may not write to disk first)
  // Note that this function does not delete the memory, just removes it from the cache
  // and puts it in the garbage vector
  virtual void remove_data_base(boxm2_block_id id, vcl_string type)=0;
  virtual void replace_data_base(boxm2_block_id id, vcl_string type, boxm2_data_base* replacement)=0;

  //: returns data pointer to data specified by ID and data_type
  template <boxm2_data_type T>
  boxm2_data<T>* get_data(boxm2_block_id id);

  //: disable the write process -- generic method does not do anything; see specialisations
  virtual void disable_write() {}

 protected:

  //: hide constructor
  boxm2_cache(boxm2_scene_sptr scene) : scene_(scene) {}

  //: hidden destructor (private so it cannot be called -- forces the class to be singleton)
  virtual ~boxm2_cache() {}

  //: singleton instance of boxm2_cache
  static boxm2_cache_sptr instance_;

  //: boxm2_scene needs to be around to initialized uninitialized blocks
  boxm2_scene_sptr scene_;

  //: boxm2_asio_manager handles asio requests
  boxm2_asio_mgr io_mgr_;
};

//: returns a boxm2_data<T>* from the cache
//  This is a work around for the lack of support of virtual templated functions
template <boxm2_data_type T>
boxm2_data<T>* boxm2_cache::get_data(boxm2_block_id id)
{
  boxm2_data_base* base = this->get_data_base(id, boxm2_data_traits<T>::prefix());
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


//: create another class whose sole purpose is to destroy the singleton instance
class boxm2_cache_destroyer
{
 public:
  boxm2_cache_destroyer(boxm2_cache_sptr s = 0);
  ~boxm2_cache_destroyer();

  void set_singleton(boxm2_cache_sptr s);
 private:
  boxm2_cache_sptr s_;
};

#endif //boxm2_cache_h_
