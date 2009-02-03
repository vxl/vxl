// This is brl/bpro/core/vil_pro/processes/vil_rgbi_to_grey_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_rgbi_to_grey_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  // input(0): the vil_image_view_base_sptr
  // input(1): the filename to save to
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has no outputs
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_rgbi_to_grey_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    vcl_cout << "vil_rgbi_to_grey_process: The input number should be 1" << vcl_endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<unsigned char> *out_img=new vil_image_view<unsigned char>(img->ni(),img->nj());

  vil_convert_planes_to_grey<unsigned char,unsigned char>(*(img.as_pointer()),*out_img);
  vil_image_view_base_sptr out_img_ptr=out_img;

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  return true;
}

