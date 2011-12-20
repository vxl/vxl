// This is brl/bpro/core/vil_pro/processes/bil_create_raw_image_istream_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view_base.h>
#include <bil/bil_raw_image_istream.h>
#include <vcl_sstream.h>

//: Constructor
bool bil_create_raw_image_istream_process_cons(bprb_func_process& pro)
{
  //process takes 1 input
  vcl_vector<vcl_string> input_types_(1);
  input_types_[0] = "vcl_string"; //raw file 

  // process has 1 output:
  vcl_vector<vcl_string>  output_types_(2);
  output_types_[0] = "bil_raw_image_istream_sptr";     //an initialized istream_sptr
  output_types_[1] = "int"; 
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: Execute the process
bool bil_create_raw_image_istream_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()<1) {
    vcl_cout << "bil_create_raw_image_istream_process: The input number should be 1" << vcl_endl;
    return false;
  }
  //Retrieve filename from input
  vcl_string raw_file = pro.get_input<vcl_string>(0);

  //: Constructor - from a file glob string
  bil_raw_image_istream_sptr stream = new bil_raw_image_istream(raw_file);
  pro.set_output_val<bil_raw_image_istream_sptr>(0, stream);
  pro.set_output_val<int> (1, stream->num_frames()); 
  return true;
}


//---------------------------------------------------------------
// get next image from stream process
//---------------------------------------------------------------
//: Constructor
bool bil_read_frame_process_cons(bprb_func_process& pro)
{
  //process takes 1 input
  vcl_vector<vcl_string> input_types_(1);
  input_types_[0] = "bil_raw_image_istream_sptr"; //raw file 

  // process has 1 output:
  vcl_vector<vcl_string>  output_types_(2);
  output_types_[0] = "vil_image_view_base_sptr";     //an initialized istream_sptr
  output_types_[1] = "unsigned";                     //time stamp
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: Execute the process
bool bil_read_frame_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()<1) {
    vcl_cout << "bil_create_raw_image_istream_process: The input number should be 1" << vcl_endl;
    return false;
  }
  //Retrieve filename from input
  bil_raw_image_istream_sptr stream = pro.get_input<bil_raw_image_istream_sptr>(0);
  vil_image_view_base_sptr   img    = stream->read_frame();
  unsigned                   time   = stream->time_stamp(); 

  //out
  pro.set_output_val<vil_image_view_base_sptr>(0, img);
  pro.set_output_val<unsigned>(1, time); 
  return true;
}


