//This is brl/bpro/core/bbas_pro/processes/bsl_expected_image_process.cxx
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
//   input an opinion image = each pixel is a binomial opinion, b, d, u and generate prob image p = b + a.u

#include <bprb/bprb_parameters.h>

#ifdef DEBUG
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

#include <brdb/brdb_value.h>
#include <bsl/bsl_opinion.h>
#include <vil/vil_image_view.h>

//: sets input and output types
bool bsl_expected_image_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(2);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "float"; // atomicity - same for all pixels

  //output
  std::vector<std::string> output_types_(1);
  output_types_[0] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

bool bsl_expected_image_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << ": Invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr inp = pro.get_input<vil_image_view_base_sptr>(0);
  auto atomicity = pro.get_input<float>(1);

  vil_image_view<float> inp_img(inp);
  unsigned ni = inp_img.ni();
  unsigned nj = inp_img.nj();
  auto* out_img = new vil_image_view<float>(ni, nj);
  if (inp_img.nplanes() == 2) {  // only b and u planes, plane 0: belief, plane 1: uncertainty
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        bsl_opinion op(inp_img(i,j,1), inp_img(i,j,0), atomicity);
        (*out_img)(i,j) = op.expectation();
      }
  }
  else if (inp_img.nplanes() == 3) {  // b, d, and u planes, plane 0: disbelief, plane 1: belief, plane 2: uncertainty
     for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        bsl_opinion op(inp_img(i,j,2), inp_img(i,j,1), atomicity);
        (*out_img)(i,j) = op.expectation();
      }
  }
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img);
  return true;
}
