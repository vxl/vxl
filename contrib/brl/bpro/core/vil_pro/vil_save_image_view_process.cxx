// This is brl/bipp/process/vil_save_image_view_process.cxx

//:
// \file

#include "vil_save_image_view_process.h"

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view_base.h>

//: Constructor
vil_save_image_view_process::vil_save_image_view_process()
{
  //this process takes two inputs:
  // input(0): the vil_image_view_base_sptr
  // input(1): the filename to save to
  
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]="vil_image_view_base_sptr";
  input_types_[1]="vcl_string";

  //this process has no outputs
  output_data_.resize(0);
  output_types_.resize(0);

//: Destructor
vil_save_image_view_process::~vil_save_image_view_process()
{
}


//: Execute the process
bool
vil_save_image_view_process::execute()
{
  // Sanity check
  if(!this->verify_inputs())
    return false;

  //Retrieve image from input
  brdb_value_t<vil_image_view_base_sptr>* input0 = 
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  
  vil_image_view_base_sptr img = input0->value();

  //Retrieve filename from input
  brdb_value_t<vcl_string>* input1 = 
    static_cast<brdb_value_t<vcl_string>* >(input_data_[1].ptr());

  vcl_string image_filename = input1->value();
 
  bool result = vil_save(*img,image_filename.c_str());
  if( !result ) {
    vcl_cerr << "Failed to save image to" << image_filename << vcl_endl;
    return false;
  }

  return true;
}




