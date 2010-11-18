#ifndef boxm2_cache_h_
#define boxm2_cache_h_
//:
// \file
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_block_id.h>
#include <boxm2/boxm2_asio_mgr.h>

//: boxm2_cache: top level storage (abstract) class
// - handles all block io, from both the cache/marshaller and the disk
// TODO: needs some notion of scene size (number of blocks in each dimension ...)
class boxm2_cache
{
  public:

    //: returns block pointer to block specified by ID
    virtual boxm2_block* get_block(boxm2_block_id id) = 0;

    //: updates cache based on input id (will call async loads)
    virtual void update_cache(boxm2_block_id id) = 0;

  protected:

    //: boxm2_asio_manager handles asio reqeusts
    boxm2_asio_mgr io_mgr;
};

#endif //boxm2_cache_h_
