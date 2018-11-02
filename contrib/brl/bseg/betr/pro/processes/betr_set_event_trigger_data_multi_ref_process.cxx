// This is brl/bseg/betr/pro/processes/betr_set_event_trigger_data_multi_ref_process.cxx
//:
// \file
// \brief  A process for seting an event_trigger data to an event_trigger from paths
//

#include <iostream>
#include <fstream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_load.h>
#include <vpgl/vpgl_rational_camera.h>
#include <core/bbas_pro/bbas_1d_array_string.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_lvcs.h>

#include <betr/betr_event_trigger.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_camera.h>


namespace betr_set_event_trigger_data_multi_ref_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool betr_set_event_trigger_data_multi_ref_process_cons(bprb_func_process& pro)
{
  using namespace betr_set_event_trigger_data_multi_ref_process_globals;

  //process takes 5 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "betr_event_trigger_sptr";// event_trigger
  input_types_[1] = "bbas_1d_array_string_sptr";// reference image paths
  input_types_[2] = "bbas_1d_array_string_sptr";// reference camera paths
  input_types_[3] = "vcl_string";// event image path
  input_types_[4] = "vcl_string";// event camera path
  // process has 0 outputs
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_set_event_trigger_data_multi_ref_process(bprb_func_process& pro)
{
  using namespace betr_set_event_trigger_data_multi_ref_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  betr_event_trigger_sptr event_trigger = pro.get_input<betr_event_trigger_sptr>(i++);
  bbas_1d_array_string_sptr ref_img_paths = pro.get_input<bbas_1d_array_string_sptr>(i++);
  // Assumed to be a global rational camera, projecting lon, lat, elv to (u,v)
  bbas_1d_array_string_sptr ref_cam_paths = pro.get_input<bbas_1d_array_string_sptr>(i++);
  std::string evt_img_path = pro.get_input<std::string>(i++);
  // Assumed to be a global rational camera, projecting lon, lat, elv to (u,v)
  std::string evt_cam_path = pro.get_input<std::string>(i);
  unsigned nimg = (ref_img_paths->data_array).size();
  unsigned ncam = (ref_cam_paths->data_array).size();
  if(!event_trigger||!nimg || !ncam || evt_img_path=="" || evt_cam_path==""){
    std::cout << " null input data " << std::endl;
    return false;
  }
  if(nimg != ncam){
    std::cout << " reference image and reference camera array sizes don't match" << std::endl;
    return false;
  }
  // load the reference images
  std::vector<vil_image_resource_sptr> ref_rescs;
  for(unsigned i = 0; i<nimg; i++){
    std::string img_path = (ref_img_paths->data_array)[i];
    vil_image_resource_sptr resc = vil_load_image_resource(img_path.c_str());
    if(!resc){
      std::cout << "Invalid reference image  path " << img_path << std::endl;
      return false;
    }
    ref_rescs.push_back(resc);
  }
  std::vector<vpgl_camera_double_sptr> ref_cams;
  for(unsigned i = 0; i<ncam; i++){
    std::string cam_path = (ref_cam_paths->data_array)[i];
    vpgl_rational_camera<double>* rcam_ptr = read_rational_camera_from_txt<double>(cam_path);
    if(!rcam_ptr){
      std::cout << "can't read reference camera from txt file " << cam_path << std::endl;
      return false;
    }
    vpgl_local_rational_camera<double>* lcam = new vpgl_local_rational_camera<double>(event_trigger->lvcs(),*rcam_ptr);
    vpgl_camera_double_sptr ref_lcam = dynamic_cast<vpgl_camera<double>*>(lcam);
    ref_cams.push_back(ref_lcam);
    delete rcam_ptr;
  }
  //load event image resource
  vil_image_resource_sptr evt_resc = vil_load_image_resource(evt_img_path.c_str());
  if(!evt_resc){
    std::cout << "Invalid event image path " << evt_img_path << std::endl;
    return false;
  }
  //load event camera
  vpgl_rational_camera<double>* ecam_ptr = read_rational_camera_from_txt<double>(evt_cam_path);
  if(!ecam_ptr){
    std::cout << "can't read event camera from txt file " << evt_cam_path << std::endl;
    return false;
  }
  vpgl_local_rational_camera<double>* elcam = new vpgl_local_rational_camera<double>(event_trigger->lvcs(),*ecam_ptr);
  vpgl_camera_double_sptr evt_lcam = dynamic_cast<vpgl_camera<double>*>(elcam);
  if(!evt_lcam)
    return false;
  delete ecam_ptr;
  event_trigger->set_ref_images(ref_rescs);
  event_trigger->set_ref_cameras(ref_cams);
  event_trigger->set_evt_image(evt_resc);
  event_trigger->set_evt_camera(evt_lcam);
  return true;
}
