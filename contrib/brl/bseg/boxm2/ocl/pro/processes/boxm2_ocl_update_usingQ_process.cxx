// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_usingQ_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the scene using Qs independently computed from different viewpoints.
//
// \author Vishal Jain
// \date Mar 25, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>

#include <vil/vil_new.h>
#include <vpl/vpl.h> // vpl_unlink()

#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_auxQ.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>

namespace boxm2_ocl_update_usingQ_process_globals
{
  constexpr unsigned int n_inputs_ = 5;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_update_usingQ_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_usingQ_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "int";                          //0: to init, 1: to accumulate Qs, 2: compute probability
  input_types_[4] = "vcl_string";                   //identifier
  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // default 4 inputs
  brdb_value_sptr idx3        = new brdb_value_t<int>(0);
  pro.set_input(3, idx3);
  brdb_value_sptr idx4        = new brdb_value_t<std::string>("");
  pro.set_input(4, idx4);

  return good;
}

bool boxm2_ocl_update_usingQ_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_usingQ_process_globals;
  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned int i = 0;
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         scene        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  int  option                           = pro.get_input<int>(i++);
  std::string               ident        = pro.get_input<std::string>(i++);
  vul_timer t;
  t.mark();
  //TODO Factor this out to a utility function
  //make sure this image small enough (or else carve it into image pieces)
  if(option == 0)
      boxm2_ocl_update_PusingQ::init_product(scene,opencl_cache->get_cpu_cache());
  else if (option == 1)
      boxm2_ocl_update_PusingQ::accumulate_product(scene,device,opencl_cache,ident);
  else if(option == 2)
      boxm2_ocl_update_PusingQ::compute_probability(scene,device,opencl_cache);
  else
  {
      std::cout<<"Incorrect option"<<std::endl;
      return false;
  }
  return true;
}
