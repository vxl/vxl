// This is brl/bseg/boxm/ocl/pro/processes/boxm_render_update_bit_scene_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for initializing a n update bit  scene  manager using OpenCL 
//
// \author Vishal Jain
// \date Sept 29, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_fstream.h>
#include <boxm/ocl/boxm_ocl_bit_scene.h>
#include <boxm/ocl/boxm_update_bit_scene_manager.h>
#include <boxm/ocl/boxm_update_bit_scene_manager_sptr.h>

namespace boxm_render_update_bit_scene_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}

bool boxm_render_update_bit_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_render_update_bit_scene_process_globals;
  // process takes 3 inputs and has no outputs
  // input[0]: scene.xml file path
  // input[1]: prob_thresh
  // input[2]: output_directory
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_update_bit_scene_manager_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";

  if (!pro.set_input_types(input_types_))
    return false;
  
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  return true;

}

bool boxm_render_update_bit_scene_process(bprb_func_process& pro)
{
  using namespace boxm_render_update_bit_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  boxm_update_bit_scene_manager_sptr mgr     = pro.get_input<boxm_update_bit_scene_manager_sptr>(i++);
  vpgl_camera_double_sptr        cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);

  if(vpgl_proj_camera<double> * pcam=dynamic_cast<vpgl_proj_camera<double> *> (cam_ptr.ptr()))
  {
      mgr->set_persp_camera(pcam);
      mgr->write_persp_camera_buffers();
      mgr->rendering();
      mgr->read_output_image();
      vil_image_view_base_sptr out_img=mgr->get_output_image();
      pro.set_output_val<vil_image_view_base_sptr>(0,out_img);
  }
  return true;
}
