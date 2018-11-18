//This is brl/bbas/bsta/pro/processes/bsta_initialize_random_seed_process.cxx
#include <iostream>
#include <ctime>
#include <bprb/bprb_func_process.h>
//:
// \file
//   Initialize the random number generator, the same instance of vnl_random will be passed to subsequent process to ensure
//   they all use the same rng initialized properly.
//
#include <bprb/bprb_parameters.h>
#include <bsta/bsta_random_wrapper.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brdb/brdb_value.h>

//: sets input and output types
bool bsta_initialize_random_seed_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types_(1);
  input_types_[0] = "unsigned";

  //output
  std::vector<std::string> output_types_(1);
  output_types_[0]= "bsta_random_wrapper_sptr";

  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);

  // in case the 1st input is not set
  brdb_value_sptr idx = new brdb_value_t<unsigned>(0);
  pro.set_input(0, idx);

  return good;
}

bool bsta_initialize_random_seed_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << ": Invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  auto seed = pro.get_input<unsigned>(0);
  if (!seed) {
    pro.set_output_val<bsta_random_wrapper_sptr>(0, new bsta_random_wrapper(std::clock()));
  }
  else {
    pro.set_output_val<bsta_random_wrapper_sptr>(0, new bsta_random_wrapper(seed));
  }
  return true;
}
