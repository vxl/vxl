#ifndef boxm2_nn_cache_h_
#define boxm2_nn_cache_h_
//:
// \file
#include <boxm2/boxm2_cache.h>

//: an example cache that loads in the nearest neighbors of the requested block (asynchronously)
class boxm2_nn_cache : boxm2_cache
{
  public:
    boxm2_nn_cache(vcl_string dir, vgl_vector_3d<int> bnum) : block_num_(bnum), scene_dir_(dir) {}
    ~boxm2_nn_cache();

    //: returns block poitner to block specified by ID
    boxm2_block* get_block(boxm2_block_id id);

    //: returns data pointer to data block specified by ID
    template <boxm2_data_type T>
    boxm2_data<T>* get_data(boxm2_block_id id);

  private:

    //: private update cache method (very simple)
    void update_block_cache(boxm2_block* blk);

    //: private update block cache method
    template <boxm2_data_type T>
    void update_data_cache(boxm2_data<T>* dat);

    //: private helper that reads finished async jobs into the cache
    void finish_async_blocks();

    //: helper method determines if this block is
    bool is_valid_id(boxm2_block_id);

    //: keep a map of boxm2_block pointers (size will be limited to 9 blocks
    vcl_map<boxm2_block_id, boxm2_block*> cached_blocks_;

    //: keeps one copy of each type of cached data
    vcl_map<vcl_string, vcl_map<boxm2_block_id, boxm2_data_base*> > cached_data_;

    //: dimensions of the scene
    vgl_vector_3d<int> block_num_;

    //: directory where blocks are found
    vcl_string scene_dir_;
};


//: get data by type and id
template<boxm2_data_type T>
boxm2_data<T>* boxm2_nn_cache::get_data(boxm2_block_id id)
{
#if 0
  if ( cached_data_.find(boxm2_data_traits<T>::prefix()) != cached_data_.end() )
  {
    if (cached_data_[boxm2_data_traits<T>::prefix()]->block_id() == id)
      return (boxm2_data<T>*) cached_data_[boxm2_data_traits<T>::prefix()];
  }

  //otherwise load it from disk
  boxm2_data<T>* loaded = boxm2_sio_mgr::load_block_data<T>(scene_dir_,id);
  this->update_data_cache<T>(loaded);
  return loaded;
#endif // 0
}

//: update data cache by type
template<boxm2_data_type T>
void boxm2_nn_cache::update_data_cache(boxm2_data<T>* dat)
{
#if 0
  vcl_map<vcl_string, boxm2_data_base* >::iterator iter;
  iter = cached_data_.find(boxm2_data_traits<T>::prefix());
  if ( iter != cached_data_.end() )
  {
    boxm2_data_base* old = (*iter).second;
    if (old) delete old;
  }
  cached_data_[boxm2_data_traits<T>::prefix()] = dat;
#endif // 0
}

#endif // boxm2_nn_cache_h_
