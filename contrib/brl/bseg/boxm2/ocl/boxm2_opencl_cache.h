#ifndef boxm2_opencl_cache_h
#define boxm2_opencl_cache_h
//:
// \file
// \brief boxm2_opencl_scene_streamer assists the processor in streaming blocks
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
#include <vcl_vector.h>
#ifdef DEBUG
#include <vcl_iostream.h>
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
    boxm2_opencl_cache(boxm2_scene_sptr scene,
                       bocl_device_sptr device,
                       int maxBlocks=1);
    ~boxm2_opencl_cache() { this->clear_cache(); }
    
    
    void set_context(cl_context& context); 
    boxm2_cache_sptr get_cpu_cache() { return cpu_cache_; }

    //: returns block pointer to block specified by ID
    bocl_mem* get_block(boxm2_block_id id);

    //: get scene info in bocl_mem*
    bocl_mem* get_block_info(boxm2_block_id id);
    bocl_mem* loaded_block_info() { return block_info_; }

    //: returns data pointer to data block specified by ID
    template<boxm2_data_type T>
    bocl_mem* get_data(boxm2_block_id, vcl_size_t num_bytes=0, bool read_only = true);
    bocl_mem* get_data(boxm2_block_id, vcl_string type, vcl_size_t num_bytes=0, bool read_only = true);

    //: empties out cache, deletes all bocl_mem*s
    bool clear_cache();
    
    //: returns num bytes in cache
    long bytes_in_cache(); 

    //: deep_replace data replaces not only the current data on the gpu cached, but pushes a block to the cpu cache
    void deep_replace_data(boxm2_block_id id, vcl_string type, bocl_mem* mem);

  private:

    //: scene this cache is operating on
    boxm2_scene_sptr scene_;

    //: keep a pointer to the CPU cache
    boxm2_cache_sptr cpu_cache_;

    //: maximum number of blocks this cache will allow (eventually this will become smart)
    int maxBlocksInCache;
    long bytesInCache_; 
    long maxBytesInCache_; 

    ////////////////////////////////////////////////////////////////////////////
    // bocl_mem objects
    ////////////////////////////////////////////////////////////////////////////
    //: scene/block info for current blocks
    bocl_mem* block_info_;

    //: dumb cache keeps one cached block, the last one used.
    vcl_map<boxm2_block_id, bocl_mem*> cached_blocks_;

    //: keeps one copy of each type of cached data
    vcl_map<vcl_string, vcl_map<boxm2_block_id, bocl_mem*> > cached_data_;

    //: helper method for finding the right data map
    vcl_map<boxm2_block_id, bocl_mem*>& cached_data_map(vcl_string prefix);

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
bocl_mem* boxm2_opencl_cache::get_data(boxm2_block_id id, vcl_size_t num_bytes, bool read_only)
{
  return get_data(id, boxm2_data_traits<T>::prefix(), num_bytes, read_only);
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

//////////////////////////////
// OLD SINGLE BLOCK CACHE IMPLEMENTATION
////////////////////////////////
#if 0
#ifndef boxm2_opencl_cache_h
#define boxm2_opencl_cache_h
//:
// \file
// \brief boxm2_opencl_scene_streamer assists the processor in streaming blocks
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/ocl/boxm2_opencl_cache_base.h>
#include <brdb/brdb_value_sptr.h>
#include <vcl_vector.h>
#ifdef DEBUG
#include <vcl_iostream.h>
#endif

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class boxm2_opencl_cache: public vbl_ref_count, public boxm2_opencl_cache_base
{
  public:
    boxm2_opencl_cache(boxm2_scene_sptr scene,
                       bocl_device_sptr device);
    boxm2_opencl_cache(boxm2_cache* cpu_cache,
                       cl_context* context,
                       cl_command_queue* queue,
                       boxm2_scene* scene);
    ~boxm2_opencl_cache() { this->clear_cache(); }

    //: empties out cache, deletes all bocl_mem*s
    bool clear_cache();
    
    //: returns block pointer to block specified by ID
    bocl_mem* get_block(boxm2_block_id id);

    //: get scene info in bocl_mem*
    bocl_mem* loaded_block_info() { return block_info_; }

    ////////////////////////////////////////////////////////////////////
    //deprecated when new refine is implemented
    bocl_mem* get_loaded_tree_ptrs();
    bocl_mem* get_loaded_trees_per_buffer();
    bocl_mem* get_loaded_mem_ptrs();
    ////////////////////////////////////////////////////////////////////

    //: returns data pointer to data block specified by ID
    template<boxm2_data_type T>
    bocl_mem* get_data(boxm2_block_id, vcl_size_t num_bytes=0);
    bocl_mem* get_data(boxm2_block_id, vcl_string type, vcl_size_t num_bytes=0);

    void deep_remove_data(boxm2_block_id id, vcl_string type);

    //: deep_replace data replaces not only the current data on the gpu cached, but pushes a block to the cpu cache
    void deep_replace_data(boxm2_block_id id, vcl_string type, bocl_mem* mem);
    

  private:

    //: scene this cache is operating on
    boxm2_scene_sptr scene_;

    //: keep a pointer to the CPU cache
    boxm2_cache* cpu_cache_;

    //: id of block currently in cache
    boxm2_block_id loaded_;

    //: ids of data
    vcl_map<vcl_string, boxm2_block_id> loaded_data_;

    ////////////////////////////////////////////////////////////////////////////
    // bocl_mem objects
    ////////////////////////////////////////////////////////////////////////////
    // ALL OF THE BOCL_MEM pointers will create Pinned host memory, which allows
    // the gpu to overlap a kernel execution

    //: scene/block info for current block
    bocl_mem* block_info_;
    bocl_mem* tree_ptrs_;
    bocl_mem* trees_per_buffer_;
    bocl_mem* mem_ptrs_;

    //: dumb cache keeps one cached block, the last one used.
    bocl_mem* cached_block_;

    //: keeps one copy of each type of cached data
    vcl_map<vcl_string, bocl_mem* > cached_data_;

    ////////////////////////////////////////////////////////////////////////////
    // end of bocl_mem objects
    ////////////////////////////////////////////////////////////////////////////

    //: opencl context to use for writing to buffers
    cl_context*       context_;
    //: opencl command queue to use for writing to buffers
    cl_command_queue* queue_;
    cl_command_queue q_;
};

typedef vbl_smart_ptr<boxm2_opencl_cache> boxm2_opencl_cache_sptr;

//: get data by type and id
template<boxm2_data_type T>
bocl_mem* boxm2_opencl_cache::get_data(boxm2_block_id id, vcl_size_t num_bytes)
{
  return get_data(id, boxm2_data_traits<T>::prefix(), num_bytes);
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
#endif
