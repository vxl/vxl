// This is brl/bpro/core/vil_pro/vil_convert_to_n_planes_process.cxx
#include "vil_convert_to_n_planes_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>

//: Constructor
vil_convert_to_n_planes_process::vil_convert_to_n_planes_process()
{
  //this process takes two inputs:
  // input(0): the input image vil_image_view_base_sptr
  // input(1): the number of planes in the output image 
   input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]="vil_image_view_base_sptr";
  input_types_[1]="unsigned";

  //this process has 1 outputs
  // output(0): the floating point image with the specified number of planes
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]="vil_image_view_base_sptr";
}

//: Destructor
vil_convert_to_n_planes_process::~vil_convert_to_n_planes_process()
{
}


//: Execute the process
bool
vil_convert_to_n_planes_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //Retrieve image from input
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());

  vil_image_view_base_sptr img = input0->value();


  brdb_value_t<unsigned>* input1 =
        static_cast<brdb_value_t<unsigned>* >(input_data_[1].ptr());

    unsigned nplanes = input1->value();

  vil_image_view<float> out_float_view = *(vil_convert_cast(float(),vil_convert_to_n_planes(nplanes, img)));
  vil_image_view_base_sptr out_img_ptr= new vil_image_view<float>(out_float_view);


  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(out_img_ptr);
  output_data_[0] = output0;

  return true;
}

