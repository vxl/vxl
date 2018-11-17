#ifndef bstm_opencl_cache_h
#define bstm_opencl_cache_h
//:
// \file
// \brief
#include <vector>
#include <iostream>
#include <list>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/bstm_data_traits.h>
#include <bstm/basic/bstm_block_id.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/io/bstm_lru_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

#include <bstm/ocl/bstm_scene_info.h>

//: bstm_dumb_cache - example realization of abstract cache class
class bstm_opencl_cache: public vbl_ref_count
{
  public:
    typedef vnl_vector_fixed<unsigned char, 8> uchar8;
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  public:
    bstm_opencl_cache(const bstm_scene_sptr& scene,
                      const bocl_device_sptr& device);
    ~bstm_opencl_cache() override
      {
      if (cpu_cache_) cpu_cache_ = nullptr;
      this->clear_cache();
      }

    void set_context(cl_context& context);
    bstm_cache_sptr get_cpu_cache() { return cpu_cache_; }
    bocl_device_sptr get_device() { return device_; }
    bstm_scene_sptr get_scene()  { return scene_; }

    //: returns block pointer to block specified by ID
    bocl_mem* get_block(const bstm_block_id& id);

    //: returns pointer to time block specified by ID
    bocl_mem* get_time_block(const bstm_block_id& id);

    //: get scene info in bocl_mem*
    bocl_mem* get_block_info(const bstm_block_id& id);
    bocl_mem* loaded_block_info() { return block_info_; }
    bocl_mem* loaded_time_block_info() { return block_info_t_; }

    //: returns data pointer to data block specified by ID
    template<bstm_data_type T>
    bocl_mem* get_data(bstm_block_id, std::size_t num_bytes=0, bool read_only = true);
    bocl_mem* get_data(const bstm_block_id&, const std::string& type, std::size_t num_bytes=0, bool read_only = true);

    template<bstm_data_type T>
    bocl_mem* get_data_new(bstm_block_id, std::size_t num_bytes=0, bool read_only = true);
    bocl_mem* get_data_new(const bstm_block_id& id, const std::string& type, std::size_t num_bytes = 0, bool read_only = true);

    //: returns a flat bocl_mem of a certain size
    bocl_mem* alloc_mem(std::size_t num_bytes, void* cpu_buff=nullptr, std::string id="bocl_mem in pool");
    void      unref_mem(bocl_mem* mem);
    void      free_mem(bocl_mem* mem);
    void      free_mem_pool();

    //: empties out cache, deletes all bocl_mem*s
    bool clear_cache();

    //: returns num bytes in mem pool only
    std::size_t bytes_in_mem_pool();

    //: returns num bytes in cache
    std::size_t bytes_in_cache();

    //: deep_replace data replaces not only the current data on the gpu cached, but pushes a block to the cpu cache
    void deep_replace_data(const bstm_block_id& id, const std::string& type, bocl_mem* mem, bool read_only=true);

    //: deep_remove_data removes this id and type from ocl cache, as well as the cpu cache
    void deep_remove_data(const bstm_block_id& id, const std::string& type, bool write_out=true);

    //: shallow_remove_data removes data with id and type from ocl cache only
    void shallow_remove_data(const bstm_block_id& id, std::string type);

    //: to string method prints out LRU order
    std::string to_string();

    //: get max mem size
    unsigned long max_memory_in_cache() { return maxBytesInCache_; }

  private:

    //: scene this cache is operating on
    bstm_scene_sptr scene_;

    //: keep a pointer to the CPU cache
    bstm_cache_sptr cpu_cache_;

    //: maximum number of blocks this cache will allow (eventually this will become smart)
    void lru_push_front( const bstm_block_id& id );
    bool lru_remove_last(bstm_block_id& id); //removes all data and block with this ID. returns false if lru is empty
    std::list<bstm_block_id> lru_order_;
    unsigned long bytesInCache_;
    unsigned long maxBytesInCache_;

    ////////////////////////////////////////////////////////////////////////////
    // bocl_mem objects
    ////////////////////////////////////////////////////////////////////////////
    //: scene/block info for current blocks
    bocl_mem* block_info_;

    //: scene/block info for current time blocks
    bocl_mem* block_info_t_;

    //: dumb cache keeps one cached block, the last one used.
    std::map<bstm_block_id, bocl_mem*> cached_blocks_;

    //: dumb cache keeps one cached block, the last one used.
    std::map<bstm_block_id, bocl_mem*> cached_time_blocks_;

    //: keeps one copy of each type of cached data
    std::map<std::string, std::map<bstm_block_id, bocl_mem*> > cached_data_;

    //: helper method for finding the right data map
    std::map<bstm_block_id, bocl_mem*>& cached_data_map(const std::string& prefix);

    //: memory cache - caches various non model memory (images, some aux data)
    // Does not account for block/data_base data
    std::map<bocl_mem*, std::size_t> mem_pool_;

    ////////////////////////////////////////////////////////////////////////////
    // opencl objects
    ////////////////////////////////////////////////////////////////////////////
    //use this for the context
    bocl_device_sptr device_;

    //: opencl context to use for writing to buffers
    cl_context*       context_;

    //: opencl command queue to use for writing to buffers
    cl_command_queue* queue_;
    cl_command_queue q_;
};

typedef vbl_smart_ptr<bstm_opencl_cache> bstm_opencl_cache_sptr;

//: get data by type and id
template<bstm_data_type T>
bocl_mem* bstm_opencl_cache::get_data(bstm_block_id id, std::size_t num_bytes, bool read_only)
{
  return get_data(id, bstm_data_traits<T>::prefix(), num_bytes, read_only);
}

//: get new data by type and id
template<bstm_data_type T>
bocl_mem* bstm_opencl_cache::get_data_new(bstm_block_id id, std::size_t num_bytes, bool read_only)
{
  return get_data_new(id, bstm_data_traits<T>::prefix(), num_bytes, read_only);
}

//: Binary write bstm_cache  to stream
void vsl_b_write(vsl_b_ostream& os, bstm_opencl_cache const& scene);
void vsl_b_write(vsl_b_ostream& os, const bstm_opencl_cache* &p);
void vsl_b_write(vsl_b_ostream& os, bstm_opencl_cache_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, bstm_opencl_cache_sptr const& sptr);

//: Binary load bstm_cache  from stream.
void vsl_b_read(vsl_b_istream& is, bstm_opencl_cache &scene);
void vsl_b_read(vsl_b_istream& is, bstm_opencl_cache* p);
void vsl_b_read(vsl_b_istream& is, bstm_opencl_cache_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, bstm_opencl_cache_sptr const& sptr);


#endif // bstm_opencl_cache_h
