// This is brl/bpro/core/vpgl_pro/vpgl_load_rational_camera_nitf_process.cxx
#include "vpgl_load_rational_camera_nitf_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>

//: Constructor
vpgl_load_rational_camera_nitf_process::vpgl_load_rational_camera_nitf_process()
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
#if 0
  if ( !parameters()->add( "Image file <filename...>" , "-image_filename" , bprb_filepath("","*") ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
#endif // 0
}


//: Destructor
vpgl_load_rational_camera_nitf_process::~vpgl_load_rational_camera_nitf_process()
{
}


//: Execute the process
bool
vpgl_load_rational_camera_nitf_process::execute()
{
  // Sanity check
    if (!this->verify_inputs())
    return false;

  // Retrieve filename from input
  brdb_value_t<vcl_string>* input0 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());

  vcl_string nitf_image_path = input0->value();

      vil_image_resource_sptr image =
        vil_load_image_resource(nitf_image_path.c_str());
    if (!image)
    {
        vcl_cout << "NITF image load failed in vpgl_load_rational_camera_nitf_process\n";
        return 0;
    }

    vcl_string format = image->file_format();
    vcl_string prefix = format.substr(0,4);
    
    if (prefix != "nitf")
    {
        vcl_cout << "source image is not NITF in vpgl_load_rational_camera_nitf_process\n";
        return 0;
    }

    //cast to an nitf2_image
    vil_nitf2_image *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());
  vpgl_camera_double_sptr ratcam = new vpgl_nitf_rational_camera(nitf_image, true); 

  if ( !ratcam.as_pointer() ) {
    vcl_cerr << "Failed to load rational camera from file" << nitf_image_path << vcl_endl;
    return false;
  }

  brdb_value_sptr output0 = new brdb_value_t<vpgl_camera_double_sptr>(ratcam);

  output_data_[0] = output0;

  return true;
}

