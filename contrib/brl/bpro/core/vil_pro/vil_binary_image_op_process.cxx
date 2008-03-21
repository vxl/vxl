// This is brl/bpro/core/vil_pro/vil_binary_image_op_process.cxx

//:
// \file


#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vcl_iostream.h>
#include "vil_binary_image_op_process.h"
//: Constructor
vil_binary_image_op_process::vil_binary_image_op_process()
{
  //input  
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0]= "vil_image_view_base_sptr"; //image a
  input_types_[1]= "vil_image_view_base_sptr"; //image b
  input_types_[2]= "vcl_string"; //operation 
  
  //output 
  output_data_.resize(1, brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";//binary operation
}


//: Destructor
vil_binary_image_op_process::~vil_binary_image_op_process()
{
}


//: Execute the process
bool
vil_binary_image_op_process::execute()
{
  // Sanity check
  if(!this->verify_inputs()){
    vcl_cerr << "In vil_binary_image_op_process::execute() -"
             << " invalid inputs\n";
    return false;
  }
  //Retrieve image a from input
  brdb_value_t<vil_image_view_base_sptr>* input0 = 
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr img_ptr_a = input0->value();
  vil_image_view<float> view_a = *(vil_convert_cast(float(), img_ptr_a));
                                   
  //Retrieve image b from input
  brdb_value_t<vil_image_view_base_sptr>* input1 = 
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr img_ptr_b = input1->value();
  vil_image_view<float> view_b = *(vil_convert_cast(float(), img_ptr_b));

  brdb_value_t<vcl_string>* input2 = 
    static_cast<brdb_value_t<vcl_string>* >(input_data_[2].ptr());
  vcl_string operation = input2->value();

  vil_image_view<float> result;

  //test for operation
  if(operation=="product")
    vil_math_image_product(view_a, view_b, result);
  else{
    vcl_cerr << "In vil_binary_image_op_process::execute() -"
            << " unknown binary operation\n";
    return false;
  }
  brdb_value_sptr output0 = 
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(result));
  output_data_[0] = output0;

  return true;
}




