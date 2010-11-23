#include "boxm2_dumb_cache.h"
//:
// \file

//: destructor 
boxm2_dumb_cache::~boxm2_dumb_cache()
{
  // clean up block
  if(cached_block_) delete cached_block_; 
  
  // clean up loaded data
  vcl_map<vcl_string, boxm2_data_base* >::iterator iter; 
  for(iter=cached_data_.begin(); iter!=cached_data_.end(); ++iter) {
    boxm2_data_base* dat = (*iter).second; 
    if(dat) delete dat; 
  } 
}

//: realization of abstract "get_block(block_id)"
boxm2_block* boxm2_dumb_cache::get_block(boxm2_block_id id)
{
  vcl_cout<<"Dumb Cache Get Block"<<vcl_endl;
  if(cached_block_) {
    if(cached_block_->block_id() == id)
      return cached_block_; 
  }

  //otherwise load it from disk with blocking
  boxm2_block* loaded = boxm2_sio_mgr::load_block(scene_dir_, id);
  
  //update cache
  this->update_block_cache(loaded); 
  return loaded;
}

void boxm2_dumb_cache::update_block_cache(boxm2_block* blk) 
{
  if(cached_block_) delete cached_block_; 
  cached_block_ = blk; 
}



////: realization of abstract "get_block(block_id)"
//void boxm2_dumb_cache::update_cache(boxm2_block_id id)
//{
  //vcl_cout<<"Dumb Cache Update"<<vcl_endl;
//}
