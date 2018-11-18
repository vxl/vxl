#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include "bvxm_edge_util.h"

#include "bvxm_util.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>
#include <vnl/algo/vnl_gaussian_kernel_1d.h>
#include <vil/vil_resample_bilin.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <sdet/sdet_detector.h>
#include <vtol/vtol_edge_2d.h>
#include <vil/vil_new.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <bil/algo/bil_edt.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vnl/algo/vnl_chi_squared.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <brip/brip_vil_float_ops.h>
#include <cassert>

vil_image_view<float> bvxm_edge_util::multiply_image_with_gaussian_kernel(vil_image_view<float> img, double gaussian_sigma)
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

vil_image_view<vxl_byte> bvxm_edge_util::detect_edges(vil_image_view<vxl_byte> img,
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

void bvxm_edge_util::edge_distance_transform(vil_image_view<vxl_byte>& inp_image, vil_image_view<float>& out_edt)
{
  vil_image_view<vxl_byte> edge_image_negated(inp_image);
  vil_math_scale_and_offset_values(edge_image_negated,-1.0,255);

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

int bvxm_edge_util::convert_uncertainty_from_meters_to_pixels(float uncertainty, const vpgl_lvcs_sptr& lvcs, const vpgl_camera_double_sptr& camera)
{
  // estimate the offset search size in the image space
  vgl_box_3d<double> box_uncertainty(-uncertainty,-uncertainty,-uncertainty,uncertainty,uncertainty,uncertainty);
  std::vector<vgl_point_3d<double> > box_uncertainty_corners = bvxm_util::corners_of_box_3d<double>(box_uncertainty);
  auto* roi_uncertainty = new vgl_box_2d<double>();

  for (auto curr_corner : box_uncertainty_corners) {
    vgl_point_3d<double> curr_pt;
    if (camera->type_name()=="vpgl_local_rational_camera") {
      curr_pt.set(curr_corner.x(),curr_corner.y(),curr_corner.z());
    }
    else if (camera->type_name()=="vpgl_rational_camera") {
      double lon, lat, gz;
      lvcs->local_to_global(curr_corner.x(), curr_corner.y(), curr_corner.z(),
                          vpgl_lvcs::wgs84, lon, lat, gz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
      curr_pt.set(lon, lat, gz);
    }
    else // dummy initialisation, to avoid compiler warning
      curr_pt.set(-1e99, -1e99, -1.0);

    double curr_u,curr_v;
    camera->project(curr_pt.x(),curr_pt.y(),curr_pt.z(),curr_u,curr_v);
    vgl_point_2d<double> p2d_uncertainty(curr_u,curr_v);
    roi_uncertainty->add(p2d_uncertainty);
  }

  return vnl_math::ceil(0.5*std::max(roi_uncertainty->width(),roi_uncertainty->height()));
}

float bvxm_edge_util::convert_edge_statistics_to_probability(float edge_statistic, float n_normal, int dof)
{
  if (dof<1) {
    return edge_statistic;
  }

  if ((edge_statistic-n_normal)>0.0f) {
    double chi_sq_stat = (double)vnl_math::sqr((edge_statistic-n_normal))/n_normal;
    return (float)vnl_chi_squared_cumulative(chi_sq_stat,dof);
  }

  return 0.0f;
}

vbl_array_2d<float> bvxm_edge_util::get_spherical_gaussian_kernel(const int size, const float sigma)
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

void bvxm_edge_util::estimate_edge_prob_image(const vil_image_view<vxl_byte>& img_edge, vil_image_view<float>& img_edgeness, const int mask_size, const float mask_sigma)
{
  vbl_array_2d<float> kernel = get_spherical_gaussian_kernel(mask_size,mask_sigma);

  for (unsigned i=0; i<kernel.rows(); i++) {
    for (unsigned j=0; j<kernel.columns(); j++) {
      kernel(i,j) = std::log(1.0f - kernel(i,j));
    }
  }

  convert_image_types(img_edge,img_edgeness,1.0f/255.0f,0.0);
  img_edgeness = brip_vil_float_ops::convolve(img_edgeness,kernel);

  for (unsigned i=0; i<img_edgeness.ni(); i++) {
    for (unsigned j=0; j<img_edgeness.nj(); j++) {
      img_edgeness(i,j) = 1.0f - std::exp(img_edgeness(i,j));
    }
  }
}
