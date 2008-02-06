// This is brl/bpro/core/vpgl_pro/vpgl_load_proj_camera_process.txx

//:
// \file

#include "vpgl_load_proj_camera_process.h"

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vnl/vnl_matrix_fixed.h>

//: Constructor
vpgl_load_proj_camera_process::vpgl_load_proj_camera_process()
{
  //this process takes one input: the filename
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0]="vcl_string";

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vpgl_camera_double_ptr";
  
  //parameters
 /* if( !parameters()->add( "Image file <filename...>" , "-image_filename" , bprb_filepath("","*") ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }*/
}


//: Destructor
vpgl_load_proj_camera_process::~vpgl_load_proj_camera_process()
{
}


//: Execute the process
bool
vpgl_load_proj_camera_process::execute()
{
  // Sanity check
    if(!this->verify_inputs())
    return false;

  
  //Retrieve filename from input
  brdb_value_t<vcl_string>* input0 = 
    static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());

  vcl_string camera_filename = input0->value();
 
  vpgl_proj_camera<double> *procam = new vpgl_proj_camera<double>();
  // read projection matrix from the file.
  vcl_ifstream ifs(camera_filename.c_str());
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << camera_filename << vcl_endl;
    return false;
  }
  vnl_matrix_fixed<double,3,4> projection_matrix;
  ifs >> projection_matrix;

  procam->set_matrix(projection_matrix);

  brdb_value_sptr output0 = new brdb_value_t<vpgl_camera<double>*>(static_cast<vpgl_camera<double>*>(procam));
  
  output_data_[0] = output0;

  return true;
}




