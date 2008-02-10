// This is brl/bseg/bbgm/pro/bbgm_save_image_of_process.cxx

//:
// \file

#include "bbgm_save_image_of_process.h"
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>

//: Constructor
bbgm_save_image_of_process::bbgm_save_image_of_process()
{
  //input  
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]= "vcl_string";
  input_types_[1]="bbgm_image_sptr";

  //output - none
}


//: Destructor
bbgm_save_image_of_process::~bbgm_save_image_of_process()
{
}


//: Execute the process
bool
bbgm_save_image_of_process::execute()
{
  // Sanity check
    if(!this->verify_inputs())
    return false;
  
  //Retrieve filename from input
  brdb_value_t<vcl_string>* input0 = 
    static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());

  vcl_string binary_filename = input0->value();
 
  vsl_b_ofstream ostr(binary_filename);
  if(!ostr){
    vcl_cerr << "Failed to load background image from " 
             << binary_filename << vcl_endl;
    return false;
  }

  vsl_b_ostream& bos = static_cast<vsl_b_ostream>(ostr);

  // Retrieve background image
  brdb_value_t<bbgm_image_sptr>* input1 = 
    static_cast<brdb_value_t<bbgm_image_sptr>* >(input_data_[1].ptr());

  bbgm_image_sptr bgm = input1->value();

  vsl_b_write(bos, bgm);

  return true;
}




