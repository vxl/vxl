// This is brl/bpro/core/vil_pro/vil_rgbi_to_grey_process.cxx
#include "vil_rgbi_to_grey_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>

//: Constructor
vil_rgbi_to_grey_process::vil_rgbi_to_grey_process()
{
  //this process takes two inputs:
  // input(0): the vil_image_view_base_sptr
  // input(1): the filename to save to

  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0]="vil_image_view_base_sptr";

  //this process has no outputs
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]="vil_image_view_base_sptr";
}

//: Destructor
vil_rgbi_to_grey_process::~vil_rgbi_to_grey_process()
{
}


//: Execute the process
bool
vil_rgbi_to_grey_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //Retrieve image from input
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());

  vil_image_view_base_sptr img = input0->value();
  vil_image_view<unsigned char> *out_img=new vil_image_view<unsigned char>(img->ni(),img->nj());

  vil_convert_planes_to_grey<unsigned char,unsigned char>(*(img.as_pointer()),*out_img);
  vil_image_view_base_sptr out_img_ptr=out_img;


  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(out_img_ptr);
  output_data_[0] = output0;

  return true;
}

