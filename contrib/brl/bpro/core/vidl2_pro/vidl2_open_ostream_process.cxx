// This is brl/bpro/core/vidl2_pro/vidl2_open_ostream_process.cxx
#include "vidl2_open_ostream_process.h"
//:
// \file

#include <vul/vul_file.h>
#include <vidl2/vidl2_image_list_ostream.h>
#include <vidl2/vidl2_ostream_sptr.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

//: Constructor
vidl2_open_ostream_process::vidl2_open_ostream_process()
{
  //input
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]="vcl_string";
  input_types_[1]="bool";

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vidl2_ostream_sptr";
}


//: Destructor
vidl2_open_ostream_process::~vidl2_open_ostream_process()
{
}


//: Execute the process
// NOTE! currently only implemented for image list ostream
bool
vidl2_open_ostream_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In vidl2_open_ostream_process::execute() - "
             << " verify inputs failed" << '\n';
    return false;
  }

  //Retrieve filename from input
  brdb_value_t<vcl_string>* input0 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());
  vcl_string ostream_path = input0->value();

  //Retrieve create directory flag from input
  brdb_value_t<bool>* input1 =
    static_cast<brdb_value_t<bool>* >(input_data_[1].ptr());
  bool create_directory = input1->value();

  if (!vul_file::is_directory(ostream_path)){
    //directory not present
    if (create_directory)
      if (!vul_file::make_directory_path(ostream_path)){
        vcl_cerr << "In vidl2_open_ostream_process::execute() - "
                 << " can't make directory with path " << ostream_path << '\n';
        return false;
      }
      else{
        vcl_cerr << "In vidl2_open_ostream_process::execute() - "
                 << " directory doesn't exist with path " << ostream_path << '\n';
        return false;
      }
  }
  vidl2_ostream_sptr vostr = new vidl2_image_list_ostream(ostream_path);
  if (!vostr->is_open()){
    vcl_cerr << "In vidl2_open_ostream_process::execute()"
             << " - could not open" << ostream_path << vcl_endl;
    return false;
  }

  brdb_value_sptr output0 = new brdb_value_t<vidl2_ostream_sptr>(vostr);

  output_data_[0] = output0;

  return true;
}

