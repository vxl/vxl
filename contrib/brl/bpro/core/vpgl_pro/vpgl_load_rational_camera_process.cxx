// This is brl/bpro/core/vpgl_pro/vpgl_load_rational_camera_process.txx

//:
// \file

#include "vpgl_load_rational_camera_process.h"

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>

//: Constructor
vpgl_load_rational_camera_process::vpgl_load_rational_camera_process()
{
  //this process takes one input: the filename
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0]="vcl_string";

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vpgl_camera_double_sptr";
  
  //parameters
 /* if( !parameters()->add( "Image file <filename...>" , "-image_filename" , bprb_filepath("","*") ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }*/
}


//: Destructor
vpgl_load_rational_camera_process::~vpgl_load_rational_camera_process()
{
}


//: Execute the process
bool
vpgl_load_rational_camera_process::execute()
{
  // Sanity check
    if(!this->verify_inputs())
    return false;

  
  //Retrieve filename from input
  brdb_value_t<vcl_string>* input0 = 
    static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());

  vcl_string camera_filename = input0->value();

  vbl_smart_ptr<vpgl_camera<double> > ratcam = read_rational_camera<double>(camera_filename);;

  if( !ratcam.as_pointer() ) {
    vcl_cerr << "Failed to load rational camera from file" << camera_filename << vcl_endl;
    return false;
  }

  brdb_value_sptr output0 = new brdb_value_t<vbl_smart_ptr<vpgl_camera<double> > >(ratcam);
  
  output_data_[0] = output0;

  return true;
}




