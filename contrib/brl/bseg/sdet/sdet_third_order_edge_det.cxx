//This is brl/bseg/sdet/sdet_third_order_edge_det.cxx
#include "sdet_third_order_edge_det.h"

#include <vul/vul_timer.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

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
  int scale = (int) (vcl_pow(2.0, double(interp_factor_))+0.5);

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
    g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

  double conv_time = t.real();
  t.mark(); //reset timer

  //Now call the nms code to determine the set of edgels and their subpixel positions
  vcl_vector<vgl_point_2d<double> > edge_locs, edge_locations;
  vcl_vector<vgl_point_2d<int> > pix_locs;
  vcl_vector<double> orientation, mag, d2f;

  //parameters for reliable NMS
  double noise_sigma = 1.5;
  double rel_thresh = 1.3*noise_sigma/(sigma_*sigma_*sigma_);
  sdet_nms NMS(sdet_nms_params(thresh_, (sdet_nms_params::PFIT_TYPE)pfit_type_,
                                 static_cast<unsigned int>(vcl_pow(2.0,double(interp_factor_))*(4*sigma_+1)+0.5),
                                 rel_thresh, adapt_thresh_),
                grad_x, grad_y, grad_mag);

  //  NMS.apply(true, edge_locs, orientation, mag, d2f, pix_locs);
  NMS.apply(true, edge_locs, orientation, mag);

  double nms_time = t.real();
  t.mark(); //reset timer

  //convert to the original image scale coordinates
  for (unsigned i=0; i<edge_locs.size(); i++)
    edge_locations.push_back(vgl_point_2d<double>(edge_locs[i].x()/scale, edge_locs[i].y()/scale));

  //for each edge, compute all the gradients to compute the new orientation
  vcl_vector<double> Ix, Iy, Ixx, Ixy, Iyy, Ixxy, Ixyy, Ixxx, Iyyy;

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
  vcl_vector<double> edge_orientations(edge_locations.size());
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
    edge_orientations[i] = vnl_math::angle_0_to_2pi(vcl_atan2(Fx, -Fy));
  }

  double third_order_time = t.real();

  //report timings
  vcl_cout << '\n'
           << "time taken for conv: " << conv_time << " msec\n"
           << "time taken for nms: " << nms_time << " msec\n"
           << "time taken for third-order: " << third_order_time << " msec" << vcl_endl;

  //------------------------------------------------------------------------------------------
  // Compute the edge uncertainty measures

#if 0
  vcl_vector<double> edge_uncertainties(edge_locations.size());
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
  vcl_cout << "time taken for edgel encode: " << edgel_encode_time << " msec" << vcl_endl;
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
        int xx = vnl_math_rnd(edge_locations[i].x());
        int yy = vnl_math_rnd(edge_locations[i].y());

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

void sdet_third_order_edge_det::line_segs(vcl_vector<vsol_line_2d_sptr>& lines)
{
  lines.clear();
  for (unsigned i = 0; i<edgels_.size(); ++i)
  {
    double ang = edgels_[i].get_theta();
    double x = edgels_[i].x(), y = edgels_[i].y();
    double vx = vcl_cos(ang), vy = vcl_sin(ang);
    vsol_point_2d_sptr line_start = new vsol_point_2d(x-vx*0.25, y-vy*0.25);
    vsol_point_2d_sptr line_end = new vsol_point_2d(x+vx*0.25, y+vy*0.25);
    vsol_line_2d_sptr line = new vsol_line_2d(line_start, line_end);
    lines.push_back(line);
  }
}

//: save edgels in the edge map file FORMAT, output files have .edg extension
bool sdet_third_order_edge_det::save_edg_ascii(const vcl_string& filename, unsigned ni, unsigned nj, const vcl_vector<vdgl_edgel>& edgels)
{
  //1) If file open fails, return.
  vcl_ofstream outfp(filename.c_str(), vcl_ios::out);

  if (!outfp){
    vcl_cout << " Error opening file  " << filename.c_str() << vcl_endl;
    return false;
  }

  //2) write out the header block
  outfp << "# EDGE_MAP v3.0" << vcl_endl << vcl_endl;
  outfp << "# Format :  [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Strength Uncer" << vcl_endl;
  outfp << vcl_endl;
  outfp << "WIDTH=" << ni << vcl_endl;
  outfp << "HEIGHT=" << nj << vcl_endl;
  outfp << "EDGE_COUNT=" << edgels.size()  << vcl_endl;
  outfp << vcl_endl << vcl_endl;

  //save the edgel tokens
  for (unsigned k = 0; k < edgels.size(); k++) {
    vdgl_edgel edgel = edgels[k];
    double x = edgel.x();
    double y = edgel.y();

    unsigned ix = (unsigned)x;
    unsigned iy = (unsigned)y;
    double idir = edgel.get_theta();
    double iconf = edgel.get_grad();
    double dir = idir, conf = iconf, uncer = 0.0;
    outfp << "[" << ix << ", " << iy << "]    " << idir << " " << iconf << "   [" << x << ", " << y << "]   " << dir << " " << conf << " " << uncer << vcl_endl;
  }

  outfp.close();

  return true;
}
