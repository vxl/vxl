//This is brl/bpro/core/sdet_pro/processes/sdet_detect_edge_tangent_process.cxx
#include "sdet_detect_edge_tangent_process.h"
//:
// \file

#include <sdet/sdet_img_edge.h>

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_fit_lines.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
//#include <vpgl/vpgl_rational_camera.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge_2d.h>

//: initialize input and output types
bool sdet_detect_edge_tangent_process_cons(bprb_func_process& pro)
{
  using namespace sdet_detect_edge_tangent_process_globals;
  // process takes 2 inputs
  //input[0]: input grayscale image
  //input[1]: string indicating the output format
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vcl_string";

  // process has 1 output image with 3 bands:
  // output[0]: output edge image with 3 planes
  // case pos_dir
  // plane 0 - sub-pixel column position of the edge.
  //           Negative value indicates no edge is present
  // plane 1 - sub-pixel row position of the edge.
  //           Negative value indicates no edge is present
  // plane 2 - Orientation of local edge tangent direction in radians
  // range is [0, 2pi).
  // case line_2d
  // plane 0 - line coefficient a --
  //                                |-- components of line normal vector
  // plane 1 - line coefficient b --
  //
  // plane 2 - line coefficient c

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

//: generates the edge map
bool sdet_detect_edge_tangent_process(bprb_func_process& pro)
{
 using namespace sdet_detect_edge_tangent_process_globals;

  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " Invalid inputs" << std::endl;
    return false;
  }

  // get inputs
  // image
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(0);

  //check input validity
  if (!input_image_sptr) {
    std::cout << pro.name() <<" :-- null input image\n";
    return false;
  }

  vil_image_view<vxl_byte> input_image =
    *vil_convert_cast(vxl_byte(), input_image_sptr);

  std::string out_type = pro.get_input<std::string>(1);
  // get parameters
  double noise_multiplier=1.5, smooth=1.5;
  bool automatic_threshold=false, junctionp=false, aggressive_junction_closure=false;

  pro.parameters()->get_value(param_noise_multiplier_, noise_multiplier);
  pro.parameters()->get_value(param_smooth_, smooth);
  pro.parameters()->get_value(param_automatic_threshold_, automatic_threshold);
  pro.parameters()->get_value(param_junctionp_, junctionp);
  pro.parameters()->get_value(param_aggressive_junction_closure_, aggressive_junction_closure);
#if 0
  std::cout << "Edge detection parameters\n";
  pro.parameters()->print_all(std::cout);
#endif
  vil_image_view<float> edge_image =
    sdet_img_edge::detect_edge_tangent(input_image,
                                       noise_multiplier,
                                       smooth,
                                       automatic_threshold,
                                       junctionp,
                                       aggressive_junction_closure);

  // return the output edge image in pos_dir format
  if (out_type=="pos_dir") {
    pro.set_output_val<vil_image_view_base_sptr>(0,new vil_image_view<float>(edge_image));
    return true;
  }
  //else convert to line format
  if (out_type == "line_2d") {
    vil_image_view<float> line_image(edge_image.ni(), edge_image.nj(), 3);
    sdet_img_edge::convert_edge_image_to_line_image(edge_image, line_image);
    pro.set_output_val<vil_image_view_base_sptr>(0,new vil_image_view<float>(line_image));
    return true;
  }
  return false;
}
