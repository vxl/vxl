// This is brl/bpro/core/vidl2_pro/vidl2_open_dshow_istream_process.cxx
#include "vidl2_open_dshow_istream_process.h"
//:
// \file

#include <vidl2/vidl2_dshow_file_istream.h>
#include <vidl2/vidl2_istream_sptr.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

//: Constructor
vidl2_open_dshow_istream_process::vidl2_open_dshow_istream_process()
{
  //input
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0]="vcl_string";  // the path of the stream

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vidl2_istream_sptr";
}


//: Destructor
vidl2_open_dshow_istream_process::~vidl2_open_dshow_istream_process()
{
}


//: Execute the process
// NOTE! currently only implemented for image list istream
bool
vidl2_open_dshow_istream_process::execute()
{
  // Sanity check
    if (!this->verify_inputs())
    return false;

  //Retrieve filename from input
  brdb_value_t<vcl_string>* input0 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());

  vcl_string istream_filename = input0->value();

  vidl2_istream_sptr out_istream=new vidl2_dshow_file_istream(istream_filename);

  if (!out_istream->is_open()) {
    vcl_cerr << "In vidl2_open_dshow_istream_process::execute()"
             << " - could not open" << istream_filename << vcl_endl;
    return false;
  }

  vcl_cout << "In vidl2_open_dshow_istream_process::execute()"
           << " - opened the stream with " << out_istream->num_frames() << " frames." << vcl_endl;

  brdb_value_sptr output0 = new brdb_value_t<vidl2_istream_sptr>(out_istream);

  output_data_[0] = output0;

  return true;
}

