// This is brl/bpro/core/vidl_pro/processes/vidl_open_istream_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_istream_sptr.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

//: Constructor
bool vidl_open_istream_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vidl_istream_sptr");  
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
// NOTE! currently only implemented for image list istream
bool vidl_open_istream_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 2) {
    vcl_cout << "vidl_open_ostream_process: The input number should be 2" << vcl_endl;
    return false;
  }
 
  unsigned i=0;
  //Retrieve filename from input
  vcl_string istream_filename = pro.get_input<vcl_string>(i++);

  vidl_istream_sptr vistr = new vidl_image_list_istream(istream_filename);
  if (!vistr->is_open()){
    vcl_cerr << "In vidl_open_istream_process::execute()"
             << " - could not open" << istream_filename << vcl_endl;
    return false;
  }

  pro.set_output_val<vidl_istream_sptr>(0, vistr);
  return true;
}

