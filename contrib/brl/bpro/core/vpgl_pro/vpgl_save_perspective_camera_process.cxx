// This is brl/bpro/core/vpgl_pro/vpgl_save_perspective_camera_process.cxx
#include "vpgl_save_perspective_camera_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vnl/vnl_matrix_fixed.h>

//: Constructor
vpgl_save_perspective_camera_process::vpgl_save_perspective_camera_process()
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

  //parameters
#if 0
  if ( !parameters()->add( "Image file <filename...>" , "-image_filename" , bprb_filepath("","*") ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
#endif // 0
}


//: Destructor
vpgl_save_perspective_camera_process::~vpgl_save_perspective_camera_process()
{
}


//: Execute the process
bool
vpgl_save_perspective_camera_process::execute()
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

  vpgl_perspective_camera<double> *cam =
    dynamic_cast<vpgl_perspective_camera<double>*>(input0->value().ptr());

  if (!cam) {
    vcl_cerr << "error: could not convert camera input to a vpgl_perspective_camera" << vcl_endl;
    return false;
  }

  vcl_string camera_filename = input1->value();

  // write matrices to the text file.
  vcl_ofstream ofs(camera_filename.c_str());
  if (!ofs.is_open()) {
    vcl_cerr << "Failed to open file " << camera_filename << vcl_endl;
    return false;
  }
  ofs << cam->get_calibration().get_matrix() << vcl_endl;
  vnl_matrix_fixed<double,3,3> R = cam->get_rotation().as_matrix();
  ofs << R << vcl_endl;
  vgl_point_3d<double> c = cam->get_camera_center();
  vnl_vector_fixed<double,3> T = -R*vnl_vector_fixed<double,3>(c.x(),c.y(),c.z());
  ofs << T << vcl_endl;

  ofs.close();

  return true;
}

