// This is brl/bpro/core/vidl_pro/processes/vidl_open_dshow_istream_process.cxx
#if defined(HAS_DIRECTSHOW) && HAS_DIRECTSHOW
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vidl/vidl_dshow_file_istream.h>
#include <vidl/vidl_istream_sptr.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool vidl_open_dshow_istream_process_cons(bprb_func_process& pro)
{
  //input
  std::vector<std::string> input_types;
  input_types.push_back("vcl_string");   // the path of the stream
  if (! pro.set_input_types(input_types))
    return false;

  //output
  std::vector<std::string> output_types;
  output_types.push_back("vidl_ostream_sptr");
  return pro.set_output_types(output_types);
}

//: Execute the process
// NOTE! currently only implemented for image list istream
bool vidl_open_dshow_istream_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 1) {
    std::cout << "vidl_open_dshow_istream_process: The input number should be 1" << std::endl;
    return false;
  }

  //Retrieve filename from input
  std::string istream_filename = pro.get_input<std::string>(0);

  vidl_istream_sptr out_istream=new vidl_dshow_file_istream(istream_filename);

  if (!out_istream->is_open()) {
    std::cerr << "In vidl_open_dshow_istream_process::execute()"
             << " - could not open" << istream_filename << std::endl;
    return false;
  }

  std::cout << "In vidl_open_dshow_istream_process::execute()"
           << " - opened the stream with " << out_istream->num_frames()
           << " frames." << std::endl;

  pro.set_output_val<vidl_ostream_sptr>(0,out_istream);
  return true;
}

#endif // defined(HAS_DIRECTSHOW) && HAS_DIRECTSHOW
