// This is brl/bpro/core/vil_pro/processes/vil_crop_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_new.h>

//: Constructor
bool vil_crop_image_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); 
  input_types.push_back("unsigned"); 
  input_types.push_back("unsigned"); 
  input_types.push_back("unsigned"); 
  input_types.push_back("unsigned"); 
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); 
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_crop_image_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 5) {
    vcl_cout << "vil_crop_image_process: The input number should be 5" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_resource_sptr image_ptr = vil_new_image_resource_of_view(*image);

  //Retrieve limits
  unsigned i0= pro.get_input<unsigned>(i++);
  unsigned j0= pro.get_input<unsigned>(i++);
  unsigned ni= pro.get_input<unsigned>(i++);
  unsigned nj= pro.get_input<unsigned>(i++);
  
  vil_image_resource_sptr out_img = vil_crop(image_ptr, i0, ni, j0, nj);
  vil_image_view_base_sptr out_sptr = vil_new_image_view_base_sptr(*(out_img->get_view()));
  
  pro.set_output_val<vil_image_view_base_sptr>(0, out_sptr);
  return true;
}

