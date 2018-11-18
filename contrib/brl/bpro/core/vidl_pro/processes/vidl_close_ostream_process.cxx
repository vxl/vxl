// This is brl/bpro/core/vidl_pro/processes/vidl_close_ostream_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vidl/vidl_ostream_sptr.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool vidl_close_ostream_process_cons(bprb_func_process& pro)
{
  //input
  std::vector<std::string> input_types;
  input_types.emplace_back("vidl_ostream_sptr");
  return pro.set_input_types(input_types);

  //no output
}

//: Execute the process
bool vidl_close_ostream_process(bprb_func_process& pro)
{
  // Sanity check
  if(!pro.verify_inputs()){
    std::cout << "vidl_close_ostream_process: invalid inputs" << std::endl;
    return false;
  }

  unsigned i=0;
  vidl_ostream_sptr o_stream = pro.get_input<vidl_ostream_sptr>(i++);

  if (!o_stream->is_open()) {
    std::cerr << "In vidl_close_ostream_process::execute()"
             << " - is already closed\n";
    return false;
  }
  o_stream->close();
  return true;
}
