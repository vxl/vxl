#ifndef boxm2_lru_cache_h_
#define boxm2_lru_cache_h_
//:
// \file
#include <boxm2/io/boxm2_cache.h>
#include <vcl_iostream.h>

#define MAX_BYTES 1024*1024*1024*4 //4 gigs of memory is max...

//: A cache that keeps the most recently used blocks and data, while kicking out the least recently used blocks and data to make more room.
//  Currently just stores blocks in a map, not caring about space
//  TODO: implement with timestamped maps.
class boxm2_lru_cache : public boxm2_cache
{
  public:
    //: construct with directory and scene dimensions (blocknum)
    boxm2_lru_cache(boxm2_scene* scene);
    ~boxm2_lru_cache();

    //: returns block pointer to block specified by ID
    virtual boxm2_block* get_block(boxm2_block_id id);

    //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
    virtual boxm2_data_base* get_data_base(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes=0);

    //: removes data from this cache (may or may not write to disk first)
    virtual void remove_data_base(boxm2_block_id id, vcl_string type); 

    //: to string method returns a string describing the cache's current state
    vcl_string to_string();

  private:

    //: keep a map of boxm2_block pointers (size will be limited to 9 blocks
    vcl_map<boxm2_block_id, boxm2_block*> cached_blocks_;

    //: keeps one copy of each type of cached data
    vcl_map<vcl_string, vcl_map<boxm2_block_id, boxm2_data_base*> > cached_data_;

    //: directory where blocks are found
    vcl_string scene_dir_;

    //---------Helper Methods --------------------------------------------------
    //: helper method returns a reference to correct data map (ensures one exists)
    vcl_map<boxm2_block_id, boxm2_data_base*>& cached_data_map(vcl_string prefix);

    //: helper method determines if this block is
    bool is_valid_id(boxm2_block_id);
    //--------------------------------------------------------------------------
};

//: shows elements in cache
vcl_ostream& operator<<(vcl_ostream &s, boxm2_lru_cache& scene);

#endif // boxm2_lru_cache_h_
