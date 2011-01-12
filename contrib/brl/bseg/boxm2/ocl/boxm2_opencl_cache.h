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
#include <brdb/brdb_value_sptr.h>
#include <vcl_vector.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_mem.h>

//: boxm2_dumb_cache - example realization of abstract cache class
class boxm2_opencl_cache
{
  public:
    boxm2_opencl_cache(boxm2_cache* cpu_cache,
                       cl_context* context,
                       cl_command_queue* queue,
                       boxm2_scene* scene);
    ~boxm2_opencl_cache();

    //: returns block poitner to block specified by ID
    bocl_mem* get_block(boxm2_block_id id);

    //: get scene info in bocl_mem*
    bocl_mem* loaded_block_info() { return block_info_; }

    //: returns data pointer to data block specified by ID
    template<boxm2_data_type T>
    bocl_mem* get_data(boxm2_block_id);

  private:

    //: scene this cache is operating on
    boxm2_scene* scene_;

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
};

//: get data by type and id
template<boxm2_data_type T>
bocl_mem* boxm2_opencl_cache::get_data(boxm2_block_id id)
{
  //make sure that the data is in the
  if (loaded_data_[boxm2_data_traits<T>::prefix()] == id)
    return cached_data_[boxm2_data_traits<T>::prefix()];

  //otherwise get the data block from cpu cache
  if ( cached_data_.find(boxm2_data_traits<T>::prefix()) != cached_data_.end())
  {
    //vcl_cout<<"ocl_cache release memory for :"<<boxm2_data_traits<T>::prefix()<<vcl_endl;
    //release existing memory
    bocl_mem* toDelete = cached_data_[boxm2_data_traits<T>::prefix()];
    delete toDelete;
    cached_data_[boxm2_data_traits<T>::prefix()] = 0;
  }

  //create new memory
  boxm2_data_base* data_base = cpu_cache_->get_data_base(id, boxm2_data_traits<T>::prefix());
  loaded_data_[boxm2_data_traits<T>::prefix()] = id;
  bocl_mem* data = new bocl_mem(*context_, data_base->data_buffer(), data_base->buffer_length(), boxm2_data_traits<T>::prefix());
  data->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  cached_data_[boxm2_data_traits<T>::prefix()] = data;
  
  vcl_cout<<"Data mem "<<boxm2_data_traits<T>::prefix()<<" length: "
                       <<data_base->buffer_length()<<", cellsize: "
                       <<boxm2_data_traits<T>::datasize()<<vcl_endl;
  
  return data;
}


#endif
