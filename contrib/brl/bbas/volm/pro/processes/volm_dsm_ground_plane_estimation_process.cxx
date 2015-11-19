// This is contrib/brl/bbas/volm/pro/process/volm_dsm_ground_plane_estimation_process.cxx
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
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_3d.h>

namespace volm_dsm_ground_plane_estimation_process_globals
{
  const unsigned int n_inputs_ = 4;
  const unsigned int n_outputs_ = 1;
}

bool volm_dsm_ground_plane_estimation_process_cons(bprb_func_process& pro)
{
  using namespace volm_dsm_ground_plane_estimation_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";       // original height map image
  input_types_[1] = "int";                            // number of height value estimations to use, bottom N, e.g. 100
  input_types_[2] = "int";                            // window size to get minimum height value around the fitting points.
  input_types_[3] = "float";                          // Invalid pixel value
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr"; // output ground plane image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_dsm_ground_plane_estimation_process(bprb_func_process& pro)
{
  using namespace volm_dsm_ground_plane_estimation_process_globals;
  if (!pro.verify_inputs())  {
    vcl_cerr << pro.name() << ": Wring inputs!!!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr img_res = pro.get_input<vil_image_view_base_sptr>(in_i++);
  int N = pro.get_input<int>(in_i++);
  int window_size = pro.get_input<int>(in_i++);
  float invalid_pixel = pro.get_input<float>(in_i++);
  
  vil_image_view<float>* in_img = dynamic_cast<vil_image_view<float>*>(img_res.ptr());
  if (!in_img) {
    vcl_cerr << pro.name() << ": Unsupported image pixel format -- " << img_res->pixel_format() << ", only float is supported!\n";
    return false;
  }
  unsigned ni = in_img->ni();
  unsigned nj = in_img->nj();
  vil_image_view<float>* out = new vil_image_view<float>(ni, nj);

  vcl_vector<vgl_point_3d<double> > src_pts, dst_pts;

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
          if ( vcl_abs((*in_img)(ii,jj)-invalid_pixel) < 1E-3 )
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
          if ( vcl_abs((*in_img)(ii,jj)-invalid_pixel) < 1E-3 )
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
          if ( vcl_abs((*in_img)(uu,vv)-invalid_pixel) < 1E-3 )
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

  vcl_cout << pro.name() << ": Total image pixels: " << ni*nj << ", size of source pts: " << src_pts.size() << vcl_endl;
  //for (unsigned i = 0; i < src_pts.size(); i++) {
  //  vcl_cout << "source: " << src_pts[i].x() << " " << src_pts[i].y() << " " << src_pts[i].z() 
  //           << "  dest: " << dst_pts[i].x() << " " << dst_pts[i].y() << " " << dst_pts[i].z() << vcl_endl;
  //}

  // construct spline object
  mbl_thin_plate_spline_3d tps;
  tps.build(src_pts, dst_pts);
  
  // apply to points
  out->fill(invalid_pixel);
  for (int i = 0; i < ni; i++) {
    for (int j = 0; j < nj; j++) {
      if (vcl_abs((*in_img)(i,j)-invalid_pixel) < 1E-3 )
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
  const unsigned int n_inputs_ = 4;
  const unsigned int n_outputs_ = 1;

  int window_size_from_nearest_edge(vil_image_view<vxl_byte>* edge_img, int const& i, int const& j, int const& search_range);
}

bool volm_dsm_ground_plane_estimation_edge_process_cons(bprb_func_process& pro)
{
  using namespace volm_dsm_ground_plane_estimation_edge_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";       // original height map image
  input_types_[1] = "vil_image_view_base_sptr";       // edge image used to obtain the search window size
  input_types_[2] = "int";                            // number of height value estimations to use, bottom N, e.g. 100
  input_types_[3] = "float";                          // invalid pixel value
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr"; // output ground plane image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_dsm_ground_plane_estimation_edge_process(bprb_func_process& pro)
{
  using namespace volm_dsm_ground_plane_estimation_edge_process_globals;
  if (!pro.verify_inputs())  {
    vcl_cerr << pro.name() << ": Wring inputs!!!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr img_res = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr edge_img_res = pro.get_input<vil_image_view_base_sptr>(in_i++);
  int N = pro.get_input<int>(in_i++);
  float invalid_pixel = pro.get_input<float>(in_i++);

  vil_image_view<float>* in_img = dynamic_cast<vil_image_view<float>*>(img_res.ptr());
  if (!in_img) {
    vcl_cerr << pro.name() << ": Unsupported image pixel format -- " << img_res->pixel_format() << ", only float is supported!\n";
    return false;
  }
  vil_image_view<vxl_byte>* edge_img = dynamic_cast<vil_image_view<vxl_byte>*>(edge_img_res.ptr());
  if (!edge_img) {
    vcl_cerr << pro.name() << ": Unsupported edge image pixel format -- " << edge_img_res->pixel_format() << ", only byte is supported!\n";
    return false;
  }
  unsigned ni = in_img->ni();
  unsigned nj = in_img->nj();
  if (ni != edge_img->ni() || nj != edge_img->nj()) {
    vcl_cerr << pro.name() << ": height image and edge image size do not match!\n";
    return false;
  }
  vil_image_view<float>* out = new vil_image_view<float>(ni, nj);
  int search_range = ni;
  if (search_range > nj)
    search_range = nj;
  vcl_vector<vgl_point_3d<double> > src_pts, dst_pts;
  for (int i = N; i < ni-N; i+=N)
  {
    for (int j = N; j < nj-N; j+=N)
    {
      // find appropriate window size for current pixel
      if ( i == 140 && j == 180)
        int tmp = 1;
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
          if ( vcl_abs((*in_img)(ii,jj)-invalid_pixel) < 1E-3 )
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
  vcl_cout << pro.name() << ": Total image pixels: " << ni*nj << ", size of source pts: " << src_pts.size() << vcl_endl;
  //for (unsigned i = 0; i < src_pts.size(); i++) {
  //  vcl_cout << "source: " << src_pts[i].x() << " " << src_pts[i].y() << " " << src_pts[i].z() 
  //           << "  dest: " << dst_pts[i].x() << " " << dst_pts[i].y() << " " << dst_pts[i].z() << vcl_endl;
  //}
  // construct spline object
  mbl_thin_plate_spline_3d tps;
  tps.build(src_pts, dst_pts);
  
  // apply to points
  out->fill(invalid_pixel);
  for (int i = 0; i < ni; i++) {
    for (int j = 0; j < nj; j++) {
      if (vcl_abs((*in_img)(i,j)-invalid_pixel) < 1E-3 )
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
        if (ii < 0 || jj < 0 || ii >= ni || jj >= nj)
          continue;
        if ( (*edge_img)(ii, jj) != 0) {
          found_neighbor = true;
          edge_window_size = radius;
        }
      }
    }
  }
  if (!edge_window_size)
    return 120;
  edge_window_size += 120;
  return edge_window_size;
}