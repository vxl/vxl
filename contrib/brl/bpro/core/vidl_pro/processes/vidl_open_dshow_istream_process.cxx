// This is brl/bpro/core/vidl_pro/processes/vidl_open_dshow_istream_process.cxx
#if defined(HAS_DIRECTSHOW) && HAS_DIRECTSHOW
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vidl/vidl_dshow_file_istream.h>
#include <vidl/vidl_istream_sptr.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

//: Constructor
bool vidl_open_dshow_istream_process_cons(bprb_func_process& pro)
{
  //input
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");   // the path of the stream
  if (! pro.set_input_types(input_types))
    return false;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vidl_ostream_sptr");
  return pro.set_output_types(output_types);
}

//: Execute the process
// NOTE! currently only implemented for image list istream
bool vidl_open_dshow_istream_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 1) {
    vcl_cout << "vidl_open_dshow_istream_process: The input number should be 1" << vcl_endl;
    return false;
  }

  //Retrieve filename from input
  vcl_string istream_filename = pro.get_input<vcl_string>(0);

  vidl_istream_sptr out_istream=new vidl_dshow_file_istream(istream_filename);

  if (!out_istream->is_open()) {
    vcl_cerr << "In vidl_open_dshow_istream_process::execute()"
             << " - could not open" << istream_filename << vcl_endl;
    return false;
  }

  vcl_cout << "In vidl_open_dshow_istream_process::execute()"
           << " - opened the stream with " << out_istream->num_frames()
           << " frames." << vcl_endl;

  pro.set_output_val<vidl_ostream_sptr>(0,out_istream);
  return true;
}

#endif // defined(HAS_DIRECTSHOW) && HAS_DIRECTSHOW
