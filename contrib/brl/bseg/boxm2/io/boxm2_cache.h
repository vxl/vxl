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

//: boxm2_cache: top level storage (abstract) class
// - handles all block io, from both the cache/marshaller and the disk
// TODO: needs some notion of scene size (number of blocks in each dimension ...)
//
//  SOMETHING to discuss is whether generic blocks should be passed from cache
//  or the specific templated blocks(as is implemented below).  either way, one of the two will have to
//  cast from generic to templated.
class boxm2_cache
{
  public:
  
    boxm2_cache(boxm2_scene* scene) : scene_(scene) {}

    //: returns block pointer to block specified by ID
    virtual boxm2_block* get_block(boxm2_block_id id) = 0;

    //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
    virtual boxm2_data_base* get_data_base(boxm2_block_id, vcl_string type) = 0;

    //: returns data pointer to data specified by ID and data_type
    template <boxm2_data_type T>
    boxm2_data<T>* get_data(boxm2_block_id id);
    
  protected:

    //: boxm2_scene needs to be around to initialized uninitalized blocks
    boxm2_scene* scene_; 

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

#endif //boxm2_cache_h_
