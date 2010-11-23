#include "boxm2_nn_cache.h"
//:
// \file

//: destructor 
boxm2_nn_cache::~boxm2_nn_cache()
{
  // clean up block
  vcl_map<boxm2_block_id, boxm2_block* >::iterator blk_i; 
  for(blk_i=cached_blocks_.begin(); blk_i!=cached_blocks_.end(); ++blk_i) {
    boxm2_block* blk = (*blk_i).second; 
    if(blk) delete blk; 
  }
  
  // clean up loaded data
  vcl_map<vcl_string, vcl_map<boxm2_block_id, boxm2_data_base*> >::iterator dat_i; 
  for(dat_i=cached_data_.begin(); dat_i!=cached_data_.end(); ++dat_i) {
    vcl_map<boxm2_block_id, boxm2_data_base*>& dmap = (*dat_i).second; 
    vcl_map<boxm2_block_id, boxm2_data_base*>::iterator db_i;
    
    //go through individual map and delete
    for(db_i = dmap.begin(); db_i != dmap.end(); ++db_i) {
      boxm2_data_base* dat = (*db_i).second; 
      if(dat) delete dat; 
    }
  } 
}

//: realization of abstract "get_block(block_id)"
boxm2_block* boxm2_nn_cache::get_block(boxm2_block_id id)
{
  //first thing to do is to load all async requests into the cache
  this->finish_async_blocks(); 
  
  
  if( cached_blocks_.find(id) != cached_blocks_.end() )
  {
    //congrats you've found the block in cache, now update cache and return block
    this->update_block_cache(cached_blocks_[id]); 
    return cached_blocks_[id]; 
  }

  //other wise load it from disk with blocking and update cache
  boxm2_block* loaded = boxm2_sio_mgr::load_block(scene_dir_, id); 
  this->update_block_cache(loaded); 
  return loaded;
}

void boxm2_nn_cache::update_block_cache(boxm2_block* blk) 
{
  
  boxm2_block_id center = blk->block_id(); 
  
  //find neighbors in x,y plane (i,j)
  vcl_vector<boxm2_block_id> neighbor_list; 
  for(int i=-1; i<=1; i++) {
    for(int j=-1; j<=1; j++) {
      boxm2_block_id id(center.i() + i, center.j() + j, center.k()); 
      if(this->is_valid_id(id)) {
        neighbor_list.push_back(id); 
      }
    }
  }
  
  //if neighbors aren't in the cache, 
  
  
  //if(cached_block_) delete cached_block_; 
  //cached_block_ = blk; 
}



////: realization of abstract "get_block(block_id)"
//void boxm2_nn_cache::update_cache(boxm2_block_id id)
//{
  //vcl_cout<<"Dumb Cache Update"<<vcl_endl;
//}


void boxm2_nn_cache::finish_async_blocks()
{
  // get async block list and push it into the cache

}

//: helper method says whether or not block id is valid
bool boxm2_nn_cache::is_valid_id(boxm2_block_id id)
{
  return  id.i() >= 0 && 
          id.j() >= 0 && 
          id.k() >= 0 &&
          id.i() <= block_num_.x() && 
          id.j() <= block_num_.y() &&
          id.k() <= block_num_.z(); 
}

