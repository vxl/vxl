// This is brl/bpro/core/vidl_pro/processes/vidl_put_frame_ostream_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
#include <vidl/vidl_ostream_sptr.h>
#include <vidl/vidl_ostream.h>
#include <vidl/vidl_convert.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool vidl_put_frame_ostream_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vidl_ostream_sptr"); // the output stream
  input_types.emplace_back("vil_image_view_base_sptr");// the frame image view
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output  none
  return ok;
}

//: Execute the process
bool vidl_put_frame_ostream_process(bprb_func_process& pro)
{
  // Sanity check
  if(!pro.verify_inputs()){
    std::cout << "vidl_put_frame_ostream_process: invalid inputs" << std::endl;
    return false;
  }
  unsigned i=0;
  vidl_ostream_sptr o_stream = pro.get_input<vidl_ostream_sptr>(i++);

  if (!o_stream->is_open()){
    std::cerr << "In vidl_put_frame_ostream_process::execute()"
             << " - output stream is not open\n";
    return false;
  }
  vil_image_view_base_sptr view = pro.get_input<vil_image_view_base_sptr>(i);
  vidl_frame_sptr frame = vidl_convert_to_frame(view);
  if(!o_stream->write_frame(frame)){
    std::cerr << "In vidl_put_frame_ostream_process::execute()"
             << " - could not write frame\n";
    return false;
  }
  return true;
}
