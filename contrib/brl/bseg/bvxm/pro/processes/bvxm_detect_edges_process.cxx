//This is brl/bseg/bvxm/pro/processes/bvxm_detect_edges_process.cxx

//:
// \file
// \brief A process that takes a grayscale image and returns the corresponding edge map
//
// \author Ibrahim Eden
// \date March 05, 2008
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_fit_lines.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge_2d.h>

//: global variables
namespace bvxm_detect_edges_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_=1;

  //parameter strings
  const vcl_string param_noise_multiplier_ =  "noise_multiplier";
  const vcl_string param_smooth_ =  "smooth";
  const vcl_string param_automatic_threshold_ =  "automatic_threshold";
  const vcl_string param_junctionp_ =  "junctionp";
  const vcl_string param_aggressive_junction_closure_ =  "aggressive_junction_closure";
}

//: initialize input and output types
bool bvxm_detect_edges_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_detect_edges_process_globals;
  // process takes 1 input:
  //input[0]: input grayscale image
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 1 output:
  // output[0]: output edge image
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

//: generates the edge map
bool bvxm_detect_edges_process(bprb_func_process& pro)
{
 using namespace bvxm_detect_edges_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get inputs
  // image
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(0);

  //check input validity
  if (!input_image_sptr) {
    vcl_cout << pro.name() <<" :--  Input 0  is not valid!\n";
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
  vil_image_view<vxl_byte> edge_image = bvxm_util::detect_edges(input_image,noise_multiplier,smooth,automatic_threshold,junctionp,aggressive_junction_closure);

  // return the output edge image
  pro.set_output_val<vil_image_view_base_sptr>(0,new vil_image_view<vxl_byte>(edge_image));

  return true;
}
