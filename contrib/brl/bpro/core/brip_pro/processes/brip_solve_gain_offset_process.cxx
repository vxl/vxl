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
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); // model image
  input_types.emplace_back("vil_image_view_base_sptr"); // test image
  input_types.emplace_back("vil_image_view_base_sptr"); // model mask
  input_types.emplace_back("vil_image_view_base_sptr"); // test mask
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // mapped test image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  brdb_value_sptr null = new brdb_value_t<vil_image_view_base_sptr>(nullptr);
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
    std::cout << "brip_solve_gain_offset_process: Invalid inputs" << std::endl;
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


//: Constructor
bool brip_solve_gain_offset_constraints_process_cons(bprb_func_process& pro)
{
    //input image assumed to be in the range 0 - 1
    bool ok = false;
    std::vector<std::string> input_types;
    input_types.emplace_back("vil_image_view_base_sptr"); // model image
    input_types.emplace_back("vil_image_view_base_sptr"); // test image
    input_types.emplace_back("double"); // default lambda = 0 for no constraints and 1 for all constraints
    input_types.emplace_back("vil_image_view_base_sptr"); // model mask
    input_types.emplace_back("vil_image_view_base_sptr"); // test mask
    ok = pro.set_input_types(input_types);
    if (!ok) return ok;

    std::vector<std::string> output_types;
    output_types.emplace_back("vil_image_view_base_sptr");  // mapped test image
    ok = pro.set_output_types(output_types);
    if (!ok) return ok;

    brdb_value_sptr null = new brdb_value_t<vil_image_view_base_sptr>(nullptr);
    // initialize mask inputs to null
    pro.set_input(3, null);
    pro.set_input(4, null);
    brdb_value_sptr default_lambda = new brdb_value_t<double>(1.0);
    pro.set_input(2, default_lambda);
    return true;
}


//: Execute the process
bool brip_solve_gain_offset_constraints_process(bprb_func_process& pro)
{
    // Sanity check
    if (!pro.verify_inputs()) {
        std::cout << "brip_solve_gain_offset_process: Invalid inputs" << std::endl;
        return false;
    }

    // get the inputs
    vil_image_view_base_sptr model_image_ptr =
        pro.get_input<vil_image_view_base_sptr>(0);
    vil_image_view<float> model_image = *model_image_ptr;

    vil_image_view_base_sptr test_image_ptr =
        pro.get_input<vil_image_view_base_sptr>(1);
    vil_image_view<float> test_image = *test_image_ptr;
    auto lambda = pro.get_input<double>(2);
    vil_image_view_base_sptr model_mask_ptr =
        pro.get_input<vil_image_view_base_sptr>(3);
    vil_image_view<unsigned char> model_mask;
    if (model_mask_ptr)
        model_mask = *model_mask_ptr;

    vil_image_view_base_sptr test_mask_ptr =
        pro.get_input<vil_image_view_base_sptr>(4);
    vil_image_view<unsigned char> test_mask;
    if (test_mask_ptr)
        test_mask = *test_mask_ptr;

    brip_gain_offset_solver gos(model_image, test_image, model_mask, test_mask);

    if (!gos.solve_with_constraints(lambda))
        return false;
    vil_image_view<float> mapped_image = gos.mapped_test_image();

    vil_image_view_base_sptr map_img_ptr =
        new vil_image_view<float>(mapped_image);

    pro.set_output_val<vil_image_view_base_sptr>(0, map_img_ptr);
    return true;
}
