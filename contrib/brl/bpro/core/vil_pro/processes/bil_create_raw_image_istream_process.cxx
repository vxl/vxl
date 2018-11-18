// This is brl/bpro/core/vil_pro/processes/bil_create_raw_image_istream_process.cxx
#include <iostream>
#include <string>
#include <sstream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view_base.h>
#include <bil/bil_raw_image_istream.h>

//: Constructor
bool bil_create_raw_image_istream_process_cons(bprb_func_process& pro)
{
  //process takes 1 input
  std::vector<std::string> input_types_(4);
  input_types_[0] = "vcl_string"; //raw file
  input_types_[1] = "int"; //raw file
  input_types_[2] = "int"; //raw file
  input_types_[3] = "int"; //raw file

  // process has 2 outputs
  std::vector<std::string>  output_types_(2);
  output_types_[0] = "bil_raw_image_istream_sptr";     //an initialized istream_sptr
  output_types_[1] = "int";
  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}


//: Execute the process
bool bil_create_raw_image_istream_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()<1) {
    std::cout << "bil_create_raw_image_istream_process: The number of inputs should be 1" << std::endl;
    return false;
  }
  //Retrieve filename from input
  std::string raw_file = pro.get_input<std::string>(0);
  int ni = pro.get_input<int>(1);
  int nj = pro.get_input<int>(2);
  int pixelsize = pro.get_input<int>(3);

  //: Constructor - from a file glob string
  bil_raw_image_istream_sptr stream;
  if(ni > 0 && nj >0 && pixelsize > 0)
  {
      stream =  new bil_raw_image_istream();
      stream->open(raw_file);//,ni,nj,pixelsize);
  }
  else
       stream =  new bil_raw_image_istream(raw_file);

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
  std::vector<std::string> input_types_(1);
  input_types_[0] = "bil_raw_image_istream_sptr"; //raw file

  // process has 1 output:
  std::vector<std::string>  output_types_(1);
  output_types_[0] = "vil_image_view_base_sptr";     //an initialized istream_sptr
  //output_types_[1] = "unsigned";                     //time stamp
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: Execute the process
bool bil_read_frame_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()<1) {
    std::cout << "bil_create_raw_image_istream_process: The number of inputs should be 1" << std::endl;
    return false;
  }
  //Retrieve filename from input
  bil_raw_image_istream_sptr stream = pro.get_input<bil_raw_image_istream_sptr>(0);
  vil_image_view_base_sptr   img    = stream->read_frame();
  //unsigned                   time   = stream->time_stamp();

  //out
  pro.set_output_val<vil_image_view_base_sptr>(0, img);
  //pro.set_output_val<unsigned>(1, time);
  return true;
}


//---------------------------------------------------------------
// seek stream to a certain image
//---------------------------------------------------------------
//: Constructor
bool bil_seek_frame_process_cons(bprb_func_process& pro)
{
  //process takes 2 inputs
  std::vector<std::string> input_types_(2);
  input_types_[0] = "bil_raw_image_istream_sptr"; //raw file
  input_types_[1] = "unsigned"; //frame to seek to

  // process has 2 outputs
  std::vector<std::string>  output_types_(1);
  output_types_[0] = "vil_image_view_base_sptr";     //an initialized istream_sptr
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: Execute the process
bool bil_seek_frame_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()<2) {
    std::cout << "bil_create_raw_image_istream_process: The number of inputs should be 2" << std::endl;
    return false;
  }

  //Retrieve filename from input
  bil_raw_image_istream_sptr stream = pro.get_input<bil_raw_image_istream_sptr>(0);
  auto                   frame  = pro.get_input<unsigned>(1);

      //seek, retrieve image, and output
  stream->seek_frame(frame);

  vil_image_view_base_sptr   img    = stream->current_frame();
  //out
  pro.set_output_val<vil_image_view_base_sptr>(0, img);
  return true;
}
