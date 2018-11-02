// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_refine_process2.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for refining the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_refine_process2_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_refine_process2_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "float";
  input_types_[3] = "vcl_string";// if identifier is empty, then only one appearance model

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // in case the 4th input is not set
  brdb_value_sptr id = new brdb_value_t<std::string>("");
  pro.set_input(3, id);
  return good;
}

bool boxm2_cpp_refine_process2(bprb_func_process& pro)
{
  using namespace boxm2_cpp_refine_process2_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  auto  thresh=pro.get_input<float>(i++);
  std::string identifier = pro.get_input<std::string>(i++);

  bool foundDataType = false;
  std::string data_type;
  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = app;
      foundDataType = true;
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_CPP_REFINE_PROCESS2 ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }
  std::string num_obs_type = boxm2_data_traits<BOXM2_NUM_OBS>::prefix();
  if (identifier.size() > 0) {
    data_type += "_" + identifier;
    num_obs_type += "_" + identifier;
  }

  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    std::cout<<"Refining Block: "<<id<<std::endl;

    boxm2_block *     blk = cache->get_block(scene,id);
    boxm2_data_base * alph = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data_base * mog = cache->get_data_base(scene,id,data_type);
    boxm2_data_base * num_obs = cache->get_data_base(scene,id,num_obs_type);

    std::vector<boxm2_data_base*> datas;
    datas.push_back(alph);
    datas.push_back(mog);
    datas.push_back(num_obs);

    //refine block and datas
    boxm2_block_metadata data = blk_iter->second;
    boxm2_refine_block(scene,blk,datas, thresh, false);
    blk->enable_write(); // now cache will make sure that it is written to disc
  }

  return true;
}
