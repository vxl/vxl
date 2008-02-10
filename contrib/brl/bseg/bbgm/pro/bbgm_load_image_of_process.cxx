// This is brl/bseg/bbgm/pro/bbgm_load_image_of_process.cxx

//:
// \file

#include "bbgm_load_image_of_process.h"
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <bbgm/bbgm_loader.h>

//: Constructor
bbgm_load_image_of_process::bbgm_load_image_of_process()
{
  //input  
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0]="vcl_string";

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "bbgm_image_sptr";

  //register different distributions for image content
  //the registration will only be done once since new instances of 
  //the process are cloned  - maybe later make a separate registration step
  bbgm_loader::register_loaders();
}


//: Destructor
bbgm_load_image_of_process::~bbgm_load_image_of_process()
{
}


//: Execute the process
bool
bbgm_load_image_of_process::execute()
{
  // Sanity check
    if(!this->verify_inputs())
    return false;
  
  //Retrieve filename from input
  brdb_value_t<vcl_string>* input0 = 
    static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());

  vcl_string binary_filename = input0->value();
 
  vsl_b_ifstream istr(binary_filename);
  if(!istr){
    vcl_cerr << "Failed to load background image from " 
             << binary_filename << vcl_endl;
    return false;
  }

  vsl_b_istream& bis = static_cast<vsl_b_istream&>(istr);
  bbgm_image_sptr image;
  vsl_b_read(bis, image);
  brdb_value_sptr output0 = new brdb_value_t<bbgm_image_sptr>(image);
  
  output_data_[0] = output0;

  return true;
}




