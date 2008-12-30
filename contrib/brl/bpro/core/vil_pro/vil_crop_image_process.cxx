// This is brl/bpro/core/vil_pro/vil_crop_image_process.cxx
#include "vil_crop_image_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_new.h>

//: Constructor
vil_crop_image_process::vil_crop_image_process()
{
  //input
  input_data_.resize(5,brdb_value_sptr(0));
  input_types_.resize(5);
  input_types_[0]="vil_image_view_base_sptr";
  input_types_[1]="unsigned"; // i0
  input_types_[2]="unsigned"; // j0
  input_types_[3]="unsigned"; // ni
  input_types_[4]="unsigned"; // nj

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";
}


//: Destructor
vil_crop_image_process::~vil_crop_image_process()
{
}


//: Execute the process
bool
vil_crop_image_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //Retrieve image from input
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr image = input0->value();
  vil_image_resource_sptr image_ptr = vil_new_image_resource_of_view(*image);

  //Retrieve limits
  brdb_value_t<unsigned>* input1 = static_cast<brdb_value_t<unsigned>* >(input_data_[1].ptr());
  unsigned i0 = input1->value();
  brdb_value_t<unsigned>* input2 = static_cast<brdb_value_t<unsigned>* >(input_data_[2].ptr());
  unsigned j0 = input2->value();
  brdb_value_t<unsigned>* input3 = static_cast<brdb_value_t<unsigned>* >(input_data_[3].ptr());
  unsigned ni = input3->value();
  brdb_value_t<unsigned>* input4 = static_cast<brdb_value_t<unsigned>* >(input_data_[4].ptr());
  unsigned nj = input4->value();
  
  vil_image_resource_sptr out_img = vil_crop(image_ptr, i0, ni, j0, nj);
  vil_image_view_base_sptr out_sptr = vil_new_image_view_base_sptr(*(out_img->get_view()));
  
  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(out_sptr);
  output_data_[0] = output0;
  return true;
}

