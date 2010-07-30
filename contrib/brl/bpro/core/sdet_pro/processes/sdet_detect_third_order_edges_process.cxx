// This is brl/bpro/core/sdet_pro/processes/sdet_detect_third_order_edges_process.cxx
#include "sdet_detect_edges_process.h"
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>

#include <sdet/sdet_third_order_edge_det_params.h>
#include <sdet/sdet_third_order_edge_det.h>

//: initialize input and output types
bool sdet_detect_third_order_edges_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vcl_string");  // path to write output edge map, extention: .edg
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output: [0] output edge image
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");
  output_types.push_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

double angle_0_360(double angle)
{
  double ang = angle;
  while (ang<0)
    ang += (2.0*vnl_math::pi);
  while (ang > 2.0*vnl_math::pi)
    ang -= (2.0*vnl_math::pi);
  return ang;
}

//: generates the edge map
bool sdet_detect_third_order_edges_process(bprb_func_process& pro)
{
 // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dbrec_edge_det_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> input_image(input_image_sptr);
  vcl_string out_edg_map_path = pro.get_input<vcl_string>(i++);

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
  det.apply(input_image);

  vcl_vector<vdgl_edgel> edgels = det.edgels();

  vcl_cout << "done, sigma: " << dp.sigma_ << " thres: " << dp.thresh_ << "!\n"
           << "#edgels = " << edgels.size() << vcl_endl;

  sdet_third_order_edge_det::save_edg_ascii(out_edg_map_path, input_image.ni(), input_image.nj(), edgels);

  // initialize the output edge image
  vil_image_view<float> edge_img(input_image.ni(),input_image.nj(),3);
  edge_img.fill(-1.0f);
  vil_image_view<vxl_byte> edge_img_byte(input_image.ni(),input_image.nj());
  edge_img_byte.fill(0);

  for (unsigned k = 0; k < edgels.size(); k++) {
    vdgl_edgel edgel = edgels[k];

    double x = edgel.x();
    double y = edgel.y();

    unsigned ix = (unsigned)x;
    unsigned iy = (unsigned)y;
    double idir = angle_0_360(edgel.get_theta());

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
