#include <boxm2/cpp/boxm2_cpp_refine_process.h>

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_save.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/cpp/boxm2_refine_block_function.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>


bool boxm2_cpp_refine_process::execute(vcl_vector<brdb_value_sptr>& input, vcl_vector<brdb_value_sptr>& output)
{

  //1. get the arguments blocks/camera/img etc from the input vector
  int i = 0;
  brdb_value_t<boxm2_scene_sptr>* brdb_scene      = static_cast<brdb_value_t<boxm2_scene_sptr>* >( input[i++].ptr() );
  boxm2_scene_sptr  scene                         = brdb_scene->value();

  //For each ID in the visibility order, grab that block
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks(); 
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk; 
  for(blk = blocks.begin(); blk != blocks.end(); ++blk) 
  {
    boxm2_block_id id = blk->first;

    boxm2_block *     blk     = this->cache_->get_block(id);
    boxm2_data_base * alph    = this->cache_->get_data_base(id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data_base * mog     = this->cache_->get_data_base(id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    boxm2_data_base * num_obs = this->cache_->get_data_base(id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    
    vcl_vector<boxm2_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);
    datas.push_back(num_obs); 
    
    //refine block and datas
    boxm2_refine_block(blk,datas, .3f);
  }
  vcl_cout<<"Execution time: "<<" ms"<<vcl_endl;
  return true;
}


bool boxm2_cpp_refine_process::clean()
{
  return true;
}

