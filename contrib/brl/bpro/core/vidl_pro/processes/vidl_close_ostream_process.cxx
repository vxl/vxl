// This is brl/bpro/core/vidl_pro/processes/vidl_close_ostream_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vidl/vidl_ostream_sptr.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

//: Constructor
bool vidl_close_ostream_process_cons(bprb_func_process& pro)
{
  //input
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vidl_ostream_sptr");   
  return pro.set_input_types(input_types);

  //no output
}

//: Execute the process
bool vidl_close_ostream_process(bprb_func_process& pro)
{
  // Sanity check
  if(!pro.verify_inputs()){
    vcl_cout << "vidl_close_ostream_process: invalid inputs" << vcl_endl;
    return false;
  }
 
  unsigned i=0;
  vidl_ostream_sptr o_stream = pro.get_input<vidl_ostream_sptr>(i++);

  if (!o_stream->is_open()) {
    vcl_cerr << "In vidl_close_ostream_process::execute()"
             << " - is already closed\n";
    return false;
  }
  o_stream->close();
  return true;
}

