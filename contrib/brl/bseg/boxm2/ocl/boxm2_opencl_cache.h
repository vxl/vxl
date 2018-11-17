#ifndef boxm2_opencl_cache_h
#define boxm2_opencl_cache_h
//:
// \file
// \brief boxm2_opencl_scene_streamer assists the processor in streaming blocks
#include <vector>
#include <list>
#include <iostream>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <brdb/brdb_value_sptr.h>
#ifdef DEBUG
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//: boxm2_dumb_cache - example realization of abstract cache class
class boxm2_opencl_cache: public vbl_ref_count
{
  public:
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  public:
    boxm2_opencl_cache(const bocl_device_sptr& device);
    ~boxm2_opencl_cache() override { if (cpu_cache_) cpu_cache_ = nullptr;
                            this->clear_cache();
                          }

    void set_context(cl_context& context);
    boxm2_cache_sptr get_cpu_cache() { return cpu_cache_; }
    bocl_device_sptr get_device() { return device_; }
    //boxm2_scene_sptr get_scene()  { return scene_; }

    //: returns block pointer to block specified by ID
    bocl_mem* get_block(boxm2_scene_sptr scene, boxm2_block_id id);

    //: get scene info in bocl_mem*
    bocl_mem* get_block_info(boxm2_scene_sptr scene, const boxm2_block_id& id);
    bocl_mem* get_copy_of_block_info(const boxm2_scene_sptr& scene, const boxm2_block_id& id);
    bocl_mem* loaded_block_info() { return block_info_; }

    //: returns data pointer to data block specified by ID
    template<boxm2_data_type T>
    bocl_mem* get_data(boxm2_scene_sptr scene,boxm2_block_id, std::size_t num_bytes=0, bool read_only = true,std::string ident = "");
    bocl_mem* get_data(boxm2_scene_sptr scene,boxm2_block_id, const std::string& type, std::size_t num_bytes=0, bool read_only = true);

    template<boxm2_data_type T>
        bocl_mem* get_data_new(boxm2_scene_sptr scene,boxm2_block_id, std::size_t num_bytes=0, bool read_only = true);
    bocl_mem* get_data_new(boxm2_scene_sptr scene,boxm2_block_id id, const std::string& type, std::size_t num_bytes = 0, bool read_only = true);

    //: returns a flat bocl_mem of a certain size
    bocl_mem* alloc_mem(std::size_t num_bytes, void* cpu_buff=nullptr, std::string id="bocl_mem in pool");
    void      unref_mem(bocl_mem* mem);
    void      free_mem(bocl_mem* mem);
    void      free_mem_pool();

    //: empties out cache, deletes all bocl_mem*s
    bool clear_cache();
    //: calls clFinish so halts cpp code
    bool finish_queue();

    //: check if max_bytes_in_cache is hit and call clear_cache() if necessary
    bool clear_cache_if_necessary();

    //: returns num bytes in cache
    std::size_t bytes_in_cache();

    //: deep_replace data replaces not only the current data on the gpu cached, but pushes a block to the cpu cache
    void deep_replace_data(boxm2_scene_sptr scene,const boxm2_block_id& id, const std::string& type, bocl_mem* mem, bool read_only=true);

    //: deep_remove_data removes this id and type from ocl cache, as well as the cpu cache
    void deep_remove_data(boxm2_scene_sptr scene,const boxm2_block_id& id, const std::string& type, bool write_out=true);

    //: shallow_remove_data removes data with id and type from ocl cache only
    void shallow_remove_data(const boxm2_scene_sptr& scene,const boxm2_block_id& id, std::string type);

    //: removes block data with id from opencl cache only, caution if block data changed: this method does not enqueu a read event before deletion!
    void shallow_remove_block(const boxm2_scene_sptr& scene,const boxm2_block_id& id);

    //: to string method prints out LRU order
    std::string to_string();

  private:

    ////: scene this cache is operating on
    //boxm2_scene_sptr scene_;

    //: keep a pointer to the CPU cache
    boxm2_cache_sptr cpu_cache_;

    //: maximum number of blocks this cache will allow (eventually this will become smart)
    void lru_push_front( std::pair<boxm2_scene_sptr, boxm2_block_id>  scene_id_pair );
    bool lru_remove_last(std::pair<boxm2_scene_sptr, boxm2_block_id> & scene_id_pair); //removes all data and block with this ID. returns false if lru is empty
    std::list<std::pair<boxm2_scene_sptr,boxm2_block_id> > lru_order_;
    unsigned int maxBlocksInCache;
    std::size_t bytesInCache_;
    std::size_t maxBytesInCache_;

    ////////////////////////////////////////////////////////////////////////////
    // bocl_mem objects
    ////////////////////////////////////////////////////////////////////////////
    //: scene/block info for current blocks
    bocl_mem* block_info_;

    //: dumb cache keeps one cached block, the last one used.
    std::map<boxm2_scene_sptr, std::map<boxm2_block_id, bocl_mem*>,ltstr1 > cached_blocks_;

    //: keeps one copy of each type of cached data
    std::map<boxm2_scene_sptr, std::map<std::string, std::map<boxm2_block_id, bocl_mem*> >,ltstr1 > cached_data_;

    //: helper method for finding the right data map
    std::map<boxm2_block_id, bocl_mem*>& cached_data_map(const boxm2_scene_sptr& scene, const std::string& prefix);

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

typedef vbl_smart_ptr<boxm2_opencl_cache> boxm2_opencl_cache_sptr;

//: get data by type and id
template<boxm2_data_type T>
bocl_mem* boxm2_opencl_cache::get_data(boxm2_scene_sptr scene, boxm2_block_id id, std::size_t num_bytes, bool read_only,std::string ident )
{
  return get_data(scene, id, boxm2_data_traits<T>::prefix(ident), num_bytes, read_only);
}

//: get new data by type and id
template<boxm2_data_type T>
bocl_mem* boxm2_opencl_cache::get_data_new(boxm2_scene_sptr scene,boxm2_block_id id, std::size_t num_bytes, bool read_only)
{
  return get_data_new(scene, id, boxm2_data_traits<T>::prefix(), num_bytes, read_only);
}

//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_opencl_cache const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_opencl_cache* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_opencl_cache_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, boxm2_opencl_cache_sptr const& sptr);

//: Binary load boxm2_cache  from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache* p);
void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_opencl_cache_sptr const& sptr);


#endif // boxm2_opencl_cache_h
