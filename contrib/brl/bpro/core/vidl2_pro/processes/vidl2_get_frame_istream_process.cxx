// This is brl/bpro/core/vidl2_pro/processes/vidl2_get_frame_istream_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vidl2/vidl2_image_list_istream.h>
#include <vidl2/vidl2_istream_sptr.h>
#include <vidl2/vidl2_convert.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

//: Constructor
bool vidl2_get_frame_istream_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vidl2_istream_sptr");   
  input_types.push_back("int");   // frame number to seek
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
// NOTE! currently only implemented for image list istream
bool vidl2_get_frame_istream_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 2) {
    vcl_cout << "vidl2_get_frame_istream_process: The input number should be 2" << vcl_endl;
    return false;
  }
 
  unsigned i=0;
  vidl2_istream_sptr i_stream = pro.get_input<vidl2_istream_sptr>(i++);

  if (!i_stream->is_open()){
    vcl_cerr << "In vidl2_get_frame_istream_process::execute()"
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

  vcl_cout << "retrieving frame #: " << i_stream->frame_number() << vcl_endl;
  vcl_cout.flush();

  vidl2_frame_sptr f = i_stream->current_frame();
  vil_image_view_base_sptr fb = vidl2_convert_wrap_in_view(*f);
  if (!fb)
    return false;

  pro.set_output_val<vil_image_view_base_sptr>(0,fb);
  return true;
}

