// This is brl/bseg/betr/pro/processes/betr_set_event_trigger_data_process.cxx
//:
// \file
// \brief  A process for seting an event_trigger data to an event_trigger
//

#include <iostream>
#include <fstream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <betr/betr_event_trigger.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>


namespace betr_set_event_trigger_partial_data_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool betr_set_event_trigger_partial_data_process_cons(bprb_func_process& pro)
{
  using namespace betr_set_event_trigger_partial_data_process_globals;

  //process takes 5 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "betr_event_trigger_sptr";// event_trigger
  input_types_[1] = "vil_image_resource_sptr";// image
  input_types_[2] = "vpgl_camera_double_sptr";// camera
  input_types_[3] = "bool"; // is a reference object (or if false an event object)
  input_types_[4] = "bool"; // keep reference data (false = clear reference data)

  // process has 0 outputs
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_set_event_trigger_partial_data_process(bprb_func_process& pro)
{
  using namespace betr_set_event_trigger_partial_data_process_globals;

  // check number of inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs
  // "cam" Assumed to be a global rational camera, projecting lon, lat, elv to (u,v)
  unsigned i = 0;
  betr_event_trigger_sptr event_trigger = pro.get_input<betr_event_trigger_sptr>(i++);
  vil_image_resource_sptr imgr = pro.get_input<vil_image_resource_sptr>(i++);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  bool is_reference = pro.get_input<bool>(i++);
  bool keep_data = pro.get_input<bool>(i++);

  // check inputs
  if(!event_trigger || !imgr || !cam)
    return false;
  if(!is_reference && keep_data) {
    std::cout << "Cannot append event data to trigger" << std::endl;
    return false;
  }

  // convert the camera to local rational cameras using the origin of the event trigger
  auto* rcam = dynamic_cast<vpgl_rational_camera<double>*>(cam.ptr());
  if(!rcam)
    return false;
  vpgl_local_rational_camera<double>* lcam = new vpgl_local_rational_camera<double>(event_trigger->lvcs(),*rcam);
  vpgl_camera_double_sptr cam_converted = dynamic_cast<vpgl_camera<double>*>(lcam);
  if(!cam_converted)
    return false;

  if (is_reference) {
    event_trigger->set_ref_image(imgr,false,keep_data);
    event_trigger->set_ref_camera(cam_converted,keep_data);
  } else {
    event_trigger->set_evt_image(imgr);
    event_trigger->set_evt_camera(cam_converted);
  }

  return true;
}
