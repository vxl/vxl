#ifndef boxm2_nn_cache_h_
#define boxm2_nn_cache_h_
//:
// \file
#include <boxm2/io/boxm2_cache.h>
#include <vcl_iostream.h>

//: an example cache that loads in the nearest neighbors of the requested block (asynchronously)
class boxm2_nn_cache : public boxm2_cache
{
  public:
    //: construct with directory and scene dimensions (blocknum)
    boxm2_nn_cache(vcl_string dir, vgl_vector_3d<int> bnum) : block_num_(bnum), scene_dir_(dir) {}
    ~boxm2_nn_cache();

    //: returns block pointer to block specified by ID
    virtual boxm2_block* get_block(boxm2_block_id id);

    //: returns data_base pointer (THIS IS NECESSARY BECAUSE TEMPLATED FUNCTIONS CANNOT BE VIRTUAL)
    virtual boxm2_data_base* get_data_base(boxm2_block_id, vcl_string type);
    
    //: returns data pointer to data block specified by ID
    template <boxm2_data_type T>
    boxm2_data<T>* get_data(boxm2_block_id id);

  private:

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
  //first thing to do is to load all async requests into the cache
  this->finish_async_data(boxm2_data_traits<T>::prefix());

  //grab a reference to the map of cached_data_
  vcl_map<boxm2_block_id, boxm2_data_base*>& data_map =
    this->cached_data_map(boxm2_data_traits<T>::prefix());

  //then look for the block you're requesting
  if ( data_map.find(id) != data_map.end() )
  {
    //congrats you've found the data block in cache, update cache and return block
    vcl_cout<<"DATA CACHE HIT! for "<<boxm2_data_traits<T>::prefix()<<vcl_endl;
    this->update_data_cache<T>(data_map[id]);
    return (boxm2_data<T>*) data_map[id];
  }

  //otherwise it's a miss, load sync from disk, update cache
  vcl_cout<<"Cache miss :( for "<<boxm2_data_traits<T>::prefix()<<vcl_endl;
  boxm2_data<T>* loaded = boxm2_sio_mgr::load_block_data<T>(scene_dir_, id);
  this->update_data_cache<T>(loaded);
  return loaded;
}


//: update data cache by type
template<boxm2_data_type T>
void boxm2_nn_cache::update_data_cache(boxm2_data_base* dat)
{
  //grab a reference to the map of cached_data
  vcl_map<boxm2_block_id, boxm2_data_base*>& data_map =
    this->cached_data_map(boxm2_data_traits<T>::prefix());

  //determine the center
  boxm2_block_id center = dat->block_id();
  vcl_cout<<"update block cache around: "<<center<<vcl_endl;

  //find neighbors in x,y plane (i,j)
  vcl_vector<boxm2_block_id> neighbor_list = this->get_neighbor_list(center);

  // initialize new cache with existing neighbor ptrs
  vcl_map<boxm2_block_id, boxm2_data_base*> new_cache;

  //find neighbors in the cache already, store em
  for (unsigned int i=0; i<neighbor_list.size(); ++i)
  {
    boxm2_block_id id = neighbor_list[i];

    //if cached_blocks_ has this neighbor, add it to new cache (delete from old)
    if ( data_map.find(id) != data_map.end() )
    {
      new_cache[id] = data_map[id];
      data_map.erase(id);
    }
    else //send an async request for this block
    {
      io_mgr_.load_block_data<T>(scene_dir_, id);
    }
  }

  //only non-neighbors remain in existing cache, delete em
  vcl_map<boxm2_block_id, boxm2_data_base*>::iterator blk_i;
  for (blk_i = data_map.begin(); blk_i != data_map.end(); ++blk_i)
  {
    boxm2_block_id   bid = blk_i->first;
    boxm2_data_base* blk = blk_i->second;
    if (bid != center && blk) {
       vcl_cout<<"deleting "<<bid<<" from cache"<<vcl_endl;
       delete blk;
    }
  }
  data_map.clear();

  //store block passed in
  new_cache[dat->block_id()] = dat;

  //swap out cache
  data_map = new_cache;
}

#endif // boxm2_nn_cache_h_
