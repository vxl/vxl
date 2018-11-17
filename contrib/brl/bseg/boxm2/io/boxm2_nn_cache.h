#ifndef boxm2_nn_cache_h_
#define boxm2_nn_cache_h_
//:
// \file
#include <iostream>
#include <boxm2/io/boxm2_cache1.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: an example cache that loads in the nearest neighbors of the requested block (asynchronously)
class boxm2_nn_cache : public boxm2_cache1
{
  public:

    //: construct with directory and scene dimensions (blocknum), by default use the local filesystem
    boxm2_nn_cache(boxm2_scene* scene, BOXM2_IO_FS_TYPE fs=LOCAL);
    ~boxm2_nn_cache() override;

    //: returns block pointer to block specified by ID
    boxm2_block* get_block(boxm2_block_id id) override;

    //: returns data_base pointer
    //  (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
    boxm2_data_base* get_data_base(boxm2_block_id, std::string type, std::size_t num_bytes=0, bool read_only = true) override;

    //: returns a data_base pointer which is initialized to the default value of the type.
    //  If a block for this type exists on the cache, it is removed and replaced with the new one
    //  This method does not check whether a block of this type already exists on the disk nor writes it to the disk
    boxm2_data_base* get_data_base_new(boxm2_block_id id, std::string type,std::size_t num_bytes=0,  bool read_only = true) override;

    //: deletes a data item from the cpu cache
    void remove_data_base(boxm2_block_id, std::string type) override;
    void replace_data_base(boxm2_block_id, std::string type, boxm2_data_base* replacement) override;

    //: returns data pointer to data block specified by ID
    template <boxm2_data_type T>
    boxm2_data<T>* get_data(boxm2_block_id id);

    //: to string method returns a string describing the cache's current state
    std::string to_string();

    //: dumps writeable data onto disk
    // \todo not yet implemented
    void write_to_disk() override { std::cerr << "Not yet implemented!!!\n"; }

    //: disable the write process
    // \todo not yet implemented
    void disable_write() override { std::cerr << "Not yet implemented!!!\n"; }

     //: delete all the memory
     //  Caution: make sure to call write to disk methods not to loose writable data
    // \todo not yet implemented
    void clear_cache() override { std::cerr << "Not yet implemented!!!\n"; }

  private:

    //: keep a map of boxm2_block pointers (size will be limited to 9 blocks)
    std::map<boxm2_block_id, boxm2_block*> cached_blocks_;

    //: keeps one copy of each type of cached data
    std::map<std::string, std::map<boxm2_block_id, boxm2_data_base*> > cached_data_;

    //: directory where blocks are found
    std::string scene_dir_;

    // ---------Helper Methods --------------------------------------------------

    //: private update cache method (very simple)
    void update_block_cache(boxm2_block* blk);

    //: private update data generic
    void update_data_base_cache(boxm2_data_base*, const std::string& type);

    //: private update block cache method
    template <boxm2_data_type T>
    void update_data_cache(boxm2_data_base* dat);

    //: private helper that reads finished async jobs into the cache
    void finish_async_blocks();

    //: finish async data
    void finish_async_data(const std::string& data_type);

    //: helper method returns a reference to correct data map (ensures one exists)
    std::map<boxm2_block_id, boxm2_data_base*>& cached_data_map(const std::string& prefix);

    //: returns a list of neighbors for a given ID
    std::vector<boxm2_block_id> get_neighbor_list(const boxm2_block_id& center);

    //: helper method determines if this block is valid
    bool is_valid_id(const boxm2_block_id&);
    // --------------------------------------------------------------------------
};

//: shows elements in cache
std::ostream& operator<<(std::ostream &s, boxm2_nn_cache& scene);


//: get data by type and id
template<boxm2_data_type T>
boxm2_data<T>* boxm2_nn_cache::get_data(boxm2_block_id id)
{
  return (boxm2_data<T>*) this->get_data_base(id, boxm2_data_traits<T>::prefix());
}

#endif // boxm2_nn_cache_h_
