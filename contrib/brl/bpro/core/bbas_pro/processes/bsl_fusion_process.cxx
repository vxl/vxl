// This is brl/bpro/core/bbas_pro/processes/bsl_fusion_process.cxx
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
bool bsl_fusion_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(3);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";
  input_types_[2] = "float"; // atomicity - same for all pixels

  //output
  std::vector<std::string> output_types_(1);
  output_types_[0] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

bool bsl_fusion_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << ": Invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr inp1 = pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view_base_sptr inp2 = pro.get_input<vil_image_view_base_sptr>(1);
  auto atomicity = pro.get_input<float>(2);

  vil_image_view<float> inp_img1(inp1);
  vil_image_view<float> inp_img2(inp2);
  unsigned ni = inp_img1.ni();
  unsigned nj = inp_img1.nj();
  unsigned np = inp_img1.nplanes();
  if (ni != inp_img2.ni() || nj != inp_img2.nj() || np != inp_img2.nplanes()) {
    std::cout << "In bsl_fusion_process: mismatch in input image dimensions!\n";
    return false;
  }

  auto* out_img = new vil_image_view<float>(ni, nj, np);
  if (np == 2) {  // only b and u planes, plane 0: belief, plane 1: uncertainty
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        bsl_opinion op1(inp_img1(i,j,1), inp_img1(i,j,0), atomicity);
        bsl_opinion op2(inp_img2(i,j,1), inp_img2(i,j,0), atomicity);
        bsl_opinion op_f = fuse(op1,op2) ;
        (*out_img)(i,j,0) = op_f.b();
        (*out_img)(i,j,1) = op_f.u();
      }
  }
  else if (np == 3) {  // b, d, and u planes, plane 0: disbelief, plane 1: belief, plane 2: uncertainty
     for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        bsl_opinion op1(inp_img1(i,j,2), inp_img1(i,j,1), atomicity);
        bsl_opinion op2(inp_img2(i,j,2), inp_img2(i,j,1), atomicity);
        bsl_opinion op_f = fuse(op1,op2);
        (*out_img)(i,j,0) = op_f.d();
        (*out_img)(i,j,1) = op_f.b();
        (*out_img)(i,j,2) = op_f.u();
      }
  }
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img);
  return true;
}
