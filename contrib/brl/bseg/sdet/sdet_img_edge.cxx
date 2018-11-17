// This is brl/bseg/sdet/sdet_img_edge.cxx

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include "sdet_img_edge.h"

#include <sdet/sdet_detector.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <brip/brip_vil_float_ops.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_cubic.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vtol/vtol_edge_2d.h>
#include <bil/algo/bil_edt.h>
#include <vil/vil_convert.h>
#include <vnl/algo/vnl_gaussian_kernel_1d.h>
#include <vnl/algo/vnl_chi_squared.h>
#include <vnl/vnl_vector_fixed.h>
#include <vil/vil_new.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgl/vgl_line_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <sdet/sdet_fit_lines_params.h>
#include <sdet/sdet_fit_lines.h>

vil_image_view<vxl_byte> sdet_img_edge::detect_edges(vil_image_view<vxl_byte> img,
                                                     double noise_multiplier,
                                                     double smooth,
                                                     bool automatic_threshold,
                                                     bool junctionp,
                                                     bool aggressive_junction_closure)
{
  if ( img.nplanes() >= 3 )
  {
    vil_image_view<vxl_byte> img_rgb;
    img_rgb.deep_copy(img);
    vil_convert_planes_to_grey(img_rgb,img);
  }

  // set parameters for the edge detector
  sdet_detector_params dp;
  dp.noise_multiplier = (float)noise_multiplier;
  dp.smooth = (float)smooth;
  dp.automatic_threshold = automatic_threshold;
  dp.junctionp = junctionp;
  dp.aggressive_junction_closure = aggressive_junction_closure;

  // detect edgels from the input image
  sdet_detector detector(dp);
  vil_image_resource_sptr img_res_sptr = vil_new_image_resource_of_view(img);
  detector.SetImage(img_res_sptr);
  detector.DoContour();
  std::vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();

  // initialize the output edge image
  vil_image_view<vxl_byte> img_edge(img.ni(),img.nj(),1);
  img_edge.fill(0);

  // iterate over each connected edge component
  for (auto & edge : *edges)
  {
    vsol_curve_2d_sptr c = edge->curve();
    vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
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
      img_edge(cr_x,cr_y) = 255;
    }
  }

  // Following loop removes the edges in the image boundary
  int temp_index = img_edge.nj()-1;
  for (unsigned i=0; i<img_edge.ni(); i++) {
    img_edge(i,0) = 0;
    img_edge(i,temp_index) = 0;
  }
  temp_index = img_edge.ni()-1;
  for (unsigned j=0; j<img_edge.nj(); j++) {
    img_edge(0,j) = 0;
    img_edge(temp_index,j) = 0;
  }
  return img_edge;
}

vil_image_view<float>
sdet_img_edge::detect_edge_tangent(const vil_image_view<vxl_byte>& img,
                                   double noise_multiplier,
                                   double smooth,
                                   bool automatic_threshold,
                                   bool junctionp,
                                   bool aggressive_junction_closure)
{
  // set parameters for the edge detector
  sdet_detector_params dp;
  dp.noise_multiplier = (float)noise_multiplier;
  dp.smooth = (float)smooth;
  dp.automatic_threshold = automatic_threshold;
  dp.junctionp = junctionp;
  dp.aggressive_junction_closure = aggressive_junction_closure;

  // detect edgels from the input image
  sdet_detector detector(dp);
  vil_image_resource_sptr img_res_sptr = vil_new_image_resource_of_view(img);
  detector.SetImage(img_res_sptr);
  detector.DoContour();
  std::vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();

  // initialize the output edge image
  vil_image_view<float> edge_img(img.ni(),img.nj(),3);
  edge_img.fill(-1.0f);

  // iterate over each connected edge component
  for (auto & edge : *edges)
  {
    vsol_curve_2d_sptr c = edge->curve();
    vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
    if (!dc)
      continue;
    vdgl_interpolator_sptr intp = dc->get_interpolator();
    vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();
    unsigned n = ec->size();
    if (n<3) continue; //can't estimate edge tangent on short chains
    //special case at start
    vdgl_edgel e0 = ec->edgel(0);
    vdgl_edgel e1 = ec->edgel(1);
    double e0x  = e0.x(), e0y = e0.y();
    double e1x  = e1.x(), e1y = e0.y();
    if (e0x<0||e0y<0) continue;
    double ang = vnl_math::angle_0_to_2pi(std::atan2(e1y-e0y, e1x-e0x));
    auto x0 = static_cast<unsigned>(e0x);
    auto y0 = static_cast<unsigned>(e0y);
    edge_img(x0, y0, 0) = static_cast<float>(e0x);
    edge_img(x0, y0, 1) = static_cast<float>(e0y);
    edge_img(x0, y0, 2) = static_cast<float>(ang);
    //special case at end of chain
    vdgl_edgel enm2 = ec->edgel(n-2);
    vdgl_edgel enm1 = ec->edgel(n-1);
    double enm2x  = enm2.x(), enm2y = enm2.y();
    double enm1x  = enm1.x(), enm1y = enm1.y();
    if (enm1x<0||enm1y<0) continue;
    double angnm1 = vnl_math::angle_0_to_2pi(std::atan2(enm1y-enm2y, enm1x-enm2x));
    auto xnm1 = static_cast<unsigned>(enm1x);
    auto ynm1 = static_cast<unsigned>(enm1y);
    edge_img(xnm1, ynm1, 0) = static_cast<float>(enm1x);
    edge_img(xnm1, ynm1, 1) = static_cast<float>(enm1y);
    edge_img(xnm1, ynm1, 2) = static_cast<float>(angnm1);
    // the general case
    for (unsigned j=1; j<n-1; j++) {
      vdgl_edgel pe = ec->edgel(j-1);
      vdgl_edgel ce = ec->edgel(j);
      vdgl_edgel ne = ec->edgel(j+1);
      double pex  = pe.x(), pey = pe.y();
      double cex  = ce.x(), cey = ce.y();
      double nex  = ne.x(), ney = ne.y();
      if (cex<0||cey<0) continue;
      double angle = vnl_math::angle_0_to_2pi(std::atan2(ney-pey, nex-pex));
      auto xc = static_cast<unsigned>(cex);
      auto yc = static_cast<unsigned>(cey);
      // set the current edge pixel in the edge image
      edge_img(xc, yc, 0) = static_cast<float>(cex);
      edge_img(xc, yc, 1) = static_cast<float>(cey);
      edge_img(xc, yc, 2) = static_cast<float>(angle);
    }
  }

  // Following loop removes the edges in the image boundary
  int temp_index = edge_img.nj()-1;
  for (unsigned i=0; i<edge_img.ni(); i++) {
    edge_img(i,0,0) = -1;     edge_img(i,0,1) = -1;
    edge_img(i,temp_index,0) = -1;
    edge_img(i,temp_index,1) = -1;
  }
  temp_index = edge_img.ni()-1;
  for (unsigned j=0; j<edge_img.nj(); j++) {
    edge_img(0,j,0) = -1;     edge_img(0,j,1) = -1;
    edge_img(temp_index,j,0) = -1;
    edge_img(temp_index,j,1) = -1;
  }
  return edge_img;
}

// return image has three planes as in detect_edge_tangent
// Canny edge detector returns edgel chains with a linear interpolator by default, replace this interpolator with a cubic one and read the edge tangents from this interpolator
vil_image_view<float>
sdet_img_edge::detect_edge_tangent_interpolated(const vil_image_view<vxl_byte>& img,
                                                double noise_multiplier,
                                                double smooth,
                                                bool automatic_threshold,
                                                bool junctionp,
                                                bool aggressive_junction_closure)
{
  // set parameters for the edge detector
  sdet_detector_params dp;
  dp.noise_multiplier = (float)noise_multiplier;
  dp.smooth = (float)smooth;
  dp.automatic_threshold = automatic_threshold;
  dp.junctionp = junctionp;
  dp.aggressive_junction_closure = aggressive_junction_closure;

  // detect edgels from the input image
  sdet_detector detector(dp);
  vil_image_resource_sptr img_res_sptr = vil_new_image_resource_of_view(img);
  detector.SetImage(img_res_sptr);
  detector.DoContour();
  std::vector<vdgl_digital_curve_sptr> edges;
  detector.get_vdgl_edges(edges);

  // initialize the output edge image
  vil_image_view<float> edge_img(img.ni(),img.nj(),3);
  edge_img.fill(-1.0f);

  // iterate over each connected edge component
  //for (std::vector<vtol_edge_2d_sptr>::iterator eit = edges->begin(); eit != edges->end(); eit++)
  for (const auto& dc : edges)
  {
    if (!dc)
      continue;
    vdgl_interpolator_sptr intp = dc->get_interpolator();
    vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();
    unsigned n = ec->size();

    vdgl_interpolator_sptr cubic_intp = new vdgl_interpolator_cubic(ec);
    //vdgl_interpolator_sptr cubic_intp = new vdgl_interpolator_linear(ec);
    std::cout << " length: " << cubic_intp->get_length()  << std::endl;

    for (unsigned j=1; j<n-1; j++) {
      double cex = cubic_intp->get_x(j);
      double cey = cubic_intp->get_y(j);

      auto xc = static_cast<unsigned>(cex);
      auto yc = static_cast<unsigned>(cey);
      double angle = vnl_math::angle_0_to_2pi((vnl_math::pi_over_180)*cubic_intp->get_theta(j)+vnl_math::pi_over_2);
      //double angle = vnl_math::angle_0_to_2pi((vnl_math::pi_over_180)*cubic_intp->get_tangent_angle(j));

      edge_img(xc, yc, 0) = static_cast<float>(cex);
      edge_img(xc, yc, 1) = static_cast<float>(cey);
      edge_img(xc, yc, 2) = static_cast<float>(angle);
    }
  }

  // Following loop removes the edges in the image boundary
  int temp_index = edge_img.nj()-1;
  for (unsigned i=0; i<edge_img.ni(); i++) {
    edge_img(i,0,0) = -1;     edge_img(i,0,1) = -1;
    edge_img(i,temp_index,0) = -1;
    edge_img(i,temp_index,1) = -1;
  }
  temp_index = edge_img.ni()-1;
  for (unsigned j=0; j<edge_img.nj(); j++) {
    edge_img(0,j,0) = -1;     edge_img(0,j,1) = -1;
    edge_img(temp_index,j,0) = -1;
    edge_img(temp_index,j,1) = -1;
  }
  return edge_img;
}

// return image has three planes as in detect_edge_tangent
vil_image_view<float>
sdet_img_edge::detect_edge_line_fitted(const vil_image_view<vxl_byte>& img,
                                       double noise_multiplier,
                                       double smooth,
                                       bool automatic_threshold,
                                       bool junctionp,
                                       bool aggressive_junction_closure,
                                       int min_fit_length, double rms_distance)
{
  sdet_detector_params dp;

  dp.noise_multiplier = (float)noise_multiplier;
  dp.smooth = (float)smooth;
  dp.automatic_threshold = automatic_threshold;
  dp.junctionp = junctionp;
  dp.aggressive_junction_closure = aggressive_junction_closure;
  dp.borderp = false;

  sdet_fit_lines_params flp;
  flp.min_fit_length_ = min_fit_length;
  flp.rms_distance_ = rms_distance;

  sdet_detector det(dp);

  vil_image_resource_sptr img_res_sptr = vil_new_image_resource_of_view(img);
  unsigned ni = img.ni(); unsigned nj = img.nj();

  // initialize the output edge image
  vil_image_view<float> edge_img(img.ni(),img.nj(),3);
  edge_img.fill(-1.0f);

  det.SetImage(img_res_sptr);
  det.DoContour();
  std::vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  if (!edges)
  {
    std::cerr << "In sdet_img_edge::detect_edge_line_fitted() - No edges found in the image\n";
    return edge_img;
  }
  sdet_fit_lines fl(flp);
  fl.set_edges(*edges);
  fl.fit_lines();
  std::vector<vsol_line_2d_sptr> lines = fl.get_line_segs();

  for (const auto& l : lines) {
    int length = (int)std::ceil(l->length());
    double angle = vnl_math::angle_0_to_2pi(vnl_math::pi_over_180*l->tangent_angle());
#if 0
    std::cout << " line: " << i << " length: " << l->length()
             << " p0: (" << l->p0()->x() << ", " << l->p0()->y() << ')'
             << " p1: (" << l->p1()->x() << ", " << l->p1()->y() << ')'
             << " mid: (" << l->middle()->x() << ", " << l->middle()->y() << ")\n";
#endif
    std::vector<vsol_point_2d_sptr> samples; samples.push_back(l->p0()); samples.push_back(l->p1());
    vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d(samples);

    // now sample length many samples along the line
    float inc = 1.0f/(float)length;
    for (float index = 0.0f; index <= 1.0f; index += inc) {
      vgl_point_2d<double> pt = dc->interp((double)index);

      double cex = pt.x();
      double cey = pt.y();
      if (cex < 0 || cey < 0) continue;

      auto xc = static_cast<unsigned>(cex);
      auto yc = static_cast<unsigned>(cey);
      if (xc > ni || yc > nj) continue;

      edge_img(xc, yc, 0) = static_cast<float>(cex);
      edge_img(xc, yc, 1) = static_cast<float>(cey);
      edge_img(xc, yc, 2) = static_cast<float>(angle);
    }
  }
  return edge_img;
}


void
sdet_img_edge::convert_edge_image_to_line_image(vil_image_view<float>& edge_image, vil_image_view<float>& line_image)
{
  if (line_image.ni() != edge_image.ni() || line_image.nj() != edge_image.nj() ||
      line_image.nplanes() != edge_image.nplanes() || line_image.nplanes() != 3) {
    std::cerr << "In sdet_img_edge::convert_edge_image_to_line_image() -- incompatible input output image pair!\n";
    return;
  }

  line_image.fill(-2.0f);
  for (unsigned j = 0; j<line_image.nj(); ++j)
    for (unsigned i = 0; i<line_image.ni(); ++i) {
      float x = edge_image(i,j,0);
      float y = edge_image(i,j,1);
      if (x<0||y<0)
        continue;
      float angle = edge_image(i,j,2);
      vgl_vector_2d<float> tangent(std::cos(angle), std::sin(angle));
      vgl_point_2d<float> pt(x,y);
      vgl_line_2d<float> l(pt, tangent);
      float a = l.a(), b = l.b(), c = l.c();
      float norm = std::sqrt(a*a+b*b);
      a/=norm; b/=norm; c/=norm;
      line_image(i,j,0)= a;
      line_image(i,j,1)= b;
      line_image(i,j,2)= c;
    }
}

void sdet_img_edge::edge_distance_transform(vil_image_view<vxl_byte>& inp_image, vil_image_view<float>& out_edt)
{
  vil_image_view<vxl_byte> edge_image_negated(inp_image);
  vil_math_scale_and_offset_values<vxl_byte,double>(edge_image_negated,-1.0,255.0);

  unsigned ni = edge_image_negated.ni();
  unsigned nj = edge_image_negated.nj();

  vil_image_view<vxl_uint_32> curr_image_edt(ni,nj,1);
  for (unsigned i=0; i<ni; i++) {
    for (unsigned j=0; j<nj; j++) {
      curr_image_edt(i,j) = edge_image_negated(i,j);
    }
  }

  bil_edt_maurer(curr_image_edt);

  out_edt.set_size(ni,nj,1);
  for (unsigned i=0; i<ni; i++) {
    for (unsigned j=0; j<nj; j++) {
      out_edt(i,j) = std::sqrt((float)curr_image_edt(i,j));
    }
  }
}


/************************************************************************/
/* Functions related to estimating edge probability given an edge image */
/************************************************************************/

vil_image_view<float> sdet_img_edge::multiply_image_with_gaussian_kernel(vil_image_view<float> img, double gaussian_sigma)
{
  vil_image_view<float> ret_img(img.ni(),img.nj(),1);

  vnl_gaussian_kernel_1d gaussian(gaussian_sigma);

  for (unsigned i=0; i<img.ni(); i++) {
    for (unsigned j=0; j<img.nj(); j++) {
      ret_img(i,j) = (float)gaussian.G((double)img(i,j));
    }
  }
  return ret_img;
}

vbl_array_2d<float> sdet_img_edge::get_spherical_gaussian_kernel(const int size, const float sigma)
{
  assert(size>=3 && size%2==1);

  vbl_array_2d<float> kernel(size,size,0.0f);

  vnl_vector_fixed<float,2> mean(0.0f);
  float variance = (sigma*sigma);
  bsta_gaussian_sphere<float,2> gaussian(mean,variance);

  int center = (size-1)/2;

  vnl_vector_fixed<float,2> min_pt;
  vnl_vector_fixed<float,2> max_pt;
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      min_pt[0] = ((float)(i-center))-0.5f;
      min_pt[1] = ((float)(j-center))-0.5f;
      max_pt[0] = ((float)(i-center))+0.5f;
      max_pt[1] = ((float)(j-center))+0.5f;
      kernel(i,j) = gaussian.probability(min_pt,max_pt);
    }
  }
  return kernel;
}

void sdet_img_edge::estimate_edge_prob_image(const vil_image_view<vxl_byte>& img_edge, vil_image_view<float>& img_edgeness, const int mask_size, const float mask_sigma)
{
  vbl_array_2d<float> kernel = get_spherical_gaussian_kernel(mask_size,mask_sigma);

  for (unsigned i=0; i<kernel.rows(); i++) {
    for (unsigned j=0; j<kernel.columns(); j++) {
      kernel(i,j) = std::log(1.0f - kernel(i,j));
    }
  }

  vil_convert_cast<vxl_byte,float>(img_edge,img_edgeness);
  vil_math_scale_values<float>(img_edgeness,1.0/255.0);
  img_edgeness = brip_vil_float_ops::convolve(img_edgeness,kernel);

  for (unsigned i=0; i<img_edgeness.ni(); i++) {
    for (unsigned j=0; j<img_edgeness.nj(); j++) {
      img_edgeness(i,j) = 1.0f - std::exp(img_edgeness(i,j));
    }
  }
}

void sdet_img_edge::convert_true_edge_prob_to_edge_statistics(
  const vil_image_view<float>& img_tep,
  vil_image_view<float>& img_es)
{
  float img_mean,img_var;
  vil_math_mean_and_variance(img_mean,img_var,img_tep,0);
  img_var = std::sqrt(img_mean*img_var);

  img_es.set_size(img_tep.ni(),img_tep.nj());

  for (unsigned i=0; i<img_tep.ni(); i++) {
    for (unsigned j=0; j<img_tep.nj(); j++) {
      img_es(i,j) = ((img_tep(i,j)-img_mean)*(img_tep(i,j)-img_mean))/img_var;
    }
  }
}

float sdet_img_edge::convert_edge_statistics_to_probability(float edge_statistic, float  /*n_normal*/, int dof)
{
  if (dof<1) {
    return edge_statistic;
  }
  return (float)vnl_chi_squared_cumulative(edge_statistic,dof);
}
