// This is brl/bbas/bhdfs/pro/processes/bhdfs_load_image_view_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <bhdfs/bhdfs_vil_load.h>

//: Constructor
bool bhdfs_load_image_view_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); 
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");
  output_types.push_back("unsigned");  // ni of the image
  output_types.push_back("unsigned");  // nj of the image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool bhdfs_load_image_view_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    vcl_cout << "vil_load_image_view_binary_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve filename from input
  vcl_string image_filename = pro.get_input<vcl_string>(i++);
  vil_image_view_base_sptr loaded_image = bhdfs_vil_load(image_filename.c_str());
  if ( !loaded_image ) {
    vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
    return false;
  }
  

  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++, loaded_image);
  pro.set_output_val<unsigned>(i++, loaded_image->ni());
  pro.set_output_val<unsigned>(i++, loaded_image->nj());
  return true;
}

