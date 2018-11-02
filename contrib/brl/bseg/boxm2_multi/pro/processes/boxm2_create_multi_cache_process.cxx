// This is brl/bseg/boxm2/pro/processes/boxm2_create_multi_cache_process.cxx
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
#include <boxm2_multi/boxm2_multi_cache.h>

#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_create_multi_cache_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}
bool boxm2_create_multi_cache_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_multi_cache_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "int";
  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_multi_cache_sptr";

  brdb_value_sptr idx = new brdb_value_t<int>(1);
  pro.set_input(1, idx);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_multi_cache_process(bprb_func_process& pro)
{
    using namespace boxm2_create_multi_cache_process_globals;
    if ( pro.n_inputs() < n_inputs_ ){
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    //get the inputs
    unsigned i = 0;
    boxm2_scene_sptr scene= pro.get_input<boxm2_scene_sptr>(i++);
    int numGPU = pro.get_input<int>(i++);
    bocl_manager_child &mgr = bocl_manager_child::instance();
    //make a multicache
    if ( numGPU > mgr.gpus_.size() ) {
        std::cout<<"-numGPU ("<<numGPU<<") is too big, only "<<mgr.gpus_.size()<<" available"<<std::endl;
        return false;
    }
    //grab the number of devices specified
    std::vector<bocl_device_sptr> gpus;
    for (unsigned int i=0; i< numGPU; ++i)
        gpus.push_back(mgr.gpus_[i]);
    boxm2_multi_cache_sptr  mcache = new boxm2_multi_cache(scene, gpus);
    i=0;
    // store scene smaprt pointer
    pro.set_output_val<boxm2_multi_cache_sptr>(i++, mcache);
    return true;
}


namespace boxm2_write_multi_cache_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}
bool boxm2_write_multi_cache_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_write_multi_cache_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_multi_cache_sptr";
  input_types_[1] = "bool";
  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);


  brdb_value_sptr idx = new brdb_value_t<bool>(true);
  pro.set_input(1, idx);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_write_multi_cache_process(bprb_func_process& pro)
{
    using namespace boxm2_write_multi_cache_process_globals;
    if ( pro.n_inputs() < n_inputs_ ){
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    //get the inputs
    unsigned i = 0;
    boxm2_multi_cache_sptr mcache= pro.get_input<boxm2_multi_cache_sptr>(i++);
    bool doclear = pro.get_input<bool>(i++);
    if(doclear)
        mcache->clear();
    boxm2_lru_cache1::instance()->write_to_disk();
    return true;
}
