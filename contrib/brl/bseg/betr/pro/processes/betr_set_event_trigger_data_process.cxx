// This is brl/bseg/betr/pro/processes/betr_set_event_trigger_data_process.cxx
#include <iostream>
#include <fstream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//:
// \file
// \brief  A process for seting an event_trigger data to an event_trigger
//


#include <betr/betr_event_trigger.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>


namespace betr_set_event_trigger_data_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool betr_set_event_trigger_data_process_cons(bprb_func_process& pro)
{
  using namespace betr_set_event_trigger_data_process_globals;

  //process takes 5 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "betr_event_trigger_sptr";// event_trigger
  input_types_[1] = "vil_image_resource_sptr";// reference image
  input_types_[2] = "vpgl_camera_double_sptr";// reference camera
  input_types_[3] = "vil_image_resource_sptr";// event image
  input_types_[4] = "vpgl_camera_double_sptr";// event camera
  // process has 0 outputs
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_set_event_trigger_data_process(bprb_func_process& pro)
{
  using namespace betr_set_event_trigger_data_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  betr_event_trigger_sptr event_trigger = pro.get_input<betr_event_trigger_sptr>(i++);
  vil_image_resource_sptr ref_imgr = pro.get_input<vil_image_resource_sptr>(i++);
  // Assumed to be a global rational camera, projecting lon, lat, elv to (u,v)
  vpgl_camera_double_sptr ref_cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_resource_sptr evt_imgr = pro.get_input<vil_image_resource_sptr>(i++);
  // Assumed to be a global rational camera, projecting lon, lat, elv to (u,v)
  vpgl_camera_double_sptr evt_cam = pro.get_input<vpgl_camera_double_sptr>(i);
  if(!event_trigger||!ref_imgr || !ref_cam || !evt_imgr || !evt_cam)
    return false;
  // convert the cameras to local rational cameras using the origin of the event trigger
  auto* ref_rcam = dynamic_cast<vpgl_rational_camera<double>*>(ref_cam.ptr());
  if(!ref_rcam)
    return false;
  vpgl_local_rational_camera<double>* lcam = new vpgl_local_rational_camera<double>(event_trigger->lvcs(),*ref_rcam);
  vpgl_camera_double_sptr ref_lcam = dynamic_cast<vpgl_camera<double>*>(lcam);
  if(!ref_lcam)
    return false;
  auto* evt_rcam = dynamic_cast<vpgl_rational_camera<double>*>(evt_cam.ptr());
  if(!evt_rcam)
    return false;
  vpgl_local_rational_camera<double>* elcam = new vpgl_local_rational_camera<double>(event_trigger->lvcs(),*evt_rcam);
  vpgl_camera_double_sptr evt_lcam = dynamic_cast<vpgl_camera<double>*>(elcam);
  if(!evt_lcam)
    return false;
  event_trigger->set_ref_image(ref_imgr);
  event_trigger->set_ref_camera(ref_lcam);
  event_trigger->set_evt_image(evt_imgr);
  event_trigger->set_evt_camera(evt_lcam);
  return true;
}
