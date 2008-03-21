// This is brl/bpro/core/vil_pro/vil_map_image_process.cxx

//:
// \file

#include "vil_map_image_process.h"
#include "vil_math_functors.h"
#include <vil/vil_convert.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_transform.h>
#include <vil/vil_math.h>

//: Constructor
vil_map_image_process::vil_map_image_process()
{
  //input  
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]="vil_image_view_base_sptr";
  input_types_[1]="vcl_string";

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";
  
}


//: Destructor
vil_map_image_process::~vil_map_image_process()
{
}


//: Execute the process
bool
vil_map_image_process::execute()
{
  // Sanity check
    if(!this->verify_inputs())
    return false;
  
  //Retrieve image from input
  brdb_value_t<vil_image_view_base_sptr>* input0 = 
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr image = input0->value();

  //Retrieve map functor name  
  brdb_value_t<vcl_string>* input1 = 
    static_cast<brdb_value_t<vcl_string>* >(input_data_[1].ptr());
  vcl_string functor = input1->value();

  // convert image to float
  vil_image_view_base_sptr fimage = vil_convert_cast(float(), image);
  vil_image_view<float>fimg = *fimage;
  // apply the functor
  if(functor == "log"){
  vil_math_log_functor lg;
  vil_transform(fimg, lg);
  }
  else if(functor == "not"){
      vil_math_not_functor nt;
      vil_transform(fimg, nt);
    }
  else if(functor == "log_not"){
      vil_math_log_not_functor nlg;
      vil_transform(fimg, nlg);
    }
  else{
    vcl_cerr << "In vil_map_image_process::execute() - unknown functor "
             << functor << '\n';
    return false;
  }
  //for now, only handle byte or float outputs
  vil_image_view_base_sptr map_image;
  if(image->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
    {
      vil_image_view<vxl_byte>* temp = new vil_image_view<vxl_byte>;
      vil_convert_stretch_range(fimg, *temp);
	  map_image = temp;
    }
  else //leave the image as float
    map_image = fimage;
  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(map_image);
  output_data_[0] = output0;
  return true;
}




