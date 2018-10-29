// This is contrib/brl/bbas/volm/pro/process/volm_dsm_ground_plane_estimation_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file
//     process to estimate a ground plane from original height maps
//
// \verbatim
//  Modifications
//    none yet
// \endverbatim

#include <vil/vil_math.h>
#include <bsta/bsta_histogram.h>
#include <mbl/mbl_thin_plate_spline_3d.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_math.h>
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vil/vil_save.h>

namespace volm_dsm_ground_plane_estimation_process_globals
{
  constexpr unsigned int n_inputs_ = 4;
  constexpr unsigned int n_outputs_ = 1;
}

bool volm_dsm_ground_plane_estimation_process_cons(bprb_func_process& pro)
{
  using namespace volm_dsm_ground_plane_estimation_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";       // original height map image
  input_types_[1] = "int";                            // number of height value estimations to use, bottom N, e.g. 100
  input_types_[2] = "int";                            // window size to get minimum height value around the fitting points.
  input_types_[3] = "float";                          // Invalid pixel value
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr"; // output ground plane image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_dsm_ground_plane_estimation_process(bprb_func_process& pro)
{
  using namespace volm_dsm_ground_plane_estimation_process_globals;
  if (!pro.verify_inputs())  {
    std::cerr << pro.name() << ": Wring inputs!!!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr img_res = pro.get_input<vil_image_view_base_sptr>(in_i++);
  int N = pro.get_input<int>(in_i++);
  int window_size = pro.get_input<int>(in_i++);
  auto invalid_pixel = pro.get_input<float>(in_i++);

  auto* in_img = dynamic_cast<vil_image_view<float>*>(img_res.ptr());
  if (!in_img) {
    std::cerr << pro.name() << ": Unsupported image pixel format -- " << img_res->pixel_format() << ", only float is supported!\n";
    return false;
  }
  int ni = static_cast<int>(in_img->ni());
  int nj = static_cast<int>(in_img->nj());
  auto* out = new vil_image_view<float>(ni, nj);

  std::vector<vgl_point_3d<double> > src_pts, dst_pts;

#if 1
  for (int i = N; i < ni-N; i+=N) {
    int s_i = i - window_size;  int e_i = i + window_size;
    if (s_i < 0)   s_i = 0;
    if (e_i > ni)  e_i = ni;
    for (int j = N; j < nj-N; j+=N) {
      int s_j = j - window_size;  int e_j = j + window_size;
      if (s_j < 0)    s_j = 0;
      if (e_j > nj)   e_j = nj;
      float min = 1000000.0f;
      for (int ii = s_i; ii < e_i; ii++) {
        for (int jj = s_j; jj < e_j; jj++) {
          if ( std::abs((*in_img)(ii,jj)-invalid_pixel) < 1E-3 )
            continue;
          if ( (*in_img)(ii,jj) < min )
            min = (*in_img)(ii,jj);
        }
      }
      if (min < 1000000.0f) {
        double img_val = (*in_img)(i,j);
        vgl_point_3d<double> pt1(i,j,img_val);
        vgl_point_3d<double>  pt(i,j,min);
        src_pts.push_back(pt1);
        dst_pts.push_back(pt);
      }
    }
  }
#endif
#if 0
  for (int i = 0; i < ni; i++)
  {
    int s_i = i - N;  int e_i = i + N;
    if (s_i < 0)   s_i = 0;
    if (e_i > ni)  e_i = ni;
    for (int j = 0; j < nj; j++)
    {
      int s_j = j - N;  int e_j = j + N;
      if (s_j < 0)    s_j = 0;
      if (e_j > nj)   e_j = nj;
      float min = 1000000.0f;
      for (int ii = s_i; ii < e_i; ii++) {
        for (int jj = s_j; jj < e_j; jj++) {
          if ( std::abs((*in_img)(ii,jj)-invalid_pixel) < 1E-3 )
            continue;
          if ( (*in_img)(ii,jj) < min )
            min = (*in_img)(ii,jj);
        }
      }
      if (min < 1000000.0f) {
        double img_val = (*in_img)(i,j);
        vgl_point_3d<double> pt1(i,j,img_val);
        vgl_point_3d<double>  pt(i,j,min);
        src_pts.push_back(pt1);
        dst_pts.push_back(pt);
      }
    }
  }
#endif
#if 0
  for (int j = N; j < nj-N; j += N)
  {
    for (int i = N; i < ni-N; i+=N)
    {
      // find the minimum in the neighborhood
      float min = 10000000.0f;
      bool found = false;
      for (int k = -N; k < N; k++) {
        for (int m = -N; m < N; m++) {
          int uu = i+k;
          int vv = j+m;
          if ( std::abs((*in_img)(uu,vv)-invalid_pixel) < 1E-3 )
            continue;
          if ( (*in_img)(uu,vv) < min ) {
            min = (*in_img)(uu,vv);
            found = true;
          }
        }
      }
      if (found) {
        double img_val = (*in_img)(i,j);
        vgl_point_3d<double> pt1(i,j,img_val);
        vgl_point_3d<double>  pt(i,j,min);
        src_pts.push_back(pt1);
        dst_pts.push_back(pt);
      }
    }
  }
#endif

  std::cout << pro.name() << ": Total image pixels: " << ni*nj << ", size of source pts: " << src_pts.size() << std::endl;

  // construct spline object
  mbl_thin_plate_spline_3d tps;
  tps.build(src_pts, dst_pts);

  // apply to points
  out->fill(invalid_pixel);
  for (int i = 0; i < ni; i++) {
    for (int j = 0; j < nj; j++) {
      if (std::abs((*in_img)(i,j)-invalid_pixel) < 1E-3 )
        continue;
      vgl_point_3d<double> p(i,j,(*in_img)(i,j));
      vgl_point_3d<double> new_p = tps(p);
      (*out)(i,j) = new_p.z();
    }
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, out);
  return true;

}


//: process to estimate a ground plane from original height maps using an edge map.  The edge map is used to control windows size
namespace volm_dsm_ground_plane_estimation_edge_process_globals
{
  constexpr unsigned int n_inputs_ = 4;
  constexpr unsigned int n_outputs_ = 1;

  int window_size_from_nearest_edge(vil_image_view<vxl_byte>* edge_img, int const& i, int const& j, int const& search_range);
}

bool volm_dsm_ground_plane_estimation_edge_process_cons(bprb_func_process& pro)
{
  using namespace volm_dsm_ground_plane_estimation_edge_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";       // original height map image
  input_types_[1] = "vil_image_view_base_sptr";       // edge image used to obtain the search window size
  input_types_[2] = "int";                            // number of height value estimations to use, bottom N, e.g. 100
  input_types_[3] = "float";                          // invalid pixel value
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr"; // output ground plane image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_dsm_ground_plane_estimation_edge_process(bprb_func_process& pro)
{
  using namespace volm_dsm_ground_plane_estimation_edge_process_globals;
  if (!pro.verify_inputs())  {
    std::cerr << pro.name() << ": Wring inputs!!!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr img_res = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr edge_img_res = pro.get_input<vil_image_view_base_sptr>(in_i++);
  int N = pro.get_input<int>(in_i++);
  auto invalid_pixel = pro.get_input<float>(in_i++);

  auto* in_img = dynamic_cast<vil_image_view<float>*>(img_res.ptr());
  if (!in_img) {
    std::cerr << pro.name() << ": Unsupported image pixel format -- " << img_res->pixel_format() << ", only float is supported!\n";
    return false;
  }
  auto* edge_img = dynamic_cast<vil_image_view<vxl_byte>*>(edge_img_res.ptr());
  if (!edge_img) {
    std::cerr << pro.name() << ": Unsupported edge image pixel format -- " << edge_img_res->pixel_format() << ", only byte is supported!\n";
    return false;
  }
  int ni = static_cast<int>(in_img->ni());
  int nj = static_cast<int>(in_img->nj());
  if (ni != edge_img->ni() || nj != edge_img->nj()) {
    std::cerr << pro.name() << ": height image and edge image size do not match!\n";
    return false;
  }
  auto* out = new vil_image_view<float>(ni, nj);
  int search_range = ni;
  if (search_range > nj)
    search_range = nj;
  std::vector<vgl_point_3d<double> > src_pts, dst_pts;
  for (int i = N; i < ni-N; i+=N)
  {
    for (int j = N; j < nj-N; j+=N)
    {
      // find appropriate window size for current pixel
      int window_size = window_size_from_nearest_edge(edge_img, i, j, search_range);
      int s_i = i - window_size;  int e_i = i + window_size;
      if (s_i < 0)   s_i = 0;
      if (e_i > ni)  e_i = ni;
      int s_j = j - window_size;  int e_j = j + window_size;
      if (s_j < 0)    s_j = 0;
      if (e_j > nj)   e_j = nj;
      float min = 1000000.0f;
      for (int ii = s_i; ii < e_i; ii++) {
        for (int jj = s_j; jj < e_j; jj++) {
          if ( std::abs((*in_img)(ii,jj)-invalid_pixel) < 1E-3 )
            continue;
          if ( (*in_img)(ii,jj) < min )
            min = (*in_img)(ii,jj);
        }
      }
      if (min < 1000000.0f) {
        double img_val = (*in_img)(i,j);
        vgl_point_3d<double> pt1(i,j,img_val);
        vgl_point_3d<double>  pt(i,j,min);
        src_pts.push_back(pt1);
        dst_pts.push_back(pt);
      }
    }
  }
  std::cout << pro.name() << ": Total image pixels: " << ni*nj << ", size of source pts: " << src_pts.size() << std::endl;
  for (unsigned i = 0; i < src_pts.size(); i++) {
    std::cout << "source: " << src_pts[i].x() << " " << src_pts[i].y() << " " << src_pts[i].z()
             << "  dest: " << dst_pts[i].x() << " " << dst_pts[i].y() << " " << dst_pts[i].z() << std::endl;
  }

  // construct spline object
  mbl_thin_plate_spline_3d tps;
  tps.build(src_pts, dst_pts);

  // apply to points
  out->fill(invalid_pixel);
  for (int i = 0; i < ni; i++) {
    for (int j = 0; j < nj; j++) {
      if (std::abs((*in_img)(i,j)-invalid_pixel) < 1E-3 )
        continue;
      vgl_point_3d<double> p(i,j,(*in_img)(i,j));
      vgl_point_3d<double> new_p = tps(p);
      (*out)(i,j) = new_p.z();
    }
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, out);
  return true;
}

int volm_dsm_ground_plane_estimation_edge_process_globals::window_size_from_nearest_edge(vil_image_view<vxl_byte>* edge_img,
                                                                                         int const& i, int const& j,
                                                                                         int const& search_range)
{
  // if the pixel itself is an edge, return an default search window
  if ( (*edge_img)(i,j))
    return 30;
  bool found_neighbor = false;
  int  edge_window_size = 0;
  unsigned num_nbrs = 8;
  unsigned ni = edge_img->ni();
  unsigned nj = edge_img->nj();
  // keep increasing search radius
  for (int radius = 1; (radius < search_range && !found_neighbor); radius++)
  {
    int start_i = i - radius;  int end_i = i + radius;
    int start_j = j - radius;  int end_j = j + radius;
    for (int ii = start_i; (ii < end_i && !found_neighbor); ii++) {
      for (int jj = start_j; (jj < end_j && !found_neighbor); jj++) {
        if (ii < 0 || jj < 0 || ii >= (int)ni || jj >= (int)nj)
          continue;
        if ( (*edge_img)(ii, jj) != 0) {
          found_neighbor = true;
          edge_window_size = radius;
        }
      }
    }
  }
  if (!edge_window_size)
    return 30;
  edge_window_size += 30;
  return edge_window_size;
}

// Process to perform ground filtering using MGF algorithm
// reference http://www.sciencedirect.com/science/article/pii/S0924271608000956
namespace volm_dsm_ground_filter_mgf_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 2;

  bool nearest_ground_elev(vil_image_view<float> const& img,
                           vil_image_view<vxl_byte> const& grd_mask,
                           unsigned const& i, unsigned const& j,
                           float& ground_elev);
  // perform linear region along the scan line and remove mis-labeled ground pixel if their STD is 3 times
  bool ground_linear_regression(vil_image_view<float> const& img,
                                vil_image_view<vxl_byte>& grd_mask,
                                bool const& is_horizontal,
                                float const& neighbor_size = 100,
                                float const& rsm_thres = 0.2);

}

bool volm_dsm_ground_filter_mgf_process_cons(bprb_func_process& pro)
{
  using namespace volm_dsm_ground_filter_mgf_process_globals;
  // this process takes 5 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";   // input DSM image
  input_types_[1] = "float";                      // local window size
  input_types_[2] = "float";                      // elevation threshold
  input_types_[3] = "float";                      // slope threshold in degree
  input_types_[4] = "float";                      // pixel resolution in meters.  default = 1.0 meter
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // filtered ground/non-ground image
  output_types_[1] = "vil_image_view_base_sptr";  // interpolated ground digital elevation model
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_dsm_ground_filter_mgf_process(bprb_func_process& pro)
{
  using namespace volm_dsm_ground_filter_mgf_process_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr dsm_img_res = pro.get_input<vil_image_view_base_sptr>(in_i++);
  auto window_size = pro.get_input<float>(in_i++);
  auto elev_thres = pro.get_input<float>(in_i++);
  auto slop_thres_deg = pro.get_input<float>(in_i++);
  auto pixel_res = pro.get_input<float>(in_i++);

  // compute slope threshold
  float slop_thres = std::tan(slop_thres_deg / vnl_math::deg_per_rad) * pixel_res;
  // load the image
  auto* in_img = dynamic_cast<vil_image_view<float>*>(dsm_img_res.ptr());
  if (!in_img) {
    std::cerr << pro.name() << ": Unsupported image pixel format -- " << dsm_img_res->pixel_format() << ", only float is supported!\n";
    return false;
  }
  unsigned ni = in_img->ni();
  unsigned nj = in_img->nj();
  if (ni < 2 || nj < 2) {
    std::cerr << pro.name() << ": Input image is too small -- ni: " << ni << ", nj: " << nj << "!\n";
    return false;
  }
  // compute slope along 4 scan line direction
  float invalid_slope = -1E3f;
  vil_image_view<float> lr_img(ni, nj);  // slope value scan from left to right
  vil_image_view<float> rl_img(ni, nj);  // slope value scan from right to left
  vil_image_view<float> tb_img(ni, nj);  // slope value scan from top to bottom
  vil_image_view<float> bt_img(ni, nj);  // slope value scan from bottom to top
  lr_img.fill(invalid_slope);
  rl_img.fill(invalid_slope);
  tb_img.fill(invalid_slope);
  bt_img.fill(invalid_slope);

  for (unsigned j = 0; j < nj; j++) {
    for (unsigned i = 1; i < (ni-1); i++) {
      lr_img(i,j) = (*in_img)(i,j) - (*in_img)(i-1,j);
      rl_img(i,j) = (*in_img)(i,j) - (*in_img)(i+1,j);
    }
  }
  for (unsigned j = 0; j < nj; j++) {
    rl_img(0,j) = (*in_img)(0,j) - (*in_img)(1,j);
    lr_img(ni-1, j) = (*in_img)(ni-1,j) - (*in_img)(ni-2,j);
  }
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 1; j < (nj-1); j++) {
      tb_img(i,j) = (*in_img)(i,j) - (*in_img)(i,j-1);
      bt_img(i,j) = (*in_img)(i,j) - (*in_img)(i,j+1);
    }
  }
  for (unsigned i = 0; i < ni; i++) {
    tb_img(i, nj-1) = (*in_img)(i,nj-1) - (*in_img)(i,nj-2);
    bt_img(i, 0) = (*in_img)(i,0) - (*in_img)(i,1);
  }

  auto* grd_mask = new vil_image_view<vxl_byte>(ni, nj);
  grd_mask->fill(0);

  // generate initial ground mask
  auto nw_i = (unsigned)std::floor(ni/window_size + 0.5f);
  auto nw_j = (unsigned)std::floor(nj/window_size + 0.5f);

  // find minimum as ground pixel for each window
  std::map<std::pair<unsigned, unsigned>, float> local_grd_elev;
  vil_image_view<float> grd_tmp(ni, nj);
  grd_tmp.fill(1E6);
  for (unsigned widx_i = 0; widx_i < nw_i; widx_i++) {
    unsigned s_ni, e_ni;
    s_ni = widx_i * window_size;
    e_ni = (widx_i+1) * window_size;
    if (e_ni > ni)
      e_ni = ni;
    for (unsigned widx_j = 0; widx_j < nw_j; widx_j++) {
      unsigned s_nj, e_nj;
      s_nj = widx_j * window_size;
      e_nj = (widx_j+1) * window_size;
      if (e_nj > nj)
        e_nj = nj;
      float min_elev = 1.0e5f;
      unsigned min_i = ni;
      unsigned min_j = nj;
      for (unsigned i = s_ni; i < e_ni; i++) {
        for (unsigned j = s_nj; j < e_nj; j++) {
          if ( (*in_img)(i,j) < min_elev ) {
            min_elev = (*in_img)(i,j);
            min_i = i; min_j = j;
          }
        }
      }
      (*grd_mask)(min_i, min_j) = 127;  // ground
      local_grd_elev.insert(std::pair<std::pair<unsigned, unsigned>, float>(std::pair<unsigned, unsigned>(widx_i, widx_j), min_elev));
      for (unsigned i = s_ni; i < e_ni; i++)
        for (unsigned j = s_nj; j < e_nj; j++)
          grd_tmp(i,j) = min_elev;
    }
  }

  std::cout << "Start MGF ground filtering with elevation threshold: " << elev_thres
           << ", slope threshold: " << slop_thres << "(" << slop_thres_deg << " degree), local neighbor size: "
           << window_size << std::endl;
  std::cout << "  scan from left to right..." << std::flush << std::endl;

  // start left to right scan
  for (unsigned j = 0; j < nj; j++)
  {
    for (unsigned i = 0; i < ni; i++) {
      float elev = (*in_img)(i, j);
      float elev_diff = elev - grd_tmp(i,j);
      if (elev_diff > elev_thres) {
        (*grd_mask)(i,j) = 255;  // non-ground
        continue;
      }
      // check slope value from left to right
      float lr_slope = lr_img(i,j);
      if (lr_slope < invalid_slope+1)
        continue;
      if (lr_slope > slop_thres) {
        (*grd_mask)(i,j) = 255;  // non-ground
      }
      else if (lr_slope <= slop_thres && lr_slope >= 0) {
        (*grd_mask)(i,j) = (*grd_mask)(i-1,j);  // copy status of previous points
      }
      else {
        // find the nearest ground plane height
        float grd_elev;
        if (!nearest_ground_elev(*in_img, *grd_mask, i, j, grd_elev)) {
          std::cerr << pro.name() << ": search nearest ground height value failed for pixel (" << i << ',' << j << ") as scanning from left to right!\n";
          return false;
        }
        float elev_diff_nearest = elev - grd_elev;
        if (elev_diff_nearest > elev_thres)
          (*grd_mask)(i,j) = 255;  // non-ground
        else
          (*grd_mask)(i,j) = 127;  // ground
      }
    }
  }

  // start right to left scan
  std::cout << "  scan from right to left..." << std::flush << std::endl;
  for (unsigned j = 0; j < nj; j++)
  {
    for (int i = ni-1; i >= 0; i--) {
      float elev = (*in_img)(i, j);
      float elev_diff = elev - grd_tmp(i,j);
      if (elev_diff > elev_thres) {
        (*grd_mask)(i,j) = 255;  // non-ground
        continue;
      }
      // check slope value from right to left
      float rl_slope = rl_img(i,j);
      if (rl_slope < invalid_slope+1)  // keep previous status
        continue;
      if (rl_slope > slop_thres) {
        (*grd_mask)(i,j) = 255;  // non-ground
      }
      else if (rl_slope >= 0 && rl_slope <= slop_thres) {
        if ((*grd_mask)(i+1,j))
          (*grd_mask)(i,j) = (*grd_mask)(i+1,j);  // copy status of previous points if previous point is not un-certain
      }
      else {
        // find the nearest ground plane height
        float grd_elev;
        if (!nearest_ground_elev(*in_img, *grd_mask, i, j, grd_elev)) {
          std::cerr << pro.name() << ": search nearest ground height value failed for pixel (" << i << ',' << j << ") as scanning from left to right!\n";
          return false;
        }
        float elev_diff_nearest = elev - grd_elev;
        if (elev_diff_nearest > elev_thres)
          (*grd_mask)(i,j) = 255;  // non-ground
        else
          (*grd_mask)(i,j) = 127;  // ground
      }
    }
  }

  // perform linear regression along horizontal scan line
  std::cout << "  perform linear regression along image row..." << std::flush << std::endl;
  unsigned n_size_horizontal = 100;
  if (n_size_horizontal >= ni)
    n_size_horizontal = ni;
  ground_linear_regression(*in_img, *grd_mask, true, n_size_horizontal);

  // start top to bottom scan
  std::cout << "  scan from top to bottom..." << std::flush << std::endl;
  for (unsigned i = 0; i < ni; i++)
  {
    for (unsigned j = 0; j < nj; j++) {
      float elev = (*in_img)(i, j);
      float elev_diff = elev - grd_tmp(i,j);
      if (elev_diff > elev_thres) {
        (*grd_mask)(i,j) = 255;  // non-ground
        continue;
      }
      // check slope value from top to bottom
      float tb_slope = tb_img(i,j);
      if (tb_slope < invalid_slope+1)  // keep previous status
        continue;
      if (tb_slope > slop_thres)  // non-ground (should be building edge)
        (*grd_mask)(i,j) = 255;
      else if (tb_slope >= 0 && tb_slope <= slop_thres) {
        if ((*grd_mask)(i,j-1))
          (*grd_mask)(i,j) = (*grd_mask)(i,j-1);  // copy status of previous point if previous point is not un-certain
      }
      else {
        // find the nearest ground plane height
        float grd_elev;
        if (!nearest_ground_elev(*in_img, *grd_mask, i, j, grd_elev)) {
          std::cerr << pro.name() << ": search nearest ground height value failed for pixel (" << i << ',' << j << ") as scanning from left to right!\n";
          return false;
        }
        float elev_diff_nearest = elev - grd_elev;
        if (elev_diff_nearest > elev_thres)
          (*grd_mask)(i,j) = 255;  // non-ground
        else
          (*grd_mask)(i,j) = 127;  // ground
      }
    }
  }

  // start top to bottom scan
  std::cout << "  scan from bottom to top..." << std::flush << std::endl;
  for (unsigned i = 0; i < ni; i++)
  {
    for (int j = nj-1; j >= 0; j--) {
      float elev = (*in_img)(i, j);
      float elev_diff = elev - grd_tmp(i,j);
      if (elev_diff > elev_thres) {
        (*grd_mask)(i,j) = 255;  // non-ground
        continue;
      }
      // check slope value from top to bottom
      float bt_slope = bt_img(i,j);
      if (bt_slope > slop_thres)  // non-ground (should be building edge)
        (*grd_mask)(i,j) = 255;
      else if (bt_slope >= 0 && bt_slope <= slop_thres) {
        if ((*grd_mask)(i,j+1))
          (*grd_mask)(i,j) = (*grd_mask)(i,j+1);  // copy status of previous point if previous point is not un-certain
      }
      else {
        // find the nearest ground plane height
        float grd_elev;
        if (!nearest_ground_elev(*in_img, *grd_mask, i, j, grd_elev)) {
          std::cerr << pro.name() << ": search nearest ground height value failed for pixel (" << i << ',' << j << ") as scanning from left to right!\n";
          return false;
        }
        float elev_diff_nearest = elev - grd_elev;
        if (elev_diff_nearest > elev_thres)
          (*grd_mask)(i,j) = 255;  // non-ground
        else
          (*grd_mask)(i,j) = 127;  // ground
      }
    }
  }

  // perform linear regression along vertical scan line
  std::cout << "  perform linear regression along image column..." << std::flush << std::endl;
  unsigned n_size_vertical = 100;
  if (n_size_vertical >= ni)
    n_size_vertical = ni;
  ground_linear_regression(*in_img, *grd_mask, false, n_size_vertical);

  std::cout << "Start to generate ground DEM tile from filtering..." << std::flush << std::endl;
  // generate ground image from ground mask -- tale minimum elevation points as sample points every 10 pixels
  vil_image_view<float> sample_img(ni, nj);
  vil_image_view<vxl_byte> sample_grd(ni, nj);
  sample_img.fill(0.0f);
  sample_grd.fill(0);
  int sample_size = (int)std::floor(10.0/pixel_res+0.5);
  for (int r = sample_size; r < (int)(ni-sample_size); r += sample_size) {
    for (int c = sample_size; c < (int)(nj-sample_size); c += sample_size) {
      // find the minimum elevation among ground pixels
      float min = 1E7f;
      int pt_i, pt_j;
      bool found = false;
      for (int k = -sample_size; k < sample_size; k++) {
        for (int m = -sample_size; m < sample_size; m++) {
          int i = r + k;
          int j = c + m;
          if (i < 0 || j < 0 || i >= (int)ni || j >= (int)nj)
            continue;
          if ( (*grd_mask)(i,j) != 127)
            continue;
          if ( (*in_img)(i,j) < min) {
            pt_i = i;  pt_j = j;
            min = (*in_img)(i,j);
            found = true;
          }
        }
      }
      if (found) {
        sample_grd(r, c) = 127;
        sample_img(r, c) = min;
      }
    }
  }

  // perform linear regression again to remove any specious ground pixels
  ground_linear_regression(sample_img, sample_grd, true,  ni, 0.5);
  ground_linear_regression(sample_img, sample_grd, false, nj, 0.5);

  std::vector<vgl_point_3d<double> > src_pts, dst_pts;
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      if (sample_grd(i,j) == 127) {
        vgl_point_3d<double> src_pt(i, j, (*in_img)(i,j));
        vgl_point_3d<double> dst_pt(i, j, sample_img(i,j));
        src_pts.push_back(src_pt);
        dst_pts.push_back(dst_pt);
      }
  unsigned grd_pt_cnt = 0;
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      if ( (*grd_mask)(i,j) == 127 )
        grd_pt_cnt += 1;
  std::cout << "  Total image pixels: " << ni*nj << ", number of ground pixels: " << grd_pt_cnt
            << ", number of sampled ground pixels: " << src_pts.size() << std::endl;

  // construct spline object
  mbl_thin_plate_spline_3d tps;
  tps.build(src_pts, dst_pts);

  // apply to other points
  auto* grd_img = new vil_image_view<float>(ni, nj);
  grd_img->fill(-1.0f);
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      vgl_point_3d<double> p(i,j,(*in_img)(i,j));
      vgl_point_3d<double> new_p = tps(p);
      (*grd_img)(i,j) = new_p.z();
    }
  }

  // output
  pro.set_output_val<vil_image_view_base_sptr>(0, grd_mask);
  pro.set_output_val<vil_image_view_base_sptr>(1, grd_img);
  return true;
}

bool volm_dsm_ground_filter_mgf_process_globals::nearest_ground_elev(vil_image_view<float> const& img,
                                                                     vil_image_view<vxl_byte> const& grd_mask,
                                                                     unsigned const& i, unsigned const& j,
                                                                     float& ground_elev)
{
  if (img.ni() != grd_mask.ni() || img.nj() != grd_mask.nj())
    return false;

  int ni = static_cast<int>(img.ni());
  int nj = static_cast<int>(img.nj());
  int search_range = ni;
  if (search_range > nj)
    search_range = nj;

  bool found = false;
  for (int radius = 1; (radius < search_range); radius++)
  {
    int start_i = i - radius;  int end_i = i + radius;
    int start_j = j - radius;  int end_j = j + radius;
    for (int ii = start_i; (ii < end_i && !found); ii++) {
      for (int jj = start_j; (jj < end_j && !found); jj++) {
        if (ii < 0 || jj < 0 || ii >= ni || jj >= nj)
          continue;
        if ( grd_mask(ii,jj) == 127) {
          found = true;
          ground_elev = img(ii,jj);
          return found;
        }
      }
    }
  }
  return found;
}

bool volm_dsm_ground_filter_mgf_process_globals::ground_linear_regression(vil_image_view<float> const& img,
                                                                          vil_image_view<vxl_byte>& grd_mask,
                                                                          bool const& is_horizontal,
                                                                          float const& neighbor_size,
                                                                          float const& rsm_thres)
{
  unsigned ni = img.ni();
  unsigned nj = img.nj();
  if (ni != grd_mask.ni() || nj != grd_mask.nj())
    return false;
  unsigned nw;
  unsigned num_lines;
  unsigned num_pts;
  if (is_horizontal) {
    num_lines = nj;
    num_pts = ni;
    nw = (unsigned)std::floor(ni/neighbor_size + 0.5);
  }
  else {
    num_lines = ni;
    num_pts = nj;
    nw = (unsigned)std::floor(nj/neighbor_size + 0.5);
  }
  for (unsigned j = 0; j < num_lines; j++)
  {
    if (j == 250)
      unsigned stop_cnt = 1;
    for (unsigned widx = 0; widx < nw; widx++) {
      unsigned s_pt, e_pt;
      s_pt = widx*neighbor_size; e_pt = (widx+1)*neighbor_size;
      if (e_pt > num_pts) e_pt = num_pts;
      // fit the line segment
      vgl_line_2d_regression<float> reg;
      std::vector<unsigned> grd_pts;
      std::vector<float> grd_elevs;
      for (unsigned i = s_pt; i < e_pt; i++) {
        if (is_horizontal) {
          if (grd_mask(i,j) == 127) {
            reg.increment_partial_sums(i, img(i,j));
            grd_pts.push_back(i);
            grd_elevs.push_back(img(i,j));
          }
        } else {
          if (grd_mask(j,i) == 127) {
            reg.increment_partial_sums(i, img(j,i));
            grd_pts.push_back(i);
            grd_elevs.push_back(img(j,i));
          }
        }
      }
      reg.fit();
      vgl_line_2d<float> line = reg.get_line();
      double rms = reg.get_rms_error();
      std::vector<unsigned> after_grd_pts;
      std::vector<float> after_gre_elev;
      // remove points that have more than three times of the standard deviation of the regression
      for (unsigned int i : grd_pts) {
        float elev;
        if (is_horizontal) elev = img(i,j);
        else               elev = img(j,i);
        float elev_est = (-line.a()*i-line.c()) / line.b();
        float elev_diff = elev-elev_est;
        if ((elev-elev_est) >= rsm_thres*rms) {
          if (is_horizontal) grd_mask(i,j) = 255;
          else               grd_mask(j,i) = 255;
        }
      }
    } // end of loop along current line
  } // end of all scan lines

  return true;
}
