// This is brl/bpro/core/sdet_pro/processes/sdet_detect_edges_process.cxx
#include "sdet_detect_edges_process.h"
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_fit_lines.h>
#include <sdet/sdet_img_edge.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vil/vil_image_view.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge_2d.h>

//: initialize input and output types
bool sdet_detect_edges_process_cons(bprb_func_process& pro)
{
  using namespace sdet_detect_edges_process_globals;
  // process takes 1 input:
  //input[0]: input grayscale image
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 1 output:
  // output[0]: output edge image
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

//: generates the edge map
bool sdet_detect_edges_process(bprb_func_process& pro)
{
 using namespace sdet_detect_edges_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get inputs
  // image
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(0);

  //check input validity
  if (!input_image_sptr) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  // get parameters
  double noise_multiplier=1.5, smooth=1.5;
  bool automatic_threshold=false, junctionp=false, aggressive_junction_closure=false;

  pro.parameters()->get_value(param_noise_multiplier_, noise_multiplier);
  pro.parameters()->get_value(param_smooth_, smooth);
  pro.parameters()->get_value(param_automatic_threshold_, automatic_threshold);
  pro.parameters()->get_value(param_junctionp_, junctionp);
  pro.parameters()->get_value(param_aggressive_junction_closure_, aggressive_junction_closure);

  vil_image_view<vxl_byte> input_image(input_image_sptr);
  vil_image_view<vxl_byte> edge_image = sdet_img_edge::detect_edges(input_image,noise_multiplier,smooth,automatic_threshold,junctionp,aggressive_junction_closure);

  // return the output edge image
  pro.set_output_val<vil_image_view_base_sptr>(0,new vil_image_view<vxl_byte>(edge_image));

  return true;
}
