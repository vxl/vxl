// This is brl/bpro/core/sdet_pro/processes/sdet_detect_third_order_edges_process.cxx
#include "sdet_detect_third_order_edges_process.h"
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>

#include <sdet/sdet_third_order_edge_det_params.h>
#include <sdet/sdet_third_order_edge_det.h>

#include <sdet/sdet_edge_champher.h>
#include <vtol/vtol_edge_2d.h>
#include <vsol/vsol_line_2d.h>

//: initialize input and output types
bool sdet_detect_third_order_edges_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vcl_string");  // path to write output edge map, extention: .edg
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output: [0] output edge image
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");
  output_types.emplace_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

//: generates the edge map
bool sdet_detect_third_order_edges_process(bprb_func_process& pro)
{
 // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << "dbrec_edge_det_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> input_image(input_image_sptr);
  std::string out_edg_map_path = pro.get_input<std::string>(i++);

  //get the parameters
  sdet_third_order_edge_det_params dp;
  dp.grad_op_ = 0; dp.conv_algo_ = 0; dp.pfit_type_ = 0;
  dp.sigma_ = 1.0; dp.thresh_ = 2.0;
  dp.interp_factor_ = 1;
  dp.adapt_thresh_ = false;

  pro.parameters()->get_value( "grad_op", dp.grad_op_);
  pro.parameters()->get_value( "conv_algo", dp.conv_algo_);
  pro.parameters()->get_value( "int_factor" , dp.interp_factor_);
  pro.parameters()->get_value( "sigma", dp.sigma_);
  pro.parameters()->get_value( "thresh", dp.thresh_);
  pro.parameters()->get_value( "badap_thresh" , dp.adapt_thresh_);
  pro.parameters()->get_value( "parabola_fit", dp.pfit_type_);

  sdet_third_order_edge_det det(dp);
  if (input_image.nplanes() == 3) {
    std::cout << "Input image has 3 planes, applying color edge detector!\n";
    det.apply_color(input_image);
  }
  else {
    det.apply(input_image);
  }

  std::vector<vdgl_edgel> edgels = det.edgels();

  std::cout << "done, sigma: " << dp.sigma_ << " thres: " << dp.thresh_ << "!\n"
           << "#edgels = " << edgels.size() << std::endl;

  sdet_third_order_edge_det::save_edg_ascii(out_edg_map_path, input_image.ni(), input_image.nj(), edgels);

  // initialize the output edge image
  vil_image_view<float> edge_img(input_image.ni(),input_image.nj(),3);
  edge_img.fill(-1.0f);
  vil_image_view<vxl_byte> edge_img_byte(input_image.ni(),input_image.nj());
  edge_img_byte.fill(0);

  for (auto edgel : edgels) {
    double x = edgel.x();
    double y = edgel.y();

    auto ix = (unsigned)x;
    auto iy = (unsigned)y;
    double idir = vnl_math::angle_0_to_2pi(edgel.get_theta());

    edge_img(ix, iy, 0) = static_cast<float>(x);
    edge_img(ix, iy, 1) = static_cast<float>(y);
    edge_img(ix, iy, 2) = static_cast<float>(idir);
    edge_img_byte(ix, iy) = 255;
  }

  // Following loop removes the edges in the image boundary
  int temp_index = edge_img_byte.nj()-1;
  for (unsigned i=0; i<edge_img_byte.ni(); i++) {
    edge_img_byte(i,0) = 0;
    edge_img_byte(i,temp_index) = 0;
  }
  temp_index = edge_img_byte.ni()-1;
  for (unsigned j=0; j<edge_img_byte.nj(); j++) {
    edge_img_byte(0,j) = 0;
    edge_img_byte(temp_index,j) = 0;
  }

  pro.set_output_val<vil_image_view_base_sptr>(0,new vil_image_view<float>(edge_img));
  pro.set_output_val<vil_image_view_base_sptr>(1,new vil_image_view<vxl_byte>(edge_img_byte));

  return true;
}


//: initialize input and output types
bool sdet_detect_third_order_edges_dt_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("int");  // max distance threshold
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output: [0] Distance transform (DT) of output edge image as a byte image
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");
  output_types.emplace_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

//: generates the edge map
bool sdet_detect_third_order_edges_dt_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 1) {
    std::cerr << "sdet_detect_third_order_edges_dt_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> input_image(input_image_sptr);
  int max_dist_threshold = pro.get_input<int>(i++);

  //get the parameters
  sdet_third_order_edge_det_params dp;
  dp.grad_op_ = 0; dp.conv_algo_ = 0; dp.pfit_type_ = 0;
  dp.sigma_ = 1.0; dp.thresh_ = 2.0;
  dp.interp_factor_ = 1;
  dp.adapt_thresh_ = false;

  pro.parameters()->get_value( "grad_op", dp.grad_op_);
  pro.parameters()->get_value( "conv_algo", dp.conv_algo_);
  pro.parameters()->get_value( "int_factor" , dp.interp_factor_);
  pro.parameters()->get_value( "sigma", dp.sigma_);
  pro.parameters()->get_value( "thresh", dp.thresh_);
  pro.parameters()->get_value( "badap_thresh" , dp.adapt_thresh_);
  pro.parameters()->get_value( "parabola_fit", dp.pfit_type_);

  sdet_third_order_edge_det det(dp);
  if (input_image.nplanes() == 3) {
    std::cout << "Input image has 3 planes, applying color edge detector!\n";
    det.apply_color(input_image);
  }
  else {
    det.apply(input_image);
  }
  std::vector<vdgl_edgel> edgels = det.edgels();

  std::vector<vsol_line_2d_sptr> line_segs;
  det.line_segs(line_segs);

  unsigned ni = input_image.ni();
  unsigned nj = input_image.nj();
  vbl_array_2d<vtol_edge_2d_sptr> vtol_edges(ni, nj);
  for (unsigned k = 0; k < edgels.size(); k++) {
    vdgl_edgel edgel = edgels[k];

    double x = edgel.x();
    double y = edgel.y();

    auto ix = (unsigned)x;
    auto iy = (unsigned)y;

    vtol_edge_2d_sptr vtol_edg = new vtol_edge_2d(*(line_segs[k]->cast_to_curve()));
    vtol_edges[ix][iy] = vtol_edg;
  }

  sdet_edge_champher ec(vtol_edges);
  //: create distance image
  vil_image_view<float> out_imgf(ni, nj);
  out_imgf.fill((float)max_dist_threshold);
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      float distf = ec.distance(j,i);
      if (out_imgf(i,j) > distf)
        out_imgf(i,j) = distf;
    }
  }
  vil_image_view<float> diff_img(ni, nj);
  diff_img.fill((float)max_dist_threshold);
  vil_image_view<float> sum_img(ni, nj);
  vil_math_image_difference(diff_img, out_imgf, sum_img);

  //: scale to 0-255 range
  //vil_math_scale_values(out_imgf, 255.0f/(float)max_dist_threshold);
  vil_math_scale_values(sum_img, 255.0f/(float)max_dist_threshold);
  vil_image_view<vxl_byte> out_img(ni, nj);
  //vil_convert_cast(out_imgf, out_img);
  vil_convert_cast(sum_img, out_img);

  pro.set_output_val<vil_image_view_base_sptr>(0,new vil_image_view<vxl_byte>(out_img));
  pro.set_output_val<vil_image_view_base_sptr>(1,new vil_image_view<float>(out_imgf));
  return true;
}
