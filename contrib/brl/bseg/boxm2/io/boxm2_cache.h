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
class boxm2_cache: public vbl_ref_count
{
  public:
    
    //: Use this instead of constructor
    static boxm2_cache* instance();
    static bool         exists() { return (boxm2_cache::instance_!=0); }

    //: returns block pointer to block specified by ID
    virtual boxm2_block* get_block(boxm2_block_id id) = 0;

    //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
    virtual boxm2_data_base* get_data_base(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes=0) = 0;

    //: removes data from this cache (may or may not write to disk first)
    virtual void remove_data_base(boxm2_block_id id, vcl_string type)=0; 

    //: returns data pointer to data specified by ID and data_type
    template <boxm2_data_type T>
    boxm2_data<T>* get_data(boxm2_block_id id);
    
    //: scene accessor

  protected:

    //: hide constructor
    boxm2_cache(boxm2_scene_sptr scene) : scene_(scene) {}

    //: singleton instance of boxm2_cache
    static boxm2_cache* instance_; 

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

typedef vbl_smart_ptr<boxm2_cache> boxm2_cache_sptr;

//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_cache const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_cache* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_cache_sptr& sptr); 
void vsl_b_write(vsl_b_ostream& os, boxm2_cache_sptr const& sptr);

//: Binary load boxm2_cache  from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_cache &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_cache* p);
void vsl_b_read(vsl_b_istream& is, boxm2_cache_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_cache_sptr const& sptr);
#endif //boxm2_cache_h_
