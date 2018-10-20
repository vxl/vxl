#include "bprb_null_process.h"

bprb_null_process::bprb_null_process()
{
  input_data_.resize(6, brdb_value_sptr(nullptr));
  output_data_.resize(6,brdb_value_sptr(nullptr));
  input_types_.resize(6);
  output_types_.resize(6);
  input_types_[0]="vcl_string";
  input_types_[1]="bool";
  input_types_[2]="int";
  input_types_[3]="long";
  input_types_[4]="float";
  input_types_[5]="double";
  output_types_[0]="vcl_string";
  output_types_[1]="bool";
  output_types_[2]="int";
  output_types_[3]="long";
  output_types_[4]="float";
  output_types_[5]="double";
}

bool bprb_null_process::execute()
{
  //sanity check
  if(!this->verify_inputs())
    return false;
  output_data_ = input_data_;
  return true;
}
