#ifndef boxm2_dumb_cache_h_
#define boxm2_dumb_cache_h_
//:
// \file
#include <boxm2/io/boxm2_cache.h>
#include <vcl_iostream.h>

//: boxm2_dumb_cache - example realization of abstract cache class
class boxm2_dumb_cache : public boxm2_cache
{
  public:
    boxm2_dumb_cache(boxm2_scene* scene);
    ~boxm2_dumb_cache();

    //: returns block pointer to block specified by ID
    virtual boxm2_block* get_block(boxm2_block_id id);

    //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
    virtual boxm2_data_base* get_data_base(boxm2_block_id, vcl_string type, vcl_size_t num_bytes=0, bool read_only = true);

    //: deletes data from dumb cache
    virtual void remove_data_base(boxm2_block_id, vcl_string type);
    virtual void replace_data_base(boxm2_block_id id, vcl_string type, boxm2_data_base* replacement);

    //: returns data pointer to data block specified by ID
    template <boxm2_data_type T>
    boxm2_data<T>* get_data(boxm2_block_id id);

    //: dumps writeable data onto disk
    // \todo not yet implemented
    virtual void write_to_disk() { vcl_cerr << "write_to_disk() not yet implemented!!!\n"; }

    //: disable the write process
    // \todo not yet implemented
    virtual void disable_write() { vcl_cerr << "disable_write() not yet implemented!!!\n"; }

    //: delete all the memory
    virtual void clear_cache() { vcl_cerr << "clear_cache() not yet implemented!!!\n"; }
  private:

    //: private update cache method (very simple)
    void update_block_cache(boxm2_block* blk);

    //: private update method (very simple)
    void update_data_base_cache(boxm2_data_base* dat, vcl_string type);

    //: private update block cache method
    template <boxm2_data_type T>
    void update_data_cache(boxm2_data<T>* dat);

    //: dumb cache keeps one cached block, the last one used.
    boxm2_block* cached_block_;

    //: keeps one copy of each type of cached data
    vcl_map<vcl_string, boxm2_data_base* > cached_data_;

    //: directory where blocks are found
    vcl_string scene_dir_;
};


//: get data by type and id
template<boxm2_data_type T>
boxm2_data<T>* boxm2_dumb_cache::get_data(boxm2_block_id id)
{
  if ( cached_data_.find(boxm2_data_traits<T>::prefix()) != cached_data_.end() )
  {
    if (cached_data_[boxm2_data_traits<T>::prefix()]->block_id() == id)
      return (boxm2_data<T>*) cached_data_[boxm2_data_traits<T>::prefix()];
  }

  //otherwise load it from disk
  boxm2_data<T>* loaded = boxm2_sio_mgr::load_block_data<T>(scene_dir_,id);
  this->update_data_cache<T>(loaded);
  return loaded;
}

//: update data cache by type
template<boxm2_data_type T>
void boxm2_dumb_cache::update_data_cache(boxm2_data<T>* dat)
{
  vcl_map<vcl_string, boxm2_data_base* >::iterator iter;
  iter = cached_data_.find(boxm2_data_traits<T>::prefix());
  if ( iter != cached_data_.end() )
  {
    boxm2_data_base* old = (*iter).second;
    if (old) delete old;
  }
  cached_data_[boxm2_data_traits<T>::prefix()] = dat;
}

#endif // boxm2_dumb_cache_h_
