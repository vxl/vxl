// This is brl/bpro/core/vil_pro/processes/vil_init_float_image_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_init_float_image_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("unsigned");//ni
  input_types.emplace_back("unsigned");//nj
  input_types.emplace_back("unsigned");//np
  input_types.emplace_back("float");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //default values
  pro.set_input(0, brdb_value_sptr(new brdb_value_t<unsigned>(1)));
  pro.set_input(1, brdb_value_sptr(new brdb_value_t<unsigned>(1)));
  pro.set_input(2, brdb_value_sptr(new brdb_value_t<unsigned>(1)));
  pro.set_input(3, brdb_value_sptr(new brdb_value_t<float>(0.0)));

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_init_float_image_process(bprb_func_process& pro)
{
  unsigned int num_inputs = static_cast<int>(pro.input_types().size());
  // Sanity check
  if (pro.n_inputs() != num_inputs) {
    std::cout << "vil_load_image_view_binary_process: The number of inputs should be " << num_inputs << std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  auto ni = pro.get_input<unsigned>(i++);
  auto nj = pro.get_input<unsigned>(i++);
  auto np = pro.get_input<unsigned>(i++);
  auto init_val = pro.get_input<float>(i++);

  auto *img = new vil_image_view<float>(ni,nj,np);
  img->fill(init_val);

  pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(img));

  return true;
}
