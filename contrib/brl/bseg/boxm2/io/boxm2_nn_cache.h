#ifndef boxm2_nn_cache_h_
#define boxm2_nn_cache_h_
//:
// \file
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <vcl_iostream.h>

//: an example cache that loads in the nearest neighbors of the requested block (asynchronously)
class boxm2_nn_cache : public boxm2_cache
{
  public:

    //: construct with directory and scene dimensions (blocknum), by default use the local filesystem
    boxm2_nn_cache(boxm2_scene* scene, BOXM2_IO_FS_TYPE fs=LOCAL);
    ~boxm2_nn_cache();

    //: returns block pointer to block specified by ID
    virtual boxm2_block* get_block(boxm2_block_id id);

    //: returns data_base pointer
    //  (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
    virtual boxm2_data_base* get_data_base(boxm2_block_id, vcl_string type, vcl_size_t num_bytes=0, bool read_only = true);

    //: returns a data_base pointer which is initialized to the default value of the type.
    //  If a block for this type exists on the cache, it is removed and replaced with the new one
    //  This method does not check whether a block of this type already exists on the disk nor writes it to the disk
    virtual boxm2_data_base* get_data_base_new(boxm2_block_id id, vcl_string type,vcl_size_t num_bytes=0,  bool read_only = true);

    //: deletes a data item from the cpu cache
    virtual void remove_data_base(boxm2_block_id, vcl_string type);
    virtual void replace_data_base(boxm2_block_id, vcl_string type, boxm2_data_base* replacement);

    //: returns data pointer to data block specified by ID
    template <boxm2_data_type T>
    boxm2_data<T>* get_data(boxm2_block_id id);

    //: to string method returns a string describing the cache's current state
    vcl_string to_string();

    //: dumps writeable data onto disk
    // \todo not yet implemented
    virtual void write_to_disk() { vcl_cerr << "Not yet implemented!!!\n"; }

    //: disable the write process
    // \todo not yet implemented
    virtual void disable_write() { vcl_cerr << "Not yet implemented!!!\n"; }

     //: delete all the memory
     //  Caution: make sure to call write to disk methods not to loose writable data
    // \todo not yet implemented
    virtual void clear_cache() { vcl_cerr << "Not yet implemented!!!\n"; }

  private:

    //: keep a map of boxm2_block pointers (size will be limited to 9 blocks)
    vcl_map<boxm2_block_id, boxm2_block*> cached_blocks_;

    //: keeps one copy of each type of cached data
    vcl_map<vcl_string, vcl_map<boxm2_block_id, boxm2_data_base*> > cached_data_;

    //: directory where blocks are found
    vcl_string scene_dir_;

    // ---------Helper Methods --------------------------------------------------

    //: private update cache method (very simple)
    void update_block_cache(boxm2_block* blk);

    //: private update data generic
    void update_data_base_cache(boxm2_data_base*, vcl_string type);

    //: private update block cache method
    template <boxm2_data_type T>
    void update_data_cache(boxm2_data_base* dat);

    //: private helper that reads finished async jobs into the cache
    void finish_async_blocks();

    //: finish async data
    void finish_async_data(vcl_string data_type);

    //: helper method returns a reference to correct data map (ensures one exists)
    vcl_map<boxm2_block_id, boxm2_data_base*>& cached_data_map(vcl_string prefix);

    //: returns a list of neighbors for a given ID
    vcl_vector<boxm2_block_id> get_neighbor_list(boxm2_block_id center);

    //: helper method determines if this block is valid
    bool is_valid_id(boxm2_block_id);
    // --------------------------------------------------------------------------
};

//: shows elements in cache
vcl_ostream& operator<<(vcl_ostream &s, boxm2_nn_cache& scene);


//: get data by type and id
template<boxm2_data_type T>
boxm2_data<T>* boxm2_nn_cache::get_data(boxm2_block_id id)
{
  return (boxm2_data<T>*) this->get_data_base(id, boxm2_data_traits<T>::prefix());
}

#endif // boxm2_nn_cache_h_
