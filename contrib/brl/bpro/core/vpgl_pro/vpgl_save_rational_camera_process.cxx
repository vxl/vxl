// This is brl/bpro/core/vpgl_pro/vpgl_save_rational_camera_process.cxx
#include "vpgl_save_rational_camera_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>

//: Constructor
vpgl_save_rational_camera_process::vpgl_save_rational_camera_process()
{
  //this process takes two inputs:
  //input[0]: the camera
  //input[1]: the filename
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]="vpgl_camera_double_sptr";
  input_types_[1]="vcl_string";

  //no output
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
}


//: Destructor
vpgl_save_rational_camera_process::~vpgl_save_rational_camera_process()
{
}


//: Execute the process
bool
vpgl_save_rational_camera_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // Retrieve the camera from input
  brdb_value_t<vpgl_camera_double_sptr>* input0 =
      static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[0].ptr());

  //Retrieve filename from input
  brdb_value_t<vcl_string>* input1 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[1].ptr());

  vpgl_rational_camera<double> *cam =
    dynamic_cast<vpgl_rational_camera<double>*>(input0->value().ptr());

  if (!cam) {
    vcl_cerr << "error: could not convert camera input to a vpgl_rational_camera\n";
    return false;
  }

  vcl_string camera_filename = input1->value();

  if (!cam->save(camera_filename)) {
    vcl_cerr << "Failed to save file " << camera_filename << vcl_endl;
    return false;
  }

  return true;
}

