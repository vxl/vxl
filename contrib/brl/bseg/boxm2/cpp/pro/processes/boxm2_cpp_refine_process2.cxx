// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_refine_process2.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for refining the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <vcl_fstream.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_refine_block_function.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_refine_process2_globals
{
  const unsigned n_inputs_ =  4;
  const unsigned n_outputs_ = 0;
}

bool boxm2_cpp_refine_process2_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_refine_process2_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "float";
  input_types_[3] = "vcl_string";// if identifier is empty, then only one appearance model

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // in case the 4th input is not set
  brdb_value_sptr id = new brdb_value_t<vcl_string>("");
  pro.set_input(3, id);
  return good;
}

bool boxm2_cpp_refine_process2(bprb_func_process& pro)
{
  using namespace boxm2_cpp_refine_process2_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  float  thresh=pro.get_input<float>(i++);
  vcl_string identifier = pro.get_input<vcl_string>(i++);

  bool foundDataType = false;
  vcl_string data_type;
  vcl_vector<vcl_string> apps = scene->appearances();
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_OCL_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }
  vcl_string num_obs_type = boxm2_data_traits<BOXM2_NUM_OBS>::prefix(); 
  if (identifier.size() > 0) {
    data_type += "_" + identifier;
    num_obs_type += "_" + identifier;
  }

  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    vcl_cout<<"Refining Block: "<<id<<vcl_endl;

    boxm2_block *     blk     = cache->get_block(id);
    boxm2_data_base * alph    = cache->get_data_base(id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data_base * mog     = cache->get_data_base(id,data_type);
    boxm2_data_base * num_obs = cache->get_data_base(id,num_obs_type);

    vcl_vector<boxm2_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);
    datas.push_back(num_obs);

    //refine block and datas
    boxm2_block_metadata data = blk_iter->second;
    boxm2_refine_block(blk,datas, thresh, false);
    blk->enable_write(); // now cache will make sure that it is written to disc
  }

  return true;
}
