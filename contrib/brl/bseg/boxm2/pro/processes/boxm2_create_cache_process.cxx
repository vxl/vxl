// This is brl/bseg/boxm2/pro/processes/boxm2_create_cache_process.cxx
//:
// \file
// \brief  A process for creating cache.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache1.h>
#include <boxm2/io/boxm2_lru_cache1.h>
#include <boxm2/io/boxm2_nn_cache.h>


namespace boxm2_create_cache1_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}
bool boxm2_create_cache1_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_cache1_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "bool";
  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_cache_sptr";
  brdb_value_sptr idx = new brdb_value_t<bool>(true);
  pro.set_input(2, idx);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_cache1_process(bprb_func_process& pro)
{
  using namespace boxm2_create_cache1_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene= pro.get_input<boxm2_scene_sptr>(i++);
  std::string cache_type= pro.get_input<std::string>(i++);
  bool islocal= pro.get_input<bool>(i++);
  if(cache_type=="lru")
  {
      std::cout<<"Create Cache"<<std::endl;
      if(!islocal)
          boxm2_lru_cache1::create(scene,HDFS);
      else
          boxm2_lru_cache1::create(scene,LOCAL);

  }
  else if (cache_type=="nn")
  {
     // boxm2_nn_cache::create(scene);

      return false;
  }
  else
  {
    return false;
  }
  i=0;

  // store scene smaprt pointer
  pro.set_output_val<boxm2_cache1_sptr>(i++, boxm2_cache1::instance());
  return true;
}

#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_lru_cache.h>

namespace boxm2_create_cache_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}
bool boxm2_create_cache_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_cache_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "bool";
  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_cache_sptr";
  brdb_value_sptr idx = new brdb_value_t<bool>(true);
  pro.set_input(2, idx);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_cache_process(bprb_func_process& pro)
{
  using namespace boxm2_create_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene= pro.get_input<boxm2_scene_sptr>(i++);
  std::string cache_type= pro.get_input<std::string>(i++);
  bool islocal= pro.get_input<bool>(i++);
  if(cache_type=="lru")
  {
      std::cout<<"Create Cache"<<std::endl;
      if(!islocal)
          boxm2_lru_cache::create(scene,HDFS);
      else
          boxm2_lru_cache::create(scene,LOCAL);

  }
  else if (cache_type=="nn")
  {
     // boxm2_nn_cache::create(scene);

      return false;
  }
  else
  {
    return false;
  }
  i=0;

  // store scene smaprt pointer
  pro.set_output_val<boxm2_cache_sptr>(i++, boxm2_cache::instance());
  return true;
}
