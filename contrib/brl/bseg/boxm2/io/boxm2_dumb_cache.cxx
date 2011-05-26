#include "boxm2_dumb_cache.h"
//:
// \file

//: constructor from scene pointer
boxm2_dumb_cache::boxm2_dumb_cache(boxm2_scene* scene) : boxm2_cache(scene), cached_block_(0)
{
  scene_dir_ = scene->data_path();
}


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

  //check to make sure it's loaded
  if (!loaded && scene_->block_exists(id)) {
    vcl_cout<<"boxm2_nn_cache::initializing empty block "<<id<<vcl_endl;
    boxm2_block_metadata data = scene_->get_block_metadata(id);
    loaded = new boxm2_block(data);
  }

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
boxm2_data_base* boxm2_dumb_cache::get_data_base(boxm2_block_id id, vcl_string type, vcl_size_t num_bytes, bool read_only)
{
  if ( cached_data_.find(type) != cached_data_.end() )
  {
    if (cached_data_[type]->block_id() == id)
      return cached_data_[type];
  }

  //otherwise load it from disk
  boxm2_data_base* loaded = boxm2_sio_mgr::load_block_data_generic(scene_dir_,id,type);

  //make sure it loaded
  if (!loaded && scene_->block_exists(id)) {
    vcl_cout<<"boxm2_nn_cache::initializing empty data "<<id<<" type: "<<type<<vcl_endl;
    boxm2_block_metadata data = scene_->get_block_metadata(id);
    loaded = new boxm2_data_base(data, type);
  }

  if (!read_only) this->update_data_base_cache(loaded, type);
  return loaded;
}

void boxm2_dumb_cache::remove_data_base(boxm2_block_id, vcl_string type)
{
  vcl_cout<<"BOXM2_DUMB_CACHE::remove_data_base not implemented"<<vcl_endl;
}

void boxm2_dumb_cache::replace_data_base(boxm2_block_id, vcl_string type, boxm2_data_base* replacement)
{
  vcl_cout<<"BOXM2_DUMB_CACHE::replace_data_base not implemented"<<vcl_endl;
}


//: update data cache by type
void boxm2_dumb_cache::update_data_base_cache(boxm2_data_base* dat, vcl_string type)
{
  vcl_map<vcl_string, boxm2_data_base* >::iterator iter;
  iter = cached_data_.find(type);
  if ( iter != cached_data_.end() )
  {
    boxm2_data_base* old = (*iter).second;
    if (old) delete old;
  }
  cached_data_[type] = dat;
}

