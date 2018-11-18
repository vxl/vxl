// This is brl/bpro/core/vidl_pro/processes/vidl_close_istream_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vidl/vidl_istream_sptr.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool vidl_close_istream_process_cons(bprb_func_process& pro)
{
  //input
  std::vector<std::string> input_types;
  input_types.emplace_back("vidl_istream_sptr");
  return pro.set_input_types(input_types);

  //no output
}

//: Execute the process
bool vidl_close_istream_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "vidl_close_istream_process: The input number should be 1" << std::endl;
    return false;
  }

  unsigned i=0;
  vidl_istream_sptr i_stream = pro.get_input<vidl_istream_sptr>(i++);

  if (!i_stream->is_open()) {
    std::cerr << "In vidl_close_istream_process::execute()"
             << " - is already closed\n";
    return false;
  }

  i_stream->close();
  return true;
}
