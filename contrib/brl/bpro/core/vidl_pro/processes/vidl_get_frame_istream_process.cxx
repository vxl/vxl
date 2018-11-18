// This is brl/bpro/core/vidl_pro/processes/vidl_get_frame_istream_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_istream.h>
#include <vidl/vidl_convert.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool vidl_get_frame_istream_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vidl_istream_sptr");
  input_types.emplace_back("int");   // frame number to seek
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool vidl_get_frame_istream_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 2) {
    std::cout << "vidl_get_frame_istream_process: The input number should be 2" << std::endl;
    return false;
  }

  unsigned i=0;
  vidl_istream_sptr i_stream = pro.get_input<vidl_istream_sptr>(i++);

  if (!i_stream->is_open()){
    std::cerr << "In vidl_get_frame_istream_process::execute()"
             << " - input stream is not open\n";
    return false;
  }

  //Retrieve frame number
  unsigned int f_n = pro.get_input<int>(i++);

  if (i_stream->frame_number() != f_n) {
    if (!i_stream->seek_frame(f_n)) { // seeking does not work
      while (i_stream->advance()) {
        if (i_stream->frame_number() == f_n)
          break;
      }
    }
  }

  std::cout << "retrieving frame #: " << i_stream->frame_number() << std::endl;
  std::cout.flush();

  vidl_frame_sptr f = i_stream->current_frame();
  vil_image_view_base_sptr fb = vidl_convert_wrap_in_view(*f);
  if (!fb)
    return false;

  pro.set_output_val<vil_image_view_base_sptr>(0,fb);
  return true;
}
