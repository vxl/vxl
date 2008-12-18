// This is brl/bpro/core/vidl2_pro/vidl2_get_frame_istream_process.cxx
#include "vidl2_get_frame_istream_process.h"
//:
// \file

#include <vidl2/vidl2_image_list_istream.h>
#include <vidl2/vidl2_istream_sptr.h>
#include <vidl2/vidl2_convert.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

//: Constructor
vidl2_get_frame_istream_process::vidl2_get_frame_istream_process()
{
  //input
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]="vidl2_istream_sptr";
  input_types_[1]="int";   // frame number to seek 

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";
}


//: Destructor
vidl2_get_frame_istream_process::~vidl2_get_frame_istream_process()
{
}


//: Execute the process
// NOTE! currently only implemented for image list istream
bool
vidl2_get_frame_istream_process::execute()
{
  // Sanity check
    if (!this->verify_inputs())
    return false;

  //Retrieve filename from input
  brdb_value_t<vidl2_istream_sptr>* input0 =
    static_cast<brdb_value_t<vidl2_istream_sptr>* >(input_data_[0].ptr());

  vidl2_istream_sptr istream = input0->value();

  if (!istream->is_open()){
    vcl_cerr << "In vidl2_get_frame_istream_process::execute()"
             << " - input stream is not open" << vcl_endl;
    return false;
  }

  //Retrieve frame number
  brdb_value_t<int>* input1 = static_cast<brdb_value_t<int>* >(input_data_[1].ptr());
  int f_n = input1->value();

  if (istream->frame_number() != f_n) {
    if (!istream->seek_frame(f_n)) { // seeking does not work
      while (istream->advance()) {
        if (istream->frame_number() == f_n)
          break;
      }
    }
  }

  vcl_cout << "retrieving frame #: " << istream->frame_number() << vcl_endl;
  vcl_cout.flush();

  vidl2_frame_sptr f = istream->current_frame();
  vil_image_view_base_sptr fb = vidl2_convert_wrap_in_view(*f);
  if (!fb)
    return false;

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(fb);
  output_data_[0] = output0;

  return true;
}

