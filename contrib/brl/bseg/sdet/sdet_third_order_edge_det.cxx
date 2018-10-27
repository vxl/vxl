// This is brl/bseg/sdet/sdet_third_order_edge_det.cxx
#include "sdet_third_order_edge_det.h"
//:
// \file
#include <vul/vul_timer.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vil/vil_convert.h>

#include <bil/algo/bil_color_conversions.h>
#include <sdet/sdet_edgemap_sptr.h>
#include <sdet/sdet_edgemap.h>
#include <sdet/sdet_edgel.h>

// function to compute generic edges
void sdet_third_order_edge_det::apply(vil_image_view<vxl_byte> const& image)
{
  //convert to grayscale
  vil_image_view<vxl_byte> greyscale_view;
  if (image.nplanes() == 3) {
    vil_convert_planes_to_grey(image, greyscale_view );
  }
  else if (image.nplanes() == 1) {
    greyscale_view = image;
  }
  else
    return;

  //start the timer
  vul_timer t;

  //compute image gradients before performing nonmax suppression
  vil_image_view<double> grad_x, grad_y, grad_mag;
  int scale = 1 << interp_factor_; // 2^interp_factor_

  //compute gradients
  switch (grad_op_)
  {
    case 0: //Interpolated Gaussian
    {
      if (conv_algo_==0) { //2-d convolutions
        brip_subpix_convolve_2d(greyscale_view, grad_x, brip_Gx_kernel(sigma_), double(), interp_factor_);
        brip_subpix_convolve_2d(greyscale_view, grad_y, brip_Gy_kernel(sigma_), double(), interp_factor_);
      }
      else {
        brip_subpix_convolve_2d_sep(greyscale_view, grad_x, brip_Gx_kernel(sigma_), double(), interp_factor_);
        brip_subpix_convolve_2d_sep(greyscale_view, grad_y, brip_Gy_kernel(sigma_), double(), interp_factor_);
      }

      break;
    }
    case 1: //h0-operator
    {
      if (conv_algo_==0) { //2-d convolutions
        brip_subpix_convolve_2d(greyscale_view, grad_x, brip_h0_Gx_kernel(sigma_), double(), interp_factor_);
        brip_subpix_convolve_2d(greyscale_view, grad_y, brip_h0_Gy_kernel(sigma_), double(), interp_factor_);
      }
      else {
        brip_subpix_convolve_2d_sep(greyscale_view, grad_x, brip_h0_Gx_kernel(sigma_), double(), interp_factor_);
        brip_subpix_convolve_2d_sep(greyscale_view, grad_y, brip_h0_Gy_kernel(sigma_), double(), interp_factor_);
      }
      break;
    }
    case 2:  //h1-operator
    {
      if (conv_algo_==0) { //2-d convolutions
        brip_subpix_convolve_2d(greyscale_view, grad_x, brip_h1_Gx_kernel(sigma_), double(), interp_factor_);
        brip_subpix_convolve_2d(greyscale_view, grad_y, brip_h1_Gy_kernel(sigma_), double(), interp_factor_);
      }
      else {
        brip_subpix_convolve_2d_sep(greyscale_view, grad_x, brip_h1_Gx_kernel(sigma_), double(), interp_factor_);
        brip_subpix_convolve_2d_sep(greyscale_view, grad_y, brip_h1_Gy_kernel(sigma_), double(), interp_factor_);
      }
      break;
    }
  }

  //compute gradient magnitude
  grad_mag.set_size(grad_x.ni(), grad_x.nj());

  //get the pointers to the memory chunks
  double *gx  =  grad_x.top_left_ptr();
  double *gy  =  grad_y.top_left_ptr();
  double *g_mag  =  grad_mag.top_left_ptr();

  //compute the gradient magnitude
  for (unsigned long i=0; i<grad_mag.size(); i++)
    g_mag[i] = std::sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

  double conv_time = t.real();
  t.mark(); //reset timer

  //Now call the nms code to determine the set of edgels and their subpixel positions
  std::vector<vgl_point_2d<double> > edge_locs, edge_locations;
  std::vector<vgl_point_2d<int> > pix_locs;
  std::vector<double> orientation, mag, d2f;

  //parameters for reliable NMS
  double noise_sigma = 1.5;
  double rel_thresh = 1.3*noise_sigma/(sigma_*sigma_*sigma_);
  sdet_nms NMS(sdet_nms_params(thresh_, (sdet_nms_params::PFIT_TYPE)pfit_type_,
                               int(4*sigma_+1)*(1 << interp_factor_), // 2^interp_factor_
                               rel_thresh, adapt_thresh_),
               grad_x, grad_y, grad_mag);

  //  NMS.apply(true, edge_locs, orientation, mag, d2f, pix_locs);
  NMS.apply(true, edge_locs, orientation, mag);

  double nms_time = t.real();
  t.mark(); //reset timer

  //convert to the original image scale coordinates
  for (auto & edge_loc : edge_locs)
    edge_locations.emplace_back(edge_loc.x()/scale, edge_loc.y()/scale);

  //for each edge, compute all the gradients to compute the new orientation
  std::vector<double> Ix, Iy, Ixx, Ixy, Iyy, Ixxy, Ixyy, Ixxx, Iyyy;

  switch (grad_op_)
  {
    case 0: //Interpolated Gaussian
    {
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ix,   brip_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Iy,   brip_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixx,  brip_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixy,  brip_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Iyy,  brip_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixxx, brip_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixxy, brip_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixyy, brip_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Iyyy, brip_Gyyy_kernel(sigma_), double(), interp_factor_);
      break;
    }
    case 1: //h0-operator
    {
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ix,   brip_h0_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Iy,   brip_h0_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixx,  brip_h0_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixy,  brip_h0_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Iyy,  brip_h0_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixxx, brip_h0_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixxy, brip_h0_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixyy, brip_h0_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Iyyy, brip_h0_Gyyy_kernel(sigma_), double(), interp_factor_);
      break;
    }
    case 2:  //h1-operator
    {
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ix,   brip_h1_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Iy,   brip_h1_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixx,  brip_h1_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixy,  brip_h1_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Iyy,  brip_h1_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixxx, brip_h1_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixxy, brip_h1_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Ixyy, brip_h1_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(greyscale_view, edge_locations, Iyyy, brip_h1_Gyyy_kernel(sigma_), double(), interp_factor_);
      break;
    }
  }

  //Now, compute and update each edge with its new orientation
  std::vector<double> edge_orientations(edge_locations.size());
  for (unsigned i=0; i<edge_locations.size();i++)
  {
    //compute F
    //F[i]   = Ix[i]*Ix[i]*Ixx[i] + 2*Ix[i]*Iy[i]*Ixy[i] + Iy[i]*Iy[i]*Iyy[i];

    //compute Fx and Fy
    double Fx = 2*Ix[i]*Ixx[i]*Ixx[i] + 2*Ix[i]*Ixy[i]*Ixy[i] + 2*Ixx[i]*Iy[i]*Ixy[i] +
                2*Ixy[i]*Iyy[i]*Iy[i] + 2*Ix[i]*Iy[i]*Ixxy[i] + Ixyy[i]*Iy[i]*Iy[i] + Ix[i]*Ix[i]*Ixxx[i];
    double Fy = 2*Iy[i]*Iyy[i]*Iyy[i] + 2*Iy[i]*Ixy[i]*Ixy[i] + 2*Ixy[i]*Ix[i]*Ixx[i] +
                2*Ix[i]*Iyy[i]*Ixy[i]  + 2*Ix[i]*Iy[i]*Ixyy[i] + Ixxy[i]*Ix[i]*Ix[i] + Iyyy[i]*Iy[i]*Iy[i];

    //save new orientation
    edge_orientations[i] = vnl_math::angle_0_to_2pi(std::atan2(Fx, -Fy));
  }

  double third_order_time = t.real();

  //report timings
  std::cout << '\n'
           << "time taken for conv: " << conv_time << " msec\n"
           << "time taken for nms: " << nms_time << " msec\n"
           << "time taken for third-order: " << third_order_time << " msec" << std::endl;

  //------------------------------------------------------------------------------------------
  // Compute the edge uncertainty measures

#if 0
  std::vector<double> edge_uncertainties(edge_locations.size());
  sdet_edge_uncertainty_measure edge_uncer(grad_x, grad_y, sigma_);
  edge_uncer.get_edgel_uncertainties(edge_locs, edge_uncertainties);
#endif
  // --- new code ---
  t.mark(); //reset timer
  for (unsigned i=0; i<edge_locations.size(); i++)
  {
    vdgl_edgel ed(edge_locations[i].x(),edge_locations[i].y() , mag[i], edge_orientations[i]);
    edgels_.push_back(ed);
  }
  double edgel_encode_time = t.real();
  std::cout << "time taken for edgel encode: " << edgel_encode_time << " msec" << std::endl;

  view_i = greyscale_view.ni();
  view_j = greyscale_view.nj();
#if 0
  //------------------------------------------------------------------------------------------
  //create a new edgemap from the tokens identified by NMS
  sdet_edgemap_sptr edge_map;
  const bool interp_grid = false;
  const bool reduce_tokens = true;
  if (interp_grid)
    edge_map = new sdet_edgemap(grad_mag.ni(), grad_mag.nj());
  else
    edge_map = new sdet_edgemap(greyscale_view.ni(), greyscale_view.nj());

  for (unsigned i=0; i<edge_locations.size(); i++)
  {
    //Now insert them into the edge map appropriately
    if (interp_grid) {
      sdet_edgel* new_edgel = new sdet_edgel(edge_locs[i], edge_orientations[i], mag[i], d2f[i], edge_uncertainties[i]);
      new_edgel->gpt = pix_locs[i];
      edge_map->insert(new_edgel, pix_locs[i].x(), pix_locs[i].y());
    }
    else {
      if (reduce_tokens) {
        //only insert one edgel per grid position
        int xx = vnl_math::rnd(edge_locations[i].x());
        int yy = vnl_math::rnd(edge_locations[i].y());

        if (edge_map->edge_cells(yy, xx).size()>0)
          continue;

        sdet_edgel* new_edgel = new sdet_edgel(edge_locations[i], edge_orientations[i], mag[i], d2f[i], edge_uncertainties[i]);
        new_edgel->gpt = vgl_point_2d<int>(xx, yy);
        edge_map->insert(new_edgel, xx, yy);
      }
      else { //insert all of them
        sdet_edgel* new_edgel = new sdet_edgel(edge_locations[i], edge_orientations[i], mag[i], d2f[i], edge_uncertainties[i]);
        new_edgel->gpt = pix_locs[i];
        edge_map->insert(new_edgel);
      }
    }
  }

  //add intensity appearance to the edges
  //sdet_add_intensity_app(greyscale_view, edge_map, sigma, 1); //opt: 0: original , 1: smoothed, 2: Half gaussian

  return edge_map;
#endif
}

bool sdet_third_order_edge_det::apply_color(vil_image_view<vxl_byte> const& image)
{
  if (image.nplanes() != 3) {
    std::cerr << "In sdet_third_order_edge_det::apply_color() - only works with color images!\n";
    return false;
  }

  vil_image_view<float> comp1, comp2, comp3;
  convert_RGB_to_Lab(image, comp1, comp2, comp3);

  //start the timer
  vul_timer t;

  //4) compute image gradients of each of the components
  vil_image_view<float> f1_dx, f1_dy, f2_dx, f2_dy, f3_dx, f3_dy;
  int scale=1;

  switch (grad_op_)
  {
    case 0: //Gaussian
    {
      scale = 1 << interp_factor_; // 2^interp_factor_
      //compute gradients
      if (conv_algo_==0) {
        brip_subpix_convolve_2d(comp1, f1_dx, brip_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp1, f1_dy, brip_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp2, f2_dx, brip_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp2, f2_dy, brip_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp3, f3_dx, brip_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp3, f3_dy, brip_Gy_kernel(sigma_), float(), interp_factor_);
      }
      else {
        brip_subpix_convolve_2d_sep(comp1, f1_dx, brip_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp1, f1_dy, brip_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp2, f2_dx, brip_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp2, f2_dy, brip_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp3, f3_dx, brip_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp3, f3_dy, brip_Gy_kernel(sigma_), float(), interp_factor_);
      }
      break;
    }
    case 1: //h0-operator
    {
      scale = 1 << interp_factor_; // 2^interp_factor_

      //compute gradients
      if (conv_algo_==0) {
        brip_subpix_convolve_2d(comp1, f1_dx, brip_h0_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp1, f1_dy, brip_h0_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp2, f2_dx, brip_h0_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp2, f2_dy, brip_h0_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp3, f3_dx, brip_h0_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp3, f3_dy, brip_h0_Gy_kernel(sigma_), float(), interp_factor_);
      }
      else {
        brip_subpix_convolve_2d_sep(comp1, f1_dx, brip_h0_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp1, f1_dy, brip_h0_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp2, f2_dx, brip_h0_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp2, f2_dy, brip_h0_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp3, f3_dx, brip_h0_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp3, f3_dy, brip_h0_Gy_kernel(sigma_), float(), interp_factor_);
      }

      break;
    }
    case 2:  //h1-operator
    {
      scale = 1 << interp_factor_; // 2^interp_factor_

      //compute gradients
      if (conv_algo_==0) {
        brip_subpix_convolve_2d(comp1, f1_dx, brip_h1_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp1, f1_dy, brip_h1_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp2, f2_dx, brip_h1_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp2, f2_dy, brip_h1_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp3, f3_dx, brip_h1_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d(comp3, f3_dy, brip_h1_Gy_kernel(sigma_), float(), interp_factor_);
      }
      else {
        brip_subpix_convolve_2d_sep(comp1, f1_dx, brip_h1_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp1, f1_dy, brip_h1_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp2, f2_dx, brip_h1_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp2, f2_dy, brip_h1_Gy_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp3, f3_dx, brip_h1_Gx_kernel(sigma_), float(), interp_factor_);
        brip_subpix_convolve_2d_sep(comp3, f3_dy, brip_h1_Gy_kernel(sigma_), float(), interp_factor_);
      }
      break;
    }
  }

  //5) compute the squared norm of the vector-gradient
  vil_image_view<double> grad_mag, nu1, nu2; //eigenvalue and eigenvector
  grad_mag.set_size(f1_dx.ni(), f1_dx.nj());
  nu1.set_size(f1_dx.ni(), f1_dx.nj());
  nu2.set_size(f1_dx.ni(), f1_dx.nj());

  //get the pointers to the memory chunks
  float *f1x  =  f1_dx.top_left_ptr();
  float *f1y  =  f1_dy.top_left_ptr();
  float *f2x  =  f2_dx.top_left_ptr();
  float *f2y  =  f2_dy.top_left_ptr();
  float *f3x  =  f3_dx.top_left_ptr();
  float *f3y  =  f3_dy.top_left_ptr();
  double *g_mag  =  grad_mag.top_left_ptr();
  double *n1  =  nu1.top_left_ptr();
  double *n2  =  nu2.top_left_ptr();

  //compute the squared norm of gradient
  for (unsigned long i=0; i<grad_mag.size(); i++) {
    double A = f1x[i]*f1x[i]+f2x[i]*f2x[i]+f3x[i]*f3x[i];
    double B = f1x[i]*f1y[i]+f2x[i]*f2y[i]+f3x[i]*f3y[i];
    double C = f1y[i]*f1y[i]+f2y[i]*f2y[i]+f3y[i]*f3y[i];

    double l = (A+C+ std::sqrt((A-C)*(A-C) + 4*B*B))/2;

    if (std::fabs(B)>1e-2) {
      n1[i] = B/std::sqrt(B*B+(l-A)*(l-A));
      n2[i] = (l-A)/std::sqrt(B*B+(l-A)*(l-A));
    }
    else {
      n1[i] = (l-C)/std::sqrt(B*B+(l-C)*(l-C));
      n2[i] = B/std::sqrt(B*B+(l-C)*(l-C));
    }

    g_mag[i] = std::sqrt(l/3); //take the square root of the squared norm
  }


  double conv_time = t.real();
  t.mark(); //reset timer

  //Now call the nms code to get the subpixel edge tokens
  std::vector<vgl_point_2d<double> > edge_locations;
  std::vector<double> orientation, mag, d2f;

  sdet_nms NMS(sdet_nms_params(thresh_, (sdet_nms_params::PFIT_TYPE)pfit_type_), nu1, nu2, grad_mag);
  NMS.apply(true, edge_locations, orientation, mag, d2f);

  double nms_time = t.real();
  t.mark(); //reset timer

  //convert to the original image scale coordinates
  for (auto & edge_location : edge_locations)
    edge_location.set(edge_location.x()/scale, edge_location.y()/scale);

  //for each edge, compute all the gradients to compute the new orientation
  std::vector<double> If1x, If1y, If1xx, If1xy, If1yy, If1xxy, If1xyy, If1xxx, If1yyy;
  std::vector<double> If2x, If2y, If2xx, If2xy, If2yy, If2xxy, If2xyy, If2xxx, If2yyy;
  std::vector<double> If3x, If3y, If3xx, If3xy, If3yy, If3xxy, If3xyy, If3xxx, If3yyy;

  switch (grad_op_)
  {
    case 0: //Interpolated Gaussian
    {
      brip_subpix_convolve_2d(comp1, edge_locations, If1x,   brip_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1y,   brip_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xx,  brip_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xy,  brip_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1yy,  brip_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xxx, brip_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xxy, brip_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xyy, brip_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1yyy, brip_Gyyy_kernel(sigma_), double(), interp_factor_);

      brip_subpix_convolve_2d(comp2, edge_locations, If2x,   brip_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2y,   brip_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xx,  brip_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xy,  brip_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2yy,  brip_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xxx, brip_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xxy, brip_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xyy, brip_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2yyy, brip_Gyyy_kernel(sigma_), double(), interp_factor_);

      brip_subpix_convolve_2d(comp3, edge_locations, If3x,   brip_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3y,   brip_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xx,  brip_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xy,  brip_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3yy,  brip_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xxx, brip_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xxy, brip_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xyy, brip_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3yyy, brip_Gyyy_kernel(sigma_), double(), interp_factor_);

      break;
    }
    case 1: //h0-operator
    {
      brip_subpix_convolve_2d(comp1, edge_locations, If1x,   brip_h0_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1y,   brip_h0_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xx,  brip_h0_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xy,  brip_h0_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1yy,  brip_h0_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xxx, brip_h0_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xxy, brip_h0_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xyy, brip_h0_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1yyy, brip_h0_Gyyy_kernel(sigma_), double(), interp_factor_);

      brip_subpix_convolve_2d(comp2, edge_locations, If2x,   brip_h0_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2y,   brip_h0_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xx,  brip_h0_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xy,  brip_h0_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2yy,  brip_h0_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xxx, brip_h0_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xxy, brip_h0_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xyy, brip_h0_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2yyy, brip_h0_Gyyy_kernel(sigma_), double(), interp_factor_);

      brip_subpix_convolve_2d(comp3, edge_locations, If3x,   brip_h0_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3y,   brip_h0_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xx,  brip_h0_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xy,  brip_h0_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3yy,  brip_h0_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xxx, brip_h0_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xxy, brip_h0_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xyy, brip_h0_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3yyy, brip_h0_Gyyy_kernel(sigma_), double(), interp_factor_);

      break;
    }
    case 2:  //h1-operator
    {
      brip_subpix_convolve_2d(comp1, edge_locations, If1x,   brip_h1_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1y,   brip_h1_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xx,  brip_h1_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xy,  brip_h1_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1yy,  brip_h1_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xxx, brip_h1_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xxy, brip_h1_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1xyy, brip_h1_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp1, edge_locations, If1yyy, brip_h1_Gyyy_kernel(sigma_), double(), interp_factor_);

      brip_subpix_convolve_2d(comp2, edge_locations, If2x,   brip_h1_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2y,   brip_h1_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xx,  brip_h1_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xy,  brip_h1_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2yy,  brip_h1_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xxx, brip_h1_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xxy, brip_h1_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2xyy, brip_h1_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp2, edge_locations, If2yyy, brip_h1_Gyyy_kernel(sigma_), double(), interp_factor_);

      brip_subpix_convolve_2d(comp3, edge_locations, If3x,   brip_h1_Gx_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3y,   brip_h1_Gy_kernel(sigma_),   double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xx,  brip_h1_Gxx_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xy,  brip_h1_Gxy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3yy,  brip_h1_Gyy_kernel(sigma_),  double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xxx, brip_h1_Gxxx_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xxy, brip_h1_Gxxy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3xyy, brip_h1_Gxyy_kernel(sigma_), double(), interp_factor_);
      brip_subpix_convolve_2d(comp3, edge_locations, If3yyy, brip_h1_Gyyy_kernel(sigma_), double(), interp_factor_);

      break;
    }
  }

  //Now, compute and update each edge with its new orientation
  std::vector<double> edge_orientations(edge_locations.size());

  for (unsigned i=0; i<edge_locations.size();i++)
  {
    double A   = If1x[i]*If1x[i] + If2x[i]*If2x[i] + If3x[i]*If3x[i];
    double Ax  = 2*If1x[i]*If1xx[i] + 2*If2x[i]*If2xx[i] + 2*If3x[i]*If3xx[i];
    double Ay  = 2*If1x[i]*If1xy[i] + 2*If2x[i]*If2xy[i] + 2*If3x[i]*If3xy[i];
    double Axx = 2*If1x[i]*If1xxx[i] + 2*If1xx[i]*If1xx[i] + 2*If2x[i]*If2xxx[i] + 2*If2xx[i]*If2xx[i] + 2*If3x[i]*If3xxx[i] + 2*If3xx[i]*If3xx[i];
    double Axy = 2*If1x[i]*If1xxy[i] + 2*If1xy[i]*If1xx[i] + 2*If2x[i]*If2xxy[i] + 2*If2xy[i]*If2xx[i] + 2*If3x[i]*If3xxy[i] + 2*If3xy[i]*If3xx[i];
    double Ayy = 2*If1x[i]*If1xyy[i] + 2*If1xy[i]*If1xy[i] + 2*If2x[i]*If2xyy[i] + 2*If2xy[i]*If2xy[i] + 2*If3x[i]*If3xyy[i] + 2*If3xy[i]*If3xy[i];

    double B   = If1x[i]*If1y[i] + If2x[i]*If2y[i] + If3x[i]*If3y[i];
    double Bx  = If1x[i]*If1xy[i] + If1y[i]*If1xx[i] + If2x[i]*If2xy[i] + If2y[i]*If2xx[i] + If3x[i]*If3xy[i] + If3y[i]*If3xx[i];
    double By  = If1x[i]*If1yy[i] + If1y[i]*If1xy[i] + If2x[i]*If2yy[i] + If2y[i]*If2xy[i] + If3x[i]*If3yy[i] + If3y[i]*If3xy[i];
    double Bxx = If1x[i]*If1xxy[i] + If1xx[i]*If1xy[i] + If1y[i]*If1xxx[i] + If1xy[i]*If1xx[i] + If2x[i]*If2xxy[i] + If2xx[i]*If2xy[i] + If2y[i]*If2xxx[i] + If2xy[i]*If2xx[i] + If3x[i]*If3xxy[i] + If3xx[i]*If3xy[i] + If3y[i]*If3xxx[i] + If3xy[i]*If3xx[i];
    double Bxy = If1x[i]*If1xyy[i] + If1xy[i]*If1xy[i] + If1y[i]*If1xxy[i] + If1yy[i]*If1xx[i] + If2x[i]*If2xyy[i] + If2xy[i]*If2xy[i] + If2y[i]*If2xxy[i] + If2yy[i]*If2xx[i] + If3x[i]*If3xyy[i] + If3xy[i]*If3xy[i] + If3y[i]*If3xxy[i] + If3yy[i]*If3xx[i];
    double Byy = If1x[i]*If1yyy[i] + If1xy[i]*If1yy[i] + If1y[i]*If1xyy[i] + If1yy[i]*If1xy[i] + If2x[i]*If2yyy[i] + If2xy[i]*If2yy[i] + If2y[i]*If2xyy[i] + If2yy[i]*If2xy[i] + If3x[i]*If3yyy[i] + If3xy[i]*If3yy[i] + If3y[i]*If3xyy[i] + If3yy[i]*If3xy[i];

    double C   = If1y[i]*If1y[i] + If2y[i]*If2y[i] + If3y[i]*If3y[i];
    double Cx  = 2*If1y[i]*If1xy[i] + 2*If2y[i]*If2xy[i] + 2*If3y[i]*If3xy[i];
    double Cy  = 2*If1y[i]*If1yy[i] + 2*If2y[i]*If2yy[i] + 2*If3y[i]*If3yy[i];
    double Cxx = 2*If1y[i]*If1xxy[i] + 2*If1xy[i]*If1xy[i] + 2*If2y[i]*If2xxy[i] + 2*If2xy[i]*If2xy[i] + 2*If3y[i]*If3xxy[i] + 2*If3xy[i]*If3xy[i];
    double Cxy = 2*If1y[i]*If1xyy[i] + 2*If1yy[i]*If1xy[i] + 2*If2y[i]*If2xyy[i] + 2*If2yy[i]*If2xy[i] + 2*If3y[i]*If3xyy[i] + 2*If3yy[i]*If3xy[i];
    double Cyy = 2*If1y[i]*If1yyy[i] + 2*If1yy[i]*If1yy[i] + 2*If2y[i]*If2yyy[i] + 2*If2yy[i]*If2yy[i] + 2*If3y[i]*If3yyy[i] + 2*If3yy[i]*If3yy[i];

    double l = ((C+A) + std::sqrt((A-C)*(A-C) + 4*B*B))/2.0;
    double e = (2*l-A-C);

    double lx = ( (l-C)*Ax + (l-A)*Cx + 2*B*Bx)/e;
    double ly = ( (l-C)*Ay + (l-A)*Cy + 2*B*By)/e;
    double lxx = ( (lx-Cx)*Ax + (l-C)*Axx + (lx-Ax)*Cx + (l-A)*Cxx + 2*Bx*Bx + 2*B*Bxx - lx*(2*lx-Cx-Ax))/e;
    double lxy = ( (ly-Cy)*Ax + (l-C)*Axy + (ly-Ay)*Cx + (l-A)*Cxy + 2*Bx*By + 2*B*Bxy - lx*(2*ly-Cy-Ay))/e;
    double lyy = ( (ly-Cy)*Ay + (l-C)*Ayy + (ly-Ay)*Cy + (l-A)*Cyy + 2*By*By + 2*B*Byy - ly*(2*ly-Cy-Ay))/e;

    /********************************************************************************
    // This computation is noisy
    double n1 = std::sqrt((1.0 + (A-C)/d)/2.0);
    double n2 = vnl_math::sgn(B)*std::sqrt((1.0 - (A-C)/d)/2.0);

    // when B is zero, these derivatives need to be corrected to the limiting value
    double n1x, n1y, n2x, n2y;
    if (std::fabs(B)>1e-2) {
      n1x = ( 2*(Ax-Cx)/d - (A-C)*(2*(C-A)*(Cx-Ax) + 8*B*Bx)/(d*d*d))/2/n1;
      n1y = ( 2*(Ay-Cy)/d - (A-C)*(2*(C-A)*(Cy-Ay) + 8*B*By)/(d*d*d))/2/n1;
      n2x = (-2*(Ax-Cx)/d + (A-C)*(2*(C-A)*(Cx-Ax) + 8*B*Bx)/(d*d*d))/2/n2;
      n2y = (-2*(Ay-Cy)/d + (A-C)*(2*(C-A)*(Cy-Ay) + 8*B*By)/(d*d*d))/2/n2;
    }
    else {
      n1x = 0;
      n1y = 0;
      n2x = 0;
      n2y = 0;
    }
    ********************************************************************************/

    double n1, n2, n1x, n1y, n2x, n2y;
    // when B is zero, these derivatives need to be fixed
    if (std::fabs(B)>1e-2) {
      double f = std::sqrt(B*B+(l-A)*(l-A));

      n1 = B/f;
      n2 = (l-A)/f;

      n1x = Bx/f - B*(B*Bx + (l-A)*(lx-Ax))/(f*f*f);
      n1y = By/f - B*(B*By + (l-A)*(ly-Ay))/(f*f*f);
      n2x = (lx-Ax)/f - (l-A)*(B*Bx + (l-A)*(lx-Ax))/(f*f*f);
      n2y = (ly-Ay)/f - (l-A)*(B*By + (l-A)*(ly-Ay))/(f*f*f);
    }
    else {
      double f = std::sqrt(B*B+(l-C)*(l-C));

      n1 = (l-C)/f;
      n2 = B/f;

      n1x = (lx-Cx)/f - (l-C)*(B*Bx + (l-C)*(lx-Cx))/(f*f*f);
      n1y = (ly-Cy)/f - (l-C)*(B*By + (l-C)*(ly-Cy))/(f*f*f);
      n2x = Bx/f - B*(B*Bx + (l-C)*(lx-Cx))/(f*f*f);
      n2y = By/f - B*(B*By + (l-C)*(ly-Cy))/(f*f*f);
    }

    //compute Fx and Fy
    double Fx = lx*n1x + n1*lxx + n2x*ly + n2*lxy;
    double Fy = lx*n1y + n1*lxy + n2y*ly + n2*lyy;

    //save new orientation
    edge_orientations[i] = vnl_math::angle_0_to_2pi(std::atan2(Fx, -Fy));
  }

  double third_order_time = t.real();

  //report timings
  std::cout << '\n'
           << "time taken for conv: " << conv_time << " msec\n"
           << "time taken for nms: " << nms_time << " msec\n"
           << "time taken for color third-order: " << third_order_time << " msec" << std::endl;

  // --- new code ---
  t.mark(); //reset timer
  for (unsigned i=0; i<edge_locations.size(); i++)
  {
    vdgl_edgel ed(edge_locations[i].x(),edge_locations[i].y() , mag[i], edge_orientations[i]);
    edgels_.push_back(ed);
  }
  double edgel_encode_time = t.real();
  std::cout << "time taken for edgel encode: " << edgel_encode_time << " msec" << std::endl;

  view_i = comp1.ni();
  view_j = comp1.nj();
#if 0
  //create a new edgemap from the tokens collected from NMS
  brip_edgemap_sptr edge_map = new brip_edgemap(comp1.ni(), comp1.nj());

  for (unsigned i=0; i<edge_locations.size(); i++) {
    if (reduce_tokens) {
      //only insert one edgel per grid position
      int xx = brip_round(edge_locations[i].x());
      int yy = brip_round(edge_locations[i].y());

      if (edge_map->edge_cells(yy, xx).size()>0)
        continue;

      edge_map->insert(new brip_edgel(edge_locations[i], edge_orientations[i], mag[i], d2f[i]));
    }
    else { //insert all of them
      edge_map->insert(new brip_edgel(edge_locations[i], edge_orientations[i], mag[i], d2f[i]));
    }
  }

  //add intensity appearance to the edges
  brip_add_color_app(comp1, comp2, comp3, edge_map, sigma_, 1); //opt: 0: original , 1: smoothed, 2: Half gaussian

  return edge_map;
#endif

  return true;
}

void sdet_third_order_edge_det::line_segs(std::vector<vsol_line_2d_sptr>& lines)
{
  lines.clear();
  for (auto & edgel : edgels_)
  {
    double ang = edgel.get_theta();
    double x = edgel.x(), y = edgel.y();
    double vx = std::cos(ang), vy = std::sin(ang);
    vsol_point_2d_sptr line_start = new vsol_point_2d(x-vx*0.25, y-vy*0.25);
    vsol_point_2d_sptr line_end = new vsol_point_2d(x+vx*0.25, y+vy*0.25);
    vsol_line_2d_sptr line = new vsol_line_2d(line_start, line_end);
    lines.push_back(line);
  }
}

//: save edgels in the edge map file FORMAT, output files have .edg extension
bool sdet_third_order_edge_det::save_edg_ascii(const std::string& filename, unsigned ni, unsigned nj, const std::vector<vdgl_edgel>& edgels)
{
  //1) If file open fails, return.
  std::ofstream outfp(filename.c_str(), std::ios::out);

  if (!outfp) {
    std::cerr << "*** Error opening file " << filename.c_str() << '\n';
    return false;
  }

  //2) write out the header block
  outfp << "# EDGE_MAP v3.0\n\n"
        << "# Format :  [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Strength Uncer\n\n"
        << "WIDTH=" << ni << '\n'
        << "HEIGHT=" << nj << '\n'
        << "EDGE_COUNT=" << edgels.size()  << '\n'
        << '\n' << std::endl;

  //save the edgel tokens
  for (auto edgel : edgels) {
    double x = edgel.x();
    double y = edgel.y();

    auto ix = (unsigned)x;
    auto iy = (unsigned)y;
    double idir = edgel.get_theta();
    double iconf = edgel.get_grad();
    double dir = idir, conf = iconf, uncer = 0.0;
    outfp << '[' << ix << ", " << iy << "]    "
          << idir << ' ' << iconf << "   [" << x << ", " << y << "]   "
          << dir << ' ' << conf << ' ' << uncer << std::endl;
  }

  outfp.close();

  return true;
}

sdet_edgemap_sptr sdet_third_order_edge_det::edgemap() {

  sdet_edgemap_sptr edge_map;
  edge_map = new sdet_edgemap(view_i, view_j);

  for (auto & edgel : edgels_)
  {
    sdet_edgel* new_edgel = new sdet_edgel(edgel.get_pt(), edgel.get_theta(), edgel.get_grad());
    edge_map->insert(new_edgel);
  }
  return edge_map;
}
