// This is brl/bpro/core/vidl2_pro/vidl2_close_istream_process.cxx
#include "vidl2_close_istream_process.h"
//:
// \file

#include <vidl2/vidl2_istream_sptr.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

//: Constructor
vidl2_close_istream_process::vidl2_close_istream_process()
{
  //input
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0]="vidl2_istream_sptr";

  //output
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
}


//: Destructor
vidl2_close_istream_process::~vidl2_close_istream_process()
{
}


//: Execute the process
bool
vidl2_close_istream_process::execute()
{
  // Sanity check
    if (!this->verify_inputs())
    return false;

  //Retrieve filename from input
  brdb_value_t<vidl2_istream_sptr>* input0 = static_cast<brdb_value_t<vidl2_istream_sptr>* >(input_data_[0].ptr());

  vidl2_istream_sptr i_stream = input0->value();

  if (!i_stream->is_open()) {
    vcl_cerr << "In vidl2_close_istream_process::execute()"
             << " - is already closed\n";
    return false;
  }

  i_stream->close();
  return true;
}

