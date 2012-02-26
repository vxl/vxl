// This is brl/bpro/core/brip_pro/processes/brip_solve_gain_offset_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <brip/brip_gain_offset_solver.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

//: Constructor
bool brip_solve_gain_offset_process_cons(bprb_func_process& pro)
{
  //input image assumed to be in the range 0 - 1
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); // model image
  input_types.push_back("vil_image_view_base_sptr"); // test image
  input_types.push_back("vil_image_view_base_sptr"); // model mask
  input_types.push_back("vil_image_view_base_sptr"); // test mask
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // mapped test image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  brdb_value_sptr null = new brdb_value_t<vil_image_view_base_sptr>(0);  
  // initialize mask inputs to null
  pro.set_input(2, null);
  pro.set_input(3, null);
  return true;
}


//: Execute the process
bool brip_solve_gain_offset_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    vcl_cout << "brip_solve_gain_offset_process: Invalid inputs" << vcl_endl;
    return false;
  }

  // get the inputs
  vil_image_view_base_sptr model_image_ptr = 
    pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view<float> model_image = *model_image_ptr;

  vil_image_view_base_sptr test_image_ptr = 
    pro.get_input<vil_image_view_base_sptr>(1);
  vil_image_view<float> test_image = *test_image_ptr;
  
  vil_image_view_base_sptr model_mask_ptr = 
    pro.get_input<vil_image_view_base_sptr>(2);
  vil_image_view<unsigned char> model_mask;
  if(model_mask_ptr)
    model_mask = *model_mask_ptr;
  
  vil_image_view_base_sptr test_mask_ptr = 
    pro.get_input<vil_image_view_base_sptr>(3);
  vil_image_view<unsigned char> test_mask;
  if(test_mask_ptr)
    test_mask = *test_mask_ptr;

  brip_gain_offset_solver gos(model_image, test_image, model_mask, test_mask);

  if(!gos.solve())
    return false;
  vil_image_view<float> mapped_image = gos.mapped_test_image();
  
  vil_image_view_base_sptr map_img_ptr = 
    new vil_image_view<float>(mapped_image);

  pro.set_output_val<vil_image_view_base_sptr>(0, map_img_ptr);
  return true;
}

