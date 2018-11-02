// This is contrib/brl/bbas/volm/pro/process/volm_ndsm_generation_process.cxx
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <complex>
#include <bprb/bprb_func_process.h>
#include <vgl/vgl_area.h>
//:
// \file
//     process to generate a normalized height image for given input land cover image from series of height image tiles
//     The input land cover image will have various land categories along with associated IDs and the specified ID is used
//     to define the ground height.  The output height map shall have same resolution as input height map tiles
//     Note that all input height image tiles are geotiff under WGS84 and the output image shall have same GSD as input height image tiles
//
// \verbatim
//  Modifications
//    Yi Dong Nov, 2015.  Modify the inputs to avoid usage of land cover image and land cover camera
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_resource.h>
#include <vgl/vgl_intersection.h>
#include <bvgl/algo/bvgl_2d_geo_index.h>
#include <bvgl/algo/bvgl_2d_geo_index_sptr.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <volm/volm_io_tools.h>
#include <mbl/mbl_thin_plate_spline_3d.h>

namespace volm_ndsm_generation_process_globals
{
  constexpr unsigned n_inputs_ = 11;
  constexpr unsigned n_outputs_ = 4;
}

bool volm_ndsm_generation_process_cons(bprb_func_process& pro)
{
  using namespace volm_ndsm_generation_process_globals;
  // this process takes 6 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "double";                   // lower left lon of the region
  input_types_[1] = "double";                   // lower left lat of the region
  input_types_[2] = "double";                   // upper right lon of the region
  input_types_[3] = "double";                   // upper right lat of the region
  input_types_[4] = "unsigned";                 // desired image size along latitude
  input_types_[5] = "unsigned";                 // desired image size along longitude
  input_types_[6] = "vcl_string";               // geo index for height maps
  input_types_[7] = "vcl_string";               // folder of height map tiles
  input_types_[8] = "vcl_string";               // folder of ground images
  input_types_[9] = "unsigned";                 // window size
  input_types_[10] = "float";                    // maximum height limit
  // this process takes 2 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // normalized height image (byte image)
  output_types_[1] = "vil_image_view_base_sptr";  // original height image (float image)
  output_types_[2] = "vil_image_view_base_sptr";  // ground plane image (float image)
  output_types_[3] = "vpgl_camera_double_sptr";   // geo camera of the output image

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_ndsm_generation_process(bprb_func_process& pro)
{
  using namespace volm_ndsm_generation_process_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get the input
  unsigned in_i = 0;
  auto                    ll_lon = pro.get_input<double>(in_i++);
  auto                    ll_lat = pro.get_input<double>(in_i++);
  auto                    ur_lon = pro.get_input<double>(in_i++);
  auto                    ur_lat = pro.get_input<double>(in_i++);
  auto                    o_ni = pro.get_input<unsigned>(in_i++);
  auto                    o_nj = pro.get_input<unsigned>(in_i++);
  std::string        geo_index_txt = pro.get_input<std::string>(in_i++);
  std::string         h_map_folder = pro.get_input<std::string>(in_i++);
  std::string           grd_folder = pro.get_input<std::string>(in_i++);
  auto             window_size = pro.get_input<unsigned>(in_i++);
  auto                h_max_limit = pro.get_input<float>(in_i++);

  if (!vul_file::exists(geo_index_txt)) {
    std::cerr << pro.name() << ": can not find geo index file " << geo_index_txt << "!\n";
    return false;
  }

  // compute the output image resolution based on given region size and image size
  double scale_x =  (ur_lon - ll_lon)/o_ni;
  double scale_y = -(ur_lat - ll_lat)/o_nj;
  vnl_matrix<double> trans_matrix(4,4,0.0);
  trans_matrix[0][0] = scale_x;
  trans_matrix[1][1] = scale_y;
  trans_matrix[0][3] = ll_lon;
  trans_matrix[1][3] = ur_lat;
  vpgl_lvcs_sptr lvcs_dummy = new vpgl_lvcs;
  vpgl_geo_camera* out_cam = new vpgl_geo_camera(trans_matrix, lvcs_dummy);
  out_cam->set_scale_format(true);
  std::cerr << "land map region -- lower_left: " << ll_lon << ',' << ll_lat << ", upper_right: " << ur_lon << ',' << ur_lat << std::endl;
  std::cout << "output image size is ni: " << o_ni << ", nj: " << o_nj << std::endl;
  auto* out_dsm = new vil_image_view<float>(o_ni, o_nj);
  auto* out_ndsm = new vil_image_view<vxl_byte>(o_ni, o_nj);
  auto* grd_img = new vil_image_view<float>(o_ni, o_nj);
  out_dsm->fill(-1.0);
  out_ndsm->fill(255);
  grd_img->fill(-1.0f);

  // obtain the height map image id that overlap with land cover image region
  vgl_box_2d<double> l_bbox(ll_lon, ur_lon, ll_lat, ur_lat);
  double min_size;
  bvgl_2d_geo_index_node_sptr root = bvgl_2d_geo_index::read_and_construct<float>(geo_index_txt, min_size);
  std::vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(root, leaves);
  // get the ids of intersected leaves
  std::vector<unsigned> leaf_ids;
  for (unsigned i = 0; i < leaves.size(); i++) {
    if (vgl_area(vgl_intersection(l_bbox, leaves[i]->extent_)) > 0)
      leaf_ids.push_back(i);
  }

  std::cout << "Number of leaves: " << leaves.size() << std::endl;
  if (leaf_ids.empty()) {
    std::cout << "no height map intersects with land cover image region, return an empty NDSM" << std::endl;
    pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_ndsm));
    pro.set_output_val<vil_image_view_base_sptr>(1, vil_image_view_base_sptr(out_dsm));
    pro.set_output_val<vil_image_view_base_sptr>(2, vil_image_view_base_sptr(grd_img));
    pro.set_output_val<vpgl_camera_double_sptr>(3, out_cam);
    return true;
  }

  std::cout << leaf_ids.size() << " height image tiles intersect with land cover image" << std::endl;

  // load all height maps
  std::vector<volm_img_info> h_infos;
  for (unsigned int leaf_id : leaf_ids)
  {
    std::stringstream img_file_stream;
    img_file_stream << h_map_folder << "/scene_" << leaf_id << "_h_stereo.tif";
    std::string h_img_file = img_file_stream.str();
    if (!vul_file::exists(h_img_file))
      continue;
    volm_img_info info;
    volm_io_tools::load_geotiff_image(h_img_file, info, false);
    h_infos.push_back(info);
  }

  // load all ground images
  std::vector<volm_img_info> grd_infos;
  for (unsigned int leaf_id : leaf_ids)
  {
    std::stringstream img_file_stream;
    img_file_stream << grd_folder << "/scene_" << leaf_id << "_h_stereo_grd.tif";
    std::string h_img_file = img_file_stream.str();
    if (!vul_file::exists(h_img_file))
      continue;
    volm_img_info info;
    volm_io_tools::load_geotiff_image(h_img_file, info, false);
    grd_infos.push_back(info);
  }

  // aggregate the height map
  std::cout << "Start to aggregate height images for land cover image region..." << std::endl;
  for (unsigned i = 0; i < o_ni; i++)
  {
    if (i%1000 == 0)
      std::cout << i << '.' << std::flush;
    for (unsigned j = 0; j < o_nj; j++)
    {
      double lon, lat;
      out_cam->img_to_global(i, j, lon, lat);
      bool found = false;
      for (auto vit = h_infos.begin(); (vit != h_infos.end() && !found); ++vit) {
        vgl_box_2d<double> bbox = vit->bbox;
        bbox.expand_about_centroid(2E-5);
        if (!bbox.contains(lon, lat))
          continue;
        double u, v;
        vit->cam->global_to_img(lon, lat, 0.0, u, v);
        auto uu = (unsigned)std::floor(u+0.5);
        auto vv = (unsigned)std::floor(v+0.5);
        if (uu < vit->ni && vv < vit->nj) {
          found = true;
          vil_image_view<float> h_img(vit->img_r);
          (*out_dsm)(i,j) = h_img(uu, vv);
        }
      }
    }
  }

  // generate ground image
  std::cout << "\nStart to aggregate ground images for land cover image region..." << std::endl;
  for (unsigned i = 0; i < o_ni; i++)
  {
    if (i%1000 == 0)
      std::cout << i << '.' << std::flush;
    for (unsigned j = 0; j < o_nj; j++)
    {
      double lon, lat;
      out_cam->img_to_global(i, j, lon, lat);
      bool found = false;
      for (auto vit = grd_infos.begin(); (vit != grd_infos.end() && !found); ++vit) {
        vgl_box_2d<double> bbox = vit->bbox;
        bbox.expand_about_centroid(2E-5);
        if (!bbox.contains(lon, lat))
          continue;
        double u, v;
        vit->cam->global_to_img(lon, lat, 0.0, u, v);
        auto uu = (unsigned)std::floor(u+0.5);
        auto vv = (unsigned)std::floor(v+0.5);
        if (uu < vit->ni && vv < vit->nj) {
          found = true;
          vil_image_view<float> g_img(vit->img_r);
          (*grd_img)(i,j) = g_img(uu, vv);
        }
      }
    }
  }
#if 0
  // get ground image from dsm image
  std::cout << "  \nStart to normalize the original DSM" << std::endl;
  std::cout << "image size: " << o_ni << 'x' << o_nj << std::endl;
  std::cout << "sub-pixel size: " << window_size << std::endl;
  std::cout << "  estimate ground plane... " << std::endl;
  vil_image_view<float>* grd_img = new vil_image_view<float>(o_ni, o_nj);
  grd_img->fill(-1.0f);
  std::vector<vgl_point_3d<double> > src_pts, dst_pts;
  int N = (int)(window_size);
  for (int j = N; j < o_nj-N; j+=N)
  {
    for (int i = N; i < o_ni-N; i+=N)
    {
      // find the minimum in the neighborhood as ground plane
      float min = 10000000.0f;
      for (int k = -N; k < N; k++) {
        for (int m = -N; m < N; m++) {
          int uu = i+k;
          int vv = j+m;
          if ((*out_dsm)(uu,vv) < 0)
            continue;
          if ((*out_dsm)(uu,vv) < min)
            min = (*out_dsm)(uu,vv);
        }
      }
      if (min < 10000000.0f) {
        double img_val = (*out_dsm)(i,j);
        if (img_val > 0) {
          vgl_point_3d<double> pt1(i,j,img_val);
          vgl_point_3d<double>  pt(i,j,min);
          src_pts.push_back(pt1);
          dst_pts.push_back(pt);
        }
      }
    }
  }
  std::cout << "\nTotal dsm image pixel: " << o_ni * o_nj << ", size of pts used for ground estimation: " << src_pts.size() << std::endl;
  for (unsigned i = 0; i < src_pts.size(); i++)
    std::cout << "source: " << src_pts[i].x() << " " << src_pts[i].y() << " " << src_pts[i].z()
             << " dest: "  << dst_pts[i].x() << " " << dst_pts[i].y() << " " << dst_pts[i].z() << std::endl;

  // construct the spline object
  mbl_thin_plate_spline_3d tps;
  tps.build(src_pts, dst_pts);

  // apply onto ground image
  std::cout << "\n  generating ground plane image..." << std::endl;
  for (int i = 0; i < o_ni; i++) {
    if (i%1000 == 0)
        std::cout << i << '.' << std::flush;
    for (int j = 0; j < o_nj; j++) {
      if ( (*out_dsm)(i,j) < 0)
        continue;
      vgl_point_3d<double> p(i,j,(*out_dsm)(i,j));
      vgl_point_3d<double> new_p = tps(p);
      (*grd_img)(i,j) = new_p.z();
    }
  }
#endif

  // normalize the image using ground image
  std::cout << "\n  normalize the image..." << std::flush << std::endl;
  for (unsigned i = 0; i < o_ni; i++)
  {
    if (i%1000 == 0)
        std::cout << i << '.' << std::flush;
    for (unsigned j = 0; j < o_nj; j++)
    {
      if ( (*out_dsm)(i, j) < 0 || (*grd_img)(i,j) < 0)
        continue;
      float v = (*out_dsm)(i,j)-(*grd_img)(i,j);
      if (v > 254.0f)
        (*out_ndsm)(i,j) = 254;
      else if (v < 0.0f)
        (*out_ndsm)(i,j) = 0;
      else
        (*out_ndsm)(i,j) = (unsigned char)std::floor(v+0.5f);
    }
  }
  // output
  pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_ndsm));
  pro.set_output_val<vil_image_view_base_sptr>(1, vil_image_view_base_sptr(out_dsm));
  pro.set_output_val<vil_image_view_base_sptr>(2, vil_image_view_base_sptr(grd_img));
  pro.set_output_val<vpgl_camera_double_sptr>(3, out_cam);
  return true;
}
