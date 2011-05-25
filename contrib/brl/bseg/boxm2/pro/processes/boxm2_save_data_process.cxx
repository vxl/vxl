// This is brl/bseg/boxm2/pro/processes/boxm2_save_data_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for saving data (SAVES ONLY ALPHA AND MOG3_GREY)
//
// \author Vishal Jain
// \date Mar 15, 2011

#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

namespace boxm2_save_data_process_globals
{
  const unsigned n_inputs_  = 2;
  const unsigned n_outputs_ = 0;
}

bool boxm2_save_data_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_save_data_process_globals;

  //process takes 2 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0]  = "boxm2_scene_sptr";
  input_types_[1]  = "boxm2_cache_sptr";

  // process has 0 outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_save_data_process(bprb_func_process& pro)
{
  using namespace boxm2_save_data_process_globals;
/*
  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }*/
  //get the inputs
  unsigned i = 0;
  //boxm2_scene_sptr scene    =pro.get_input<boxm2_scene_sptr>(i++);
  //boxm2_cache_sptr cache    =pro.get_input<boxm2_cache_sptr>(i++);
  //vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  //vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  /*
  for (iter = blocks.begin(); iter != blocks.end(); ++iter)
  {
    boxm2_block_id id = iter->first;
    boxm2_sio_mgr::save_block(scene->data_path(), cache->get_block(id));
    boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_ALPHA>(id) );
    vcl_vector<vcl_string> apps = scene->appearances();
    for(unsigned j=0;j<apps.size();j++)
    {

        if(apps[j]== boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
           boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_MOG3_GREY>(id) );
        else if(apps[j]== boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix())
           boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_MOG3_GREY_16>(id) );
        else if(apps[j]== boxm2_data_traits<BOXM2_NUM_OBS>::prefix())
           boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_NUM_OBS>(id) );
        else if(apps[j]== boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix())
           boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_NUM_OBS_SINGLE>(id) );
        else if(apps[j]== boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix())
           boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_GAUSS_RGB>(id) );
        else 
        {
            vcl_cout<<"Write function missing for "<<apps[j]<<vcl_endl;
        }
    }   
  }
*/
  return true;
}
