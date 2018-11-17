#ifndef boxm2_lru_cache_h_
#define boxm2_lru_cache_h_
//:
// \file
// \brief boxm2_lru_cache (least recently used) is a singleton, derived from abstract class boxm2_cache

#include <iostream>
#include <boxm2/io/boxm2_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define MAX_BYTES 1024*1024*1024*4 // 4 gigs of memory is max...
struct ltstr1
{
  bool operator()(const boxm2_scene_sptr  s1, const boxm2_scene_sptr  s2) const
  {
    return s1.ptr() < s2.ptr();
  }
};
//: A cache that keeps the most recently used blocks and data, while kicking out the least recently used blocks and data to make more room.
//  Currently just stores blocks in a map, not caring about space
//  \todo implement with timestamped maps.
class boxm2_lru_cache : public boxm2_cache
{
  public:

    //: create function used instead of constructor
    static void create(const boxm2_scene_sptr& scene, BOXM2_IO_FS_TYPE fs_type=LOCAL);

    //: returns block pointer to block specified by ID
    boxm2_block* get_block(boxm2_scene_sptr & scene, boxm2_block_id id) override;

    //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
    boxm2_data_base* get_data_base(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type, std::size_t num_bytes=0, bool read_only = true) override;

    //: returns a data_base pointer which is initialized to the default value of the type.
    //  If a block for this type exists on the cache, it is removed and replaced with the new one.
    //  This method does not check whether a block of this type already exists on the disc nor writes it to the disc
    boxm2_data_base* get_data_base_new(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type, std::size_t num_bytes=0, bool read_only = true) override;

    //: removes data from this cache (may or may not write to disk first)
    void remove_data_base(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type, bool write_out=true) override;

    //: replaces a database in the cache, deletes it
    void replace_data_base(boxm2_scene_sptr & scene, boxm2_block_id id, std::string type, boxm2_data_base* replacement) override;

    //: dumps writeable data to disk
    void write_to_disk() override;

    //: dumps writeable data for specified scene to disk
    void write_to_disk(boxm2_scene_sptr & scene) override;

    //: add a new scene to the cache
    bool add_scene(boxm2_scene_sptr & scene) override;

    //: remove an existing scene from the cache
    bool remove_scene(boxm2_scene_sptr & scene) override;

    //: to string method returns a string describing the cache's current state
    std::string to_string();

    //: delete all the memory, caution: make sure to call write to disc methods not to loose writable data
    void clear_cache() override;

    //: return the list of scenes with any data in the cache
    std::vector<boxm2_scene_sptr> get_scenes() override;

  private:

    //: hidden constructor (private so it cannot be called -- forces the class to be singleton)
    boxm2_lru_cache(const boxm2_scene_sptr& scene, BOXM2_IO_FS_TYPE=LOCAL);

    //: hidden destructor (private so it cannot be called -- forces the class to be singleton)
    ~boxm2_lru_cache() override;

    //: keep a map of boxm2_block pointers (size will be limited to 9 blocks
    std::map< boxm2_scene_sptr, std::map<boxm2_block_id, boxm2_block*>,ltstr1 > cached_blocks_;

    //: keeps one copy of each type of cached data
    std::map< boxm2_scene_sptr, std::map<std::string, std::map<boxm2_block_id, boxm2_data_base*> >,ltstr1 > cached_data_;


    // ---------Helper Methods --------------------------------------------------

    //: helper method returns a reference to correct data map (ensures one exists)
    std::map<boxm2_block_id, boxm2_data_base*>& cached_data_map(boxm2_scene_sptr & scene, const std::string& prefix);

    //: helper method determines if this block is
    bool is_valid_id(boxm2_scene_sptr & scene, const boxm2_block_id&);
    // --------------------------------------------------------------------------
};

//: shows elements in cache
std::ostream& operator<<(std::ostream &s, boxm2_lru_cache& scene);

#endif // boxm2_lru_cache_h_
