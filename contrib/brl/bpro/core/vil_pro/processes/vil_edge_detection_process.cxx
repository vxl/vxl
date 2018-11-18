// This is brl/bpro/core/vil_pro/processes/vil_edge_detection_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process that performs edge detection on a grey image and returns the corresponding edge map

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <sdet/sdet_detector.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vtol/vtol_edge_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>

//: global variables and functions
namespace vil_edge_detection_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 1;
}

//: constructor
bool vil_edge_detection_process_cons(bprb_func_process& pro)
{
  using namespace vil_edge_detection_process_globals;
  // process takes 6 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input grey image
  input_types_[1] = "float";                     // noise multiplier (edge detection parameter)
  input_types_[2] = "float";                     // smooth parameter (edge detection parameter)
  input_types_[3] = "bool";                      // automatic_threshold (edge detection parameter)
  input_types_[4] = "bool";                      // junctionp (edge detection parameter)
  input_types_[5] = "bool";                      // aggressive_junction_closure (edge detection parameter)

  // process takes 1 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // output edge map
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: execute the process
bool vil_edge_detection_process(bprb_func_process& pro)
{
  using namespace vil_edge_detection_process_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr in_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  auto noise_multiplier = pro.get_input<float>(in_i++);
  auto smooth = pro.get_input<float>(in_i++);
  bool automatic_threshold = pro.get_input<bool>(in_i++);
  bool junctionp = pro.get_input<bool>(in_i++);
  bool aggressive_junction_closure = pro.get_input<bool>(in_i++);

  // input image validity
  auto* in_img = dynamic_cast<vil_image_view<vxl_byte>*>(in_img_sptr.ptr());
  if (!in_img) {
    std::cout << pro.name() << ": Unsupported input image format " << in_img_sptr->pixel_format() << std::endl;
    return false;
  }

  if (in_img->nplanes() >= 3)
  {
    vil_image_view<vxl_byte> img_rgb;
    img_rgb.deep_copy(*in_img);
    vil_convert_planes_to_grey(img_rgb, *in_img);
  }

  // set parameters for the edge detector
  sdet_detector_params dp;
  dp.noise_multiplier = noise_multiplier;
  dp.smooth = smooth;
  dp.automatic_threshold = automatic_threshold;
  dp.junctionp = junctionp;
  dp.aggressive_junction_closure = aggressive_junction_closure;

  // detect edges from the input image
  sdet_detector detector(dp);
  vil_image_resource_sptr in_img_res_sptr = vil_new_image_resource_of_view(*in_img);
  detector.SetImage(in_img_res_sptr);
  detector.DoContour();
  std::vector<vtol_edge_2d_sptr>* edges = detector.GetEdges();

  // generate output edge image
  vil_image_view<vxl_byte> edge_image(in_img->ni(), in_img->nj());
  edge_image.fill(0);

  for (auto & edge : *edges)
  {
    vdgl_digital_curve_sptr dc = (edge->curve())->cast_to_vdgl_digital_curve();
    if (!dc)
      continue;
    vdgl_interpolator_sptr intp = dc->get_interpolator();
    vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();
    // iterate over each point in the connected edge component
    for (unsigned j=0; j<ec->size(); j++) {
      vdgl_edgel curr_edgel = ec->edgel(j);
      int cr_x = (int)curr_edgel.x();
      int cr_y = (int)curr_edgel.y();
      // set the current edge pixel in the edge image
      edge_image(cr_x,cr_y) = 255;
    }
  }
  // Following loop removes the edges in the image boundary
  int temp_index = edge_image.nj()-1;
  for (unsigned i = 0; i <edge_image.ni(); i++) {
    edge_image(i,0) = 0;
    edge_image(i, temp_index) = 0;
  }
  temp_index = edge_image.ni()-1;
  for (unsigned j=0; j<edge_image.nj(); j++) {
    edge_image(0,j) = 0;
    edge_image(temp_index,j) = 0;
  }
  // output
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_byte>(edge_image));

  return true;
}
