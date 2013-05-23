// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_update_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the scene.
//
// \author Ali Osman Ulusoy
// \date May 10, 2013

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <bstm/bstm_util.h>
#include <vil/vil_image_view.h>

#include <vil/vil_new.h>
#include <vpl/vpl.h> // vpl_unlink()

#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <bstm/ocl/algo/bstm_ocl_update.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace bstm_ocl_update_process_globals
{
  const unsigned int n_inputs_  = 5;
  const unsigned int n_outputs_ = 0;
}

bool bstm_ocl_update_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_update_process_globals;

  //process takes 9 inputs (of which the four last ones are optional):
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";      //input camera
  input_types_[4] = "vil_image_view_base_sptr";     //input image

  // process has no outputs
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);


  return good;
}

bool bstm_ocl_update_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_update_process_globals;

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned int i = 0;
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
  bstm_scene_sptr         scene        = pro.get_input<bstm_scene_sptr>(i++);
  bstm_opencl_cache_sptr  opencl_cache = pro.get_input<bstm_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr  cam          = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img          = pro.get_input<vil_image_view_base_sptr>(i++);

  return bstm_ocl_update::update(scene, device, opencl_cache, cam, img);
}
