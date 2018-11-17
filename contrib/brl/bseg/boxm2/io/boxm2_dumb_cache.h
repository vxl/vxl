#ifndef boxm2_dumb_cache_h_
#define boxm2_dumb_cache_h_
//:
// \file
#include <iostream>
#include <boxm2/io/boxm2_cache1.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: boxm2_dumb_cache - example realization of abstract cache class
class boxm2_dumb_cache : public boxm2_cache1
{
  public:
    boxm2_dumb_cache(boxm2_scene* scene);
    ~boxm2_dumb_cache() override;

    //: returns block pointer to block specified by ID
    boxm2_block* get_block(boxm2_block_id id) override;

    //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
    boxm2_data_base* get_data_base(boxm2_block_id, std::string type, std::size_t num_bytes=0, bool read_only = true) override;

    //: deletes data from dumb cache
    void remove_data_base(boxm2_block_id, std::string type) override;
    void replace_data_base(boxm2_block_id id, std::string type, boxm2_data_base* replacement) override;

    //: returns data pointer to data block specified by ID
    template <boxm2_data_type T>
    boxm2_data<T>* get_data(boxm2_block_id id);

    //: dumps writeable data onto disk
    // \todo not yet implemented
    void write_to_disk() override { std::cerr << "write_to_disk() not yet implemented!!!\n"; }

    //: disable the write process
    // \todo not yet implemented
    void disable_write() override { std::cerr << "disable_write() not yet implemented!!!\n"; }

    //: delete all the memory
    void clear_cache() override { std::cerr << "clear_cache() not yet implemented!!!\n"; }
  private:

    //: private update cache method (very simple)
    void update_block_cache(boxm2_block* blk);

    //: private update method (very simple)
    void update_data_base_cache(boxm2_data_base* dat, const std::string& type);

    //: private update block cache method
    template <boxm2_data_type T>
    void update_data_cache(boxm2_data<T>* dat);

    //: dumb cache keeps one cached block, the last one used.
    boxm2_block* cached_block_;

    //: keeps one copy of each type of cached data
    std::map<std::string, boxm2_data_base* > cached_data_;

    //: directory where blocks are found
    std::string scene_dir_;
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
  std::map<std::string, boxm2_data_base* >::iterator iter;
  iter = cached_data_.find(boxm2_data_traits<T>::prefix());
  if ( iter != cached_data_.end() )
  {
    boxm2_data_base* old = (*iter).second;
    if (old) delete old;
  }
  cached_data_[boxm2_data_traits<T>::prefix()] = dat;
}

#endif // boxm2_dumb_cache_h_
