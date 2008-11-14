// This is brl/bpro/core/vil_pro/vil_map_image_binary_process.cxx
#include "vil_map_image_binary_process.h"
//:
// \file

#include "vil_math_functors.h"
#include <vil/vil_convert.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_transform.h>
#include <vil/vil_math.h>

//: Constructor
vil_map_image_binary_process::vil_map_image_binary_process()
{
  //input
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0]="vil_image_view_base_sptr";
  input_types_[1]="vil_image_view_base_sptr";
  input_types_[2]="vcl_string";

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";
}


//: Destructor
vil_map_image_binary_process::~vil_map_image_binary_process()
{
}


//: Execute the process
bool
vil_map_image_binary_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //Retrieve image from input
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr image1 = input0->value();

  //Retrieve image from input
  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr image2 = input1->value();

  //Retrieve map functor name
  brdb_value_t<vcl_string>* input2 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[2].ptr());
  vcl_string functor = input2->value();

  // convert image to float
  vil_image_view_base_sptr fimage1 = vil_convert_cast(float(), image1);
  vil_image_view<float>fimg1 = *fimage1;
  vil_image_view_base_sptr fimage2 = vil_convert_cast(float(), image2);
  vil_image_view<float>fimg2 = *fimage2;

  vil_image_view<float> dest(fimg1.ni(), fimg1.nj(), fimg1.nplanes());
  // apply the functor
  if (functor == "add"){
    vcl_cout << "in add\n";
    vil_math_add_functor af;
    vil_transform(fimg1, fimg2, dest, af);
  }
  else{
    vcl_cerr << "In vil_map_image_binary_process::execute() - unknown functor "
             << functor << '\n';
    return false;
  }

  //for now, only handle byte or float outputs
  vil_image_view_base_sptr map_image;
  if (image1->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<vxl_byte>* temp = new vil_image_view<vxl_byte>;
    vil_convert_stretch_range(dest, *temp);
    map_image = temp;
  }
  else //leave the image as float
    map_image = new vil_image_view<float>(dest);

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(map_image);
  output_data_[0] = output0;
  return true;
}

