// This is brl/bpro/core/vidl2_pro/processes/vidl2_open_dshow_istream_process.cxx
#if HAS_DIRECTSHOW
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vidl2/vidl2_dshow_file_istream.h>
#include <vidl2/vidl2_istream_sptr.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

//: Constructor
bool vidl2_open_dshow_istream_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");   // the path of the stream
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vidl2_ostream_sptr");  
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
// NOTE! currently only implemented for image list istream
bool vidl2_open_dshow_istream_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    vcl_cout << "vidl2_open_dshow_istream_process: The input number should be 1" << vcl_endl;
    return false;
  }
 
  unsigned i=0;
  //Retrieve filename from input
  vcl_string istream_filename = pro.get_input<vcl_string>(i++);

  vidl2_istream_sptr out_istream=new vidl2_dshow_file_istream(istream_filename);

  if (!out_istream->is_open()) {
    vcl_cerr << "In vidl2_open_dshow_istream_process::execute()"
             << " - could not open" << istream_filename << vcl_endl;
    return false;
  }

  vcl_cout << "In vidl2_open_dshow_istream_process::execute()"
           << " - opened the stream with " << out_istream->num_frames() << " frames." << vcl_endl;

  pro.set_output_val<vidl2_ostream_sptr>(0,out_istream);
  return true;
}

#endif