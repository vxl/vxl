// This is brl/bpro/core/vidl_pro/processes/vidl_close_istream_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vidl/vidl_istream_sptr.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

//: Constructor
bool vidl_close_istream_process_cons(bprb_func_process& pro)
{
  //input
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vidl_istream_sptr");   
  return pro.set_input_types(input_types);

  //no output
}

//: Execute the process
bool vidl_close_istream_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    vcl_cout << "vidl_close_istream_process: The input number should be 1" << vcl_endl;
    return false;
  }
 
  unsigned i=0;
  vidl_istream_sptr i_stream = pro.get_input<vidl_istream_sptr>(i++);

  if (!i_stream->is_open()) {
    vcl_cerr << "In vidl_close_istream_process::execute()"
             << " - is already closed\n";
    return false;
  }

  i_stream->close();
  return true;
}

