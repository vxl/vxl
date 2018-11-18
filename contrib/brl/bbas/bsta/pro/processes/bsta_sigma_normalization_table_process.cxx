//This is brl/bbas/bsta/pro/processes/bsta_sigma_normalization_table_process.cxx
//:
// \file
//   Precompute normalization values for possible sample set sizes,
//   made into a separate process for fast access and also to avoid optimization routines on GPU
//   if later processes are run on GPU
//
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bsta/algo/bsta_sigma_normalizer.h>

#ifdef DEBUG
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

#include <brdb/brdb_value.h>

//: sets input and output types
bool bsta_sigma_normalization_table_process_cons(bprb_func_process& pro)
{
  //inputs
  //0: under_estimation_probability
  //1: N_PRECOMPUTED if available; default value: 40

  std::vector<std::string> input_types_(2);
  input_types_[0] = "float";
  input_types_[1] = "unsigned";

  //output
  std::vector<std::string> output_types_(1);
  output_types_[0]= "bsta_sigma_normalizer_sptr";

  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);

  // in case the 1st input is not set
  brdb_value_sptr idx = new brdb_value_t<unsigned>(40);
  pro.set_input(1, idx);

  return good;
}

bool bsta_sigma_normalization_table_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << ": Invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  auto under_estimation_probability = pro.get_input<float>(0);
  unsigned n_precomputed = pro.get_input<int>(1);

  pro.set_output_val<bsta_sigma_normalizer_sptr>(0, new bsta_sigma_normalizer(under_estimation_probability, n_precomputed));
  return true;
}
