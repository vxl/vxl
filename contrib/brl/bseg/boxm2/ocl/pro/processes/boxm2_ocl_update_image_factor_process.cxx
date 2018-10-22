// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_image_factor_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the pre cellwise to compute image factor for BP formulation
//
// \author Vishal Jain
// \date Nov 11, 2014

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>

#include <vil/vil_new.h>


#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_image_factor.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>

namespace boxm2_ocl_update_image_factor_process_globals
{
  constexpr unsigned int n_inputs_ = 5;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_update_image_factor_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_image_factor_process_globals;
  //process takes 9 inputs (of which the four last ones are optional):
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned int i = 0;
  input_types_[i++] = "bocl_device_sptr";
  input_types_[i++] = "boxm2_scene_sptr";
  input_types_[i++] = "boxm2_opencl_cache_sptr";
  input_types_[i++] = "bool";
  input_types_[i++] = "vcl_string";     //input image
  // process has no outputs
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  return good;
}

bool boxm2_ocl_update_image_factor_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_image_factor_process_globals;
  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned int i = 0;
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         scene        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  bool  does_add = pro.get_input<bool>(i++);
  vcl_string view_identifier = pro.get_input<vcl_string>(i++);

  vul_timer t;
  t.mark();
  if(!boxm2_ocl_update_image_factor::update_image_factor(scene, device, opencl_cache, does_add, view_identifier))
    return false;
  vcl_cout<<"Total time taken is "<<t.all()<<vcl_endl;
  return true;
}
