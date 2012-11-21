#ifndef bstm_lru_cache_h_
#define bstm_lru_cache_h_
//:
// \file
// \brief bstm_lru_cache (least recently used) is a singleton, derived from abstract class bstm_cache

#include <bstm/io/bstm_cache.h>
#include <vcl_iostream.h>


//: A cache that keeps the most recently used blocks and data, while kicking out the least recently used blocks and data to make more room.
//  Currently just stores blocks in a map, not caring about space
class bstm_lru_cache : public bstm_cache
{
  public:

    //: create function used instead of constructor
    static void create(bstm_scene_sptr scene);

    //: returns block pointer to block specified by ID
    virtual bstm_block* get_block(bstm_block_id id);

    //: returns pointer to time_block specified by ID
    virtual bstm_time_block* get_time_block(bstm_block_id id);

    virtual void replace_time_block(bstm_block_id id, bstm_time_block* replacement);

    //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
    virtual bstm_data_base* get_data_base(bstm_block_id id, vcl_string type, vcl_size_t num_bytes=0, bool read_only = true);

    //: returns a data_base pointer which is initialized to the default value of the type.
    //  If a block for this type exists on the cache, it is removed and replaced with the new one.
    //  This method does not check whether a block of this type already exists on the disc nor writes it to the disc
    virtual bstm_data_base* get_data_base_new(bstm_block_id id, vcl_string type, vcl_size_t num_bytes=0, bool read_only = true);

    //: removes data from this cache (may or may not write to disk first)
    virtual void remove_data_base(bstm_block_id id, vcl_string type);

    //: replaces a database in the cache, deletes it
    virtual void replace_data_base(bstm_block_id id, vcl_string type, bstm_data_base* replacement);

    //: dumps writeable data to disk
    virtual void write_to_disk();

    //: to string method returns a string describing the cache's current state
    vcl_string to_string();

    //: delete all the memory, caution: make sure to call write to disc methods not to loose writable data
    virtual void clear_cache();

  private:

    //: hidden constructor (private so it cannot be called -- forces the class to be singleton)
    bstm_lru_cache(bstm_scene_sptr scene);

    //: hidden destructor (private so it cannot be called -- forces the class to be singleton)
    ~bstm_lru_cache();

    //: keep a map of bstm_block pointers
    vcl_map<bstm_block_id, bstm_block*> cached_blocks_;

    //: keep a map of bstm_time_block pointers
    vcl_map<bstm_block_id, bstm_time_block*> cached_time_blocks_;

    //: keeps one copy of each type of cached data
    vcl_map<vcl_string, vcl_map<bstm_block_id, bstm_data_base*> > cached_data_;

    //: directory where blocks are found
    vcl_string scene_dir_;

    // ---------Helper Methods --------------------------------------------------

    //: helper method returns a reference to correct data map (ensures one exists)
    vcl_map<bstm_block_id, bstm_data_base*>& cached_data_map(vcl_string prefix);

    //: helper method determines if this block is
    bool is_valid_id(bstm_block_id);
    // --------------------------------------------------------------------------
};

//: shows elements in cache
vcl_ostream& operator<<(vcl_ostream &s, bstm_lru_cache& scene);

#endif // bstm_lru_cache_h_
