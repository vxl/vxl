// This is brl/bbas/bhdfs/pro/processes/bhdfs_save_image_view_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <bhdfs/bhdfs_vil_save.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool bhdfs_save_image_view_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  // input(0): the vil_image_view_base_sptr
  // input(1): the filename to save to
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vcl_string");
  return pro.set_input_types(input_types);

  //this process has no outputs
}


//: Execute the process
bool bhdfs_save_image_view_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    vcl_cout << "vil_save_image_view_process: The number of inputs should be 2" << vcl_endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);

  //Retrieve filename from input
  vcl_string image_filename = pro.get_input<vcl_string>(i++);

  bool result = bhdfs_vil_save(*img, image_filename.c_str());

  if ( !result ) {
    vcl_cerr << "Failed to save image to" << image_filename << '\n';
    return false;
  }
  else
    return true;
}

