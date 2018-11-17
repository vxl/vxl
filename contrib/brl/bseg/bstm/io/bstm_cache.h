#ifndef bstm_cache_h_
#define bstm_cache_h_
//:
// \file
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_time_block.h>
#include <bstm/basic/bstm_block_id.h>
#include <bstm/bstm_data_base.h>

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
//: top level storage (abstract) class
// - handles all block io, from both the cache/marshaller and the disk

class bstm_cache;
typedef vbl_smart_ptr<bstm_cache> bstm_cache_sptr;

class bstm_cache_destroyer;

class bstm_cache: public vbl_ref_count
{
 public:

  //: Use this instead of constructor
  static bstm_cache_sptr instance();
  static bool         exists() { return bstm_cache::instance_!=nullptr; }

  //: the destructor instance to make sure memory is deallocated when the program exits
  static bstm_cache_destroyer destroyer_;  // it's not a pointer so C++ will make sure that its destructor will be called
  friend class bstm_cache_destroyer;

  //: returns block pointer to block specified by ID
  virtual bstm_block* get_block(bstm_block_id id) = 0;

  //: returns pointer to time_block specified by ID
  virtual bstm_time_block* get_time_block(bstm_block_id id) = 0;

  virtual void replace_time_block(bstm_block_id id, bstm_time_block* replacement)=0;

  //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
  virtual bstm_data_base* get_data_base(bstm_block_id id, std::string type, std::size_t num_bytes=0, bool read_only = true) = 0;

  //: returns a data_base pointer which is initialized to the default value of the type.
  //  If a block for this type exists on the cache, it is removed and replaced with the new one.
  //  This method does not check whether a block of this type already exists on the disk nor writes it to the disk
  virtual bstm_data_base* get_data_base_new(bstm_block_id id, std::string type=nullptr, std::size_t num_bytes=0, bool read_only = true) = 0;

  //: removes data from this cache (may or may not write to disk first)
  //  Note that this function does not delete the memory, just removes it from the cache
  //  and puts it in the garbage vector
  virtual void remove_data_base(bstm_block_id id, std::string type)=0;

  virtual void replace_data_base(bstm_block_id id, std::string type, bstm_data_base* replacement)=0;

  //: dumps writeable data onto disk
  // -- pure virtual method; see specialisations
  virtual void write_to_disk() = 0;

  //: delete all the memory
  virtual void clear_cache() = 0;

  //: return scene sptr
  virtual bstm_scene_sptr get_scene() { return scene_; }

 protected:

  //: hidden constructor
  bstm_cache(bstm_scene_sptr scene) : scene_(scene) {}

  //: hidden destructor (protected so it cannot be called -- forces the class to be singleton)
  ~bstm_cache() override = default;

  //: singleton instance of bstm_cache
  static bstm_cache_sptr instance_;

  //: bstm_scene needs to be around to initialized uninitialized blocks
  bstm_scene_sptr scene_;

};

//: Binary write bstm_cache  to stream
void vsl_b_write(vsl_b_ostream& os, bstm_cache const& scene);
//: Binary write bstm_cache  to stream
void vsl_b_write(vsl_b_ostream& os, const bstm_cache* &p);
//: Binary write bstm_cache smart pointer to stream
void vsl_b_write(vsl_b_ostream& os, bstm_cache_sptr& sptr);
//: Binary write bstm_cache smart pointer to stream
void vsl_b_write(vsl_b_ostream& os, bstm_cache_sptr const& sptr);

//: Binary load bstm_cache  from stream.
void vsl_b_read(vsl_b_istream& is, bstm_cache &scene);
//: Binary load bstm_cache  from stream.
void vsl_b_read(vsl_b_istream& is, bstm_cache* p);
//: Binary load bstm_cache smart pointer from stream.
void vsl_b_read(vsl_b_istream& is, bstm_cache_sptr& sptr);
//: Binary load bstm_cache smart pointer from stream.
void vsl_b_read(vsl_b_istream& is, bstm_cache_sptr const& sptr);


//: create another class whose sole purpose is to destroy the singleton instance
class bstm_cache_destroyer
{
 public:
  bstm_cache_destroyer(const bstm_cache_sptr& s = nullptr);
  ~bstm_cache_destroyer();

  void set_singleton(const bstm_cache_sptr& s);
 private:
  bstm_cache_sptr s_;
};

#endif // bstm_cache_h_
