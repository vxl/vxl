// This is brl/bseg/boxm/ocl/pro/processes/boxm_change_detection_bit_scene_process.cxx
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
#include <vil/vil_convert.h>

namespace boxm_change_detection_bit_scene_process_globals
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 1;
}

bool boxm_change_detection_bit_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_change_detection_bit_scene_process_globals;
  // process takes 3 inputs and has no outputs
  // input[0]: scene.xml file path
  // input[1]: prob_thresh
  // input[2]: output_directory
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_update_bit_scene_manager_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "vil_image_view_base_sptr";

  if (!pro.set_input_types(input_types_))
    return false;
  
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  return true;

}

bool boxm_change_detection_bit_scene_process(bprb_func_process& pro)
{
  using namespace boxm_change_detection_bit_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  boxm_update_bit_scene_manager_sptr mgr = pro.get_input<boxm_update_bit_scene_manager_sptr>(i++);
  vpgl_camera_double_sptr        cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr       img_ptr = pro.get_input<vil_image_view_base_sptr>(i++);

  if(vpgl_proj_camera<double> * pcam=dynamic_cast<vpgl_proj_camera<double> *> (cam_ptr.ptr()))
  {
      mgr->set_persp_camera(pcam);
      mgr->write_persp_camera_buffers();

      vil_image_view<float> floatimg(img_ptr->ni(), img_ptr->nj(), 1);
      if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(img_ptr.ptr()))
          vil_convert_stretch_range_limited(*img_byte ,floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
      else {
          vcl_cerr << "Failed to load image " << vcl_endl;
          return 0;
      }
      mgr->set_input_image(floatimg);
      mgr->write_image_buffer();

      mgr->change_detection();
      mgr->read_output_image();
      vil_image_view_base_sptr out_img=mgr->get_output_image();
      pro.set_output_val<vil_image_view_base_sptr>(0,out_img);
  }
  return true;
}
