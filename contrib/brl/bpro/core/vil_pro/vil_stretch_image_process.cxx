// This is brl/bpro/core/vil_pro/vil_stretch_image_process.cxx
#include "vil_stretch_image_process.h"
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
vil_stretch_image_process::vil_stretch_image_process()
{
  //input
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0]="vil_image_view_base_sptr";
  input_types_[1]="float"; // min limit
  input_types_[2]="float"; // max limit

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";
}


//: Destructor
vil_stretch_image_process::~vil_stretch_image_process()
{
}


//: Execute the process
bool
vil_stretch_image_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //Retrieve image from input
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr image = input0->value();

  //Retrieve limits
  brdb_value_t<float>* input1 =
    static_cast<brdb_value_t<float>* >(input_data_[1].ptr());
  float min_limit = input1->value();

  brdb_value_t<float>* input2 =
    static_cast<brdb_value_t<float>* >(input_data_[2].ptr());
  float max_limit = input2->value();

  // retrieve float image
  vil_image_view_base_sptr fimage = vil_convert_cast(float(), image);
  vil_image_view<float>fimg = *fimage;

    vil_image_view<vxl_byte>* temp = new vil_image_view<vxl_byte>;
    vil_convert_stretch_range_limited(fimg, *temp, min_limit, max_limit);

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(temp);
  output_data_[0] = output0;
  return true;
}

