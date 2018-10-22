// This is brl/bseg/boxm2/pro/processes/boxm2_combine_expected_images_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for combining two images
//
// \author Gamze D. Tunali
// \date   June 28, 2012

#include <vil/vil_image_view.h>

namespace boxm2_combine_expected_images_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 2;
}

bool boxm2_combine_expected_images_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_combine_expected_images_process_globals;

  //process takes 4 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";
  input_types_[2] = "vil_image_view_base_sptr";
  input_types_[3] = "vil_image_view_base_sptr";

  // process has 2 output:
  // output[0]: expected image
  // output[1]: expected image
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_combine_expected_images_process(bprb_func_process& pro)
{
  using namespace boxm2_combine_expected_images_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr e1_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr v1_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr e2_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr v2_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  vil_image_view<unsigned char> e1(e1_sptr);
  vil_image_view<unsigned char> v1(v1_sptr);
  vil_image_view<unsigned char> e2(e2_sptr);
  vil_image_view<unsigned char> v2(v2_sptr);

  int ni=e1_sptr->ni(), nj=e1_sptr->nj();

  unsigned char* e1buff = e1.top_left_ptr();
  unsigned char* v1buff = v1.top_left_ptr();
  unsigned char* e2buff = e2.top_left_ptr();
  unsigned char* v2buff = v2.top_left_ptr();
  int minU = 0, minV = 0, maxU = ni, maxV = nj;
  for (int jj=minV; jj<maxV; ++jj)
    for (int ii=minU; ii<maxU; ++ii) {
      int imIdx = jj*ni + ii;
      e1buff[imIdx] +=(unsigned char)( (float)e2buff[imIdx] * (float)v1buff[imIdx]/255.0f - 0.5 * (float)v1buff[imIdx]);
      v1buff[imIdx] = (unsigned char)( (float)v1buff[imIdx] * (float)v2buff[imIdx]/255.0f);
    }

  i=0;

  // output image smart pointers
  pro.set_output_val<vil_image_view_base_sptr>(i++, new vil_image_view<unsigned char>(e1));
  pro.set_output_val<vil_image_view_base_sptr>(i++, new vil_image_view<unsigned char>(v1));

  return true;
}
