#ifndef boxm2_lru_cache_h_
#define boxm2_lru_cache_h_
//:
// \file
// \brief boxm2_lru_cache (least recently used) is a singleton, derived from abstract class boxm2_cache

#include <boxm2/io/boxm2_cache.h>
#include <vcl_iostream.h>

#define MAX_BYTES 1024*1024*1024*4 // 4 gigs of memory is max...

//: A cache that keeps the most recently used blocks and data, while kicking out the least recently used blocks and data to make more room.
//  Currently just stores blocks in a map, not caring about space
//  \todo implement with timestamped maps.
class boxm2_lru_cache : public boxm2_cache
{
  public:

    //: create function used instead of constructor
    static void create(boxm2_scene_sptr scene, BOXM2_IO_FS_TYPE fs_type=LOCAL);

    //: returns block pointer to block specified by ID
    virtual boxm2_block* get_block(boxm2_block_id id);

    //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
    virtual boxm2_data_base* get_data_base(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes=0, bool read_only = true);

    //: returns a data_base pointer which is initialized to the default value of the type.
    //  If a block for this type exists on the cache, it is removed and replaced with the new one.
    //  This method does not check whether a block of this type already exists on the disc nor writes it to the disc
    virtual boxm2_data_base* get_data_base_new(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes=0, bool read_only = true);

    //: removes data from this cache (may or may not write to disk first)
    virtual void remove_data_base(boxm2_block_id id, vcl_string type);

    //: replaces a database in the cache, deletes it
    virtual void replace_data_base(boxm2_block_id id, vcl_string type, boxm2_data_base* replacement);

    //: dumps writeable data to disk
    virtual void write_to_disk();

    //: to string method returns a string describing the cache's current state
    vcl_string to_string();

    //: delete all the memory, caution: make sure to call write to disc methods not to loose writable data
    virtual void clear_cache();

  private:

    //: hidden constructor (private so it cannot be called -- forces the class to be singleton)
    boxm2_lru_cache(boxm2_scene_sptr scene, BOXM2_IO_FS_TYPE=LOCAL);

    //: hidden destructor (private so it cannot be called -- forces the class to be singleton)
    ~boxm2_lru_cache();

    //: keep a map of boxm2_block pointers (size will be limited to 9 blocks
    vcl_map<boxm2_block_id, boxm2_block*> cached_blocks_;

    //: keeps one copy of each type of cached data
    vcl_map<vcl_string, vcl_map<boxm2_block_id, boxm2_data_base*> > cached_data_;

    //: directory where blocks are found
    vcl_string scene_dir_;

    // ---------Helper Methods --------------------------------------------------

    //: helper method returns a reference to correct data map (ensures one exists)
    vcl_map<boxm2_block_id, boxm2_data_base*>& cached_data_map(vcl_string prefix);

    //: helper method determines if this block is
    bool is_valid_id(boxm2_block_id);
    // --------------------------------------------------------------------------
};

//: shows elements in cache
vcl_ostream& operator<<(vcl_ostream &s, boxm2_lru_cache& scene);

#endif // boxm2_lru_cache_h_
