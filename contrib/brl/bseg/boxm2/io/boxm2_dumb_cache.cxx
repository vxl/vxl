#include "boxm2_dumb_cache.h"
//:
// \file

//: destructor
boxm2_dumb_cache::~boxm2_dumb_cache()
{
#if 0
  // clean up block
  delete cached_block_;

  // clean up loaded data
  vcl_map<vcl_string, boxm2_data_base* >::iterator iter;
  for (iter=cached_data_.begin(); iter!=cached_data_.end(); ++iter) {
    delete (*iter).second;
  }
#endif // 0
}

//: realization of abstract "get_block(block_id)"
boxm2_block* boxm2_dumb_cache::get_block(boxm2_block_id id)
{
  vcl_cout<<"Dumb Cache Get Block"<<vcl_endl;
  if (cached_block_ && cached_block_->block_id() == id)
    return cached_block_;

  //otherwise load it from disk with blocking
  boxm2_block* loaded = boxm2_sio_mgr::load_block(scene_dir_, id);

  //update cache
  this->update_block_cache(loaded);
  return loaded;
}

void boxm2_dumb_cache::update_block_cache(boxm2_block* blk)
{
  //delete cached_block_;
  cached_block_ = blk;
}

//: get data by type and id
boxm2_data_base* boxm2_dumb_cache::get_data_base(boxm2_block_id id, vcl_string type)
{
  if( cached_data_.find(type) != cached_data_.end() )
  {
    if(cached_data_[type]->block_id() == id)
      return cached_data_[type]; 
  }
  
  //otherwise load it from disk
  boxm2_data_base* loaded = boxm2_sio_mgr::load_block_data_generic(scene_dir_,id,type);
  this->update_data_base_cache(loaded, type); 
  return loaded; 
}

//: update data cache by type
void boxm2_dumb_cache::update_data_base_cache(boxm2_data_base* dat, vcl_string type)
{
  vcl_map<vcl_string, boxm2_data_base* >::iterator iter; 
  iter = cached_data_.find(type);
  if( iter != cached_data_.end() )
  {
    boxm2_data_base* old = (*iter).second;
    if(old) delete old; 
  }
  cached_data_[type] = dat; 
}

