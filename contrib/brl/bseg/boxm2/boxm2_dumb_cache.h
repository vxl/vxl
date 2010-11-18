#ifndef boxm2_dumb_cache_h_
#define boxm2_dumb_cache_h_
//:
// \file
#include <boxm2/boxm2_cache.h>

//: boxm2_dumb_cache - example realization of abstract cache class
class boxm2_dumb_cache : boxm2_cache
{
 public:

  //: returns block poitner to block specified by ID
  boxm2_block* get_block(boxm2_block_id id);

  //: updates cache
  void update_cache(boxm2_block_id id);
};

#endif // boxm2_dumb_cache_h_
