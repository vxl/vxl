// This is brl/bpro/core/vpgl_pro/processes/vpgl_find_connected_component_process.cxx
#include <iostream>
#include <vil/vil_config.h>
#if HAS_GEOTIFF
//:
// \file
//    find the connected component of a geotiff image and visualize it in KML
#include <bprb/bprb_func_process.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vil/algo/vil_find_4con_boundary.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <bkml/bkml_write.h>

namespace vpgl_find_connected_component_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 2;
}

bool vpgl_find_connected_component_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_find_connected_component_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  std::vector<std::string> output_types_(n_outputs_);
  input_types_[0] = "vil_image_view_base_sptr";    // input image
  input_types_[1] = "vpgl_camera_double_sptr";     // input geo camera
  input_types_[2] = "float";                       // threshold
  input_types_[3] = "vcl_string";                  // output kml file
  input_types_[4] = "bool";                        // option to select is above threshold or below

  output_types_[0] = "vil_image_view_base_sptr";   // output connected component image
  output_types_[1] = "unsigned";                   // number of connected component

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool vpgl_find_connected_component_process(bprb_func_process& pro)
{
  using namespace vpgl_find_connected_component_process_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  auto threshold = pro.get_input<float>(in_i++);
  std::string out_kml = pro.get_input<std::string>(in_i++);
  bool is_above = pro.get_input<bool>(in_i++);

  // convert input image to float
  vil_image_view<float> in_img;
  in_img = *vil_convert_cast(float(), img_sptr);

  // load the geo camera
  vpgl_geo_camera* geocam = nullptr;
  geocam = dynamic_cast<vpgl_geo_camera*> (cam_sptr.ptr());
  if (!geocam) {
    std::cerr << pro.name() << ": load geo camera failed!!\n";
    return false;
  }

  std::ofstream ofs(out_kml.c_str());
  if (!ofs) {
    std::cerr << pro.name() << ": Can not open the given kml file " << out_kml << std::endl;
    return false;
  }
  bkml_write::open_document(ofs);

  // collect pixels that larger than the given threshold
  std::vector<vgl_point_2d<int> > pixels;
  for (int i = 0; i < in_img.ni(); i++) {
    for (int j = 0; j < in_img.nj(); j++) {
      if (is_above) {
        if (in_img(i,j) >= threshold)
          pixels.emplace_back(i,j);
      }
      else {
        if (in_img(i,j) <= threshold)
          pixels.emplace_back(i,j);
      }
    }
  }

  std::vector<vgl_polygon<double> > poly_region;
  auto num_pixels = (unsigned)pixels.size();
  for (unsigned id = 0; id < num_pixels; id++)
  {
    bool is_contain = false;
    auto u = (double)pixels[id].x();
    auto v = (double)pixels[id].y();
    for (unsigned sh_idx = 0; (!is_contain && sh_idx < poly_region.size()); sh_idx++)
      is_contain = poly_region[sh_idx].contains(u, v);
    if (is_contain)
      continue;
    // find the boundary
    std::vector<int> bi, bj;
    if (is_above)
      vil_find_4con_boundary_above_threshold(bi, bj, in_img, float(threshold), u, v);
    else
      vil_find_4con_boundary_below_threshold(bi, bj, in_img, float(threshold), u, v);
    if (bi.size() <= 2)
      continue;
    vgl_polygon<double> poly;
    poly.new_sheet();
    for (unsigned i = 0; i < bi.size(); i++)
      if (!poly.contains((double)bi[i], (double)bj[i]))
        poly.push_back((double)bi[i], (double)bj[i]);
    // check formed polygon geometry
    if (vgl_area(poly) < 1E-3)
      continue;
    poly_region.push_back(poly);
  }

  // generate the output image
  auto* out_img = new vil_image_view<vxl_byte>(in_img.ni(), in_img.nj());
  out_img->fill(0);
  unsigned num_poly = poly_region.size();
  std::vector<unsigned> poly_num_pixels;
  std::vector<float>    poly_avg_height;
  for (unsigned sh_idx = 0; sh_idx < num_poly; sh_idx++) {
    vgl_polygon_scan_iterator<double> it(poly_region[sh_idx], true);
    unsigned n_pixel = 0;
    float total_h = 0.0f;
    for (it.reset(); it.next();  ) {
      int y = it.scany();
      for (int x = it.startx(); x <= it.endx(); ++x) {
        if (x >= 0 && y >= 0 && x < out_img->ni() && y < out_img->nj()) {
          (*out_img)(x,y) = 255;
          n_pixel++;
          total_h += in_img(x, y);
        }
      }
    }
    float avg_height = total_h / n_pixel;
    poly_num_pixels.push_back(n_pixel);
    poly_avg_height.push_back(avg_height);
  }

  // generate output kml
  unsigned num_regions = poly_region.size();
  for (unsigned sh_idx = 0; sh_idx < num_regions; sh_idx++)
  {
    vgl_polygon<double> poly = poly_region[sh_idx];
    unsigned num_pts = poly.num_vertices();
    vgl_polygon<double> wgs_poly;
    wgs_poly.new_sheet();
    for (unsigned pidx = 0; pidx < num_pts; pidx++) {
      // convert image to wgs
      double u = poly[0][pidx].x();
      double v = poly[0][pidx].y();
      double lon, lat;
      geocam->img_to_global(u, v, lon, lat);
      wgs_poly.push_back(lon, lat);
    }
    std::stringstream name, description;
    name << "zone_" << sh_idx << "_" << threshold;
    description << "NumPixel_" << poly_num_pixels[sh_idx] << "_AvgHeight_" << poly_avg_height[sh_idx];
    bkml_write::write_polygon(ofs, wgs_poly, name.str(), description.str(), 1.0, 3.0, 0.8, 0, 255, 0);
  }
  bkml_write::close_document(ofs);
  ofs.close();

  // output
  pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_img));
  pro.set_output_val<unsigned>(1, poly_region.size());
  return true;
}

#endif // HAS_GEOTIFF
