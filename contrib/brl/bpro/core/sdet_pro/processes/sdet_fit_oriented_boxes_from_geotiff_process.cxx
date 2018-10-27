// This is brl/bpro/core/sdet_pro/processes/sdet_fit_oriented_boxes_from_geotiff_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file  Given a geotiff image along with its geo-camera, fit oritented box for detected regions inside the image and convert them into kml polygons

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vgl_polygon.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <volm_candidate_list.h>
#include <bkml_write.h>
#include <vnl/vnl_random.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <sdet/sdet_region.h>

namespace sdet_fit_oriented_boxes_from_geotiff_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 2;
  unsigned polygon_size(vgl_polygon<int> const& poly);
}

// count the number of poixels that are covered by a polygon using polygon iterator, including the polygon boundary pixels
unsigned sdet_fit_oriented_boxes_from_geotiff_process_globals::polygon_size(vgl_polygon<int> const& poly)
{
  unsigned n_pixels;
  vgl_polygon_scan_iterator<int> psi(poly, false);
  for (psi.reset(); psi.next(); ) {
    int y = psi.scany();
    for (int x = psi.startx(); x <= psi.endx(); ++x)
      n_pixels++;
  }
  // add the outline of polygon
  n_pixels += poly.num_vertices();
  return n_pixels;
}

//: constructor
bool sdet_fit_oriented_boxes_from_geotiff_process_cons(bprb_func_process& pro)
{
  using namespace sdet_fit_oriented_boxes_from_geotiff_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";   // input binary image
  input_types_[1] = "vpgl_camera_double_sptr";    // input geo camera
  input_types_[2] = "unsigned";                   // pixel number threshold
  input_types_[3] = "vcl_string";                 // outout kml filename
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "unsigned";                  // number of fitted oriented box
  output_types_[1] = "vil_image_view_base_sptr";  // fitted oriented box image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool sdet_fit_oriented_boxes_from_geotiff_process(bprb_func_process& pro)
{
  using namespace sdet_fit_oriented_boxes_from_geotiff_process_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr in_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  in_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  auto pixel_thres = pro.get_input<unsigned>(in_i++);
  std::string out_kml = pro.get_input<std::string>(in_i++);

  // check inputs
  auto* in_img = dynamic_cast<vil_image_view<vxl_byte>*>(in_img_sptr.ptr());
  if (!in_img) {
    std::cerr << pro.name() << ": Unsupported input image pixel type: " << in_img_sptr->pixel_format() << ", only Byte is supported!\n";
    return false;
  }
  auto* cam = dynamic_cast<vpgl_geo_camera*>(in_cam_sptr.ptr());
  if (!cam) {
    std::cerr << pro.name() << ": failed to load input geo-camera!\n";
    return false;
  }

  // perform region extraction from image
  volm_candidate_list cand_list(*in_img, 127);
  vgl_polygon<int> img_polys = cand_list.cand_poly();
  unsigned n_polys = img_polys.num_sheets();
  // clean polygons by their size and construct convex hull polygon
  std::vector<vgl_polygon<float> > valid_polys;
  for (unsigned i = 0; i < n_polys; i++) {
    vgl_polygon<int> single_poly(img_polys[i], 1);
    if (polygon_size(single_poly) < pixel_thres)
      continue;
    // construct convex hull
    std::vector<vgl_point_2d<float> > pts;
    for (unsigned idx = 0; idx < single_poly.num_vertices(); idx++)
      pts.emplace_back(single_poly[0][idx].x(), single_poly[0][idx].y());
    vgl_convex_hull_2d<float> ch(pts);
    vgl_polygon<float> c_poly = ch.hull();
    single_poly.print(std::cerr);
    std::cerr << "single polygon size: "  << polygon_size(single_poly) << ", convex hull: " << c_poly.num_vertices() << std::endl;
    valid_polys.push_back(c_poly);
  }
  n_polys = valid_polys.size();
  unsigned ni = in_img->ni();
  unsigned nj = in_img->nj();

#if 0
  // fit oriented boxes
  std::vector<vgl_oriented_box_2d<float> > orient_boxes;
  for (unsigned i = 0; i < n_polys; i++)
  {
    vgl_polygon_scan_iterator<float> psi(valid_polys[i], false);
    std::vector<vgl_point_2d<float> > v;
    for (psi.reset(); psi.next(); ) {
      int y = psi.scany();
      for (int x = psi.startx(); x <= psi.endx(); ++x) {
        if ( x < 0 || y < 0 || x >= ni || y >= nj)
          continue;
        v.push_back(vgl_point_2d<float>(x, y));
      }
    }
    float* xp = new float[v.size()];
    float* yp = new float[v.size()];
    unsigned short* Ip = new unsigned short[v.size()];  // supposed to be grey values for the blob but here we don't care so just fill with same color
    for (unsigned vidx = 0; vidx < v.size(); vidx++) {
      xp[i] = v[i].x();
      yp[i] = v[i].y();
      Ip[i] = 100;
    }
    sdet_region sr(v.size(), xp, yp, Ip);
    vgl_oriented_box_2d<float> box = sr.obox();
    orient_boxes.push_back(box);

    delete [] xp;
    delete [] yp;
    delete [] Ip;
  }

  // convert image domain to geographic domain
  unsigned n_box = orient_boxes.size();
  std::vector<vgl_polygon<double> > geo_polys;
  for (unsigned i = 0; i < n_box; i++)
  {
    std::vector<vgl_point_2d<float> > corners = orient_boxes[i].corners();
    vgl_polygon<double> single_poly;
    single_poly.new_sheet();
    for (std::vector<vgl_point_2d<float> >::iterator vit = corners.begin(); vit != corners.end(); ++vit) {
      double lon, lat;
      cam->img_to_global(vit->x(), vit->y(), lon, lat);
      single_poly.push_back(lon, lat);
    }
    geo_polys.push_back(single_poly);
  }


  // write to kml
  std::ofstream ofs(out_kml.c_str());
  if (!ofs.is_open()) {
    std::cerr << pro.name() << ": Faild to write kml -- " << out_kml << "!\n";
    return false;
  }
  bkml_write::open_document(ofs);
  vgl_polygon<double> bbox;
  bbox.new_sheet();
  double lon, lat;
  cam->img_to_global(0, 0, lon, lat);
  bbox.push_back(lon, lat);
  cam->img_to_global(ni, 0, lon, lat);
  bbox.push_back(lon, lat);
  cam->img_to_global(ni, nj, lon, lat);
  bbox.push_back(lon, lat);
  cam->img_to_global(0, nj, lon, lat);
  bbox.push_back(lon, lat);
  bkml_write::write_polygon(ofs, bbox, "", "", 1.0, 3.0, 0.0, 255, 255, 255);

  for (unsigned i = 0; i < geo_polys.size(); i++) {
    std::stringstream name;
    name << "polygon_" << i;
    bkml_write::write_polygon(ofs, geo_polys[i], name.str(), name.str(), 1.0, 3.0, 0.45, 0, 0, 255);
  }
  bkml_write::close_document(ofs);
  ofs.close();

  // generate an output image
  vil_image_view<vil_rgb<vxl_byte> > out_rgb(ni, nj);
  out_rgb.fill(vil_rgb<vxl_byte>(0, 0, 0));
  vnl_random rng;
  for (unsigned i = 0; i < n_box; i++)
  {
    std::vector<vgl_point_2d<float> > corners = orient_boxes[i].corners();
    vgl_polygon<float> single_poly(corners, 1);
    vgl_polygon_scan_iterator<float> psi(single_poly, false);
    vil_rgb<vxl_byte> random_color = vil_rgb<vxl_byte>((char)rng.lrand32(0,255), (char)rng.lrand32(0,255), (char)rng.lrand32(0,255));
    for (psi.reset(); psi.next(); ) {
      int y = psi.scany();
      for (int x = psi.startx(); x <= psi.endx(); ++x) {
        if ( x < 0 || y < 0 ||  x >= ni || y >= nj)
          continue;
        out_rgb(x, y) = random_color;
      }
    }
  }
#endif

#if 1
  // convert image domain to geographic domain
  std::vector<vgl_polygon<double> > geo_polys;
  for (unsigned i = 0; i < n_polys; i++)
  {
    std::vector<vgl_point_2d<float> > corners = valid_polys[i][0];
    vgl_polygon<double> single_poly;
    single_poly.new_sheet();
    for (auto & corner : corners) {
      double lon, lat;
      cam->img_to_global(corner.x(), corner.y(), lon, lat);
      single_poly.push_back(lon, lat);
    }
    geo_polys.push_back(single_poly);
  }
  // write to kml
  std::ofstream ofs(out_kml.c_str());
  if (!ofs.is_open()) {
    std::cerr << pro.name() << ": Faild to write kml -- " << out_kml << "!\n";
    return false;
  }
  bkml_write::open_document(ofs);
  vgl_polygon<double> bbox;
  bbox.new_sheet();
  double lon, lat;
  cam->img_to_global(0, 0, lon, lat);
  bbox.push_back(lon, lat);
  cam->img_to_global(ni, 0, lon, lat);
  bbox.push_back(lon, lat);
  cam->img_to_global(ni, nj, lon, lat);
  bbox.push_back(lon, lat);
  cam->img_to_global(0, nj, lon, lat);
  bbox.push_back(lon, lat);
  bkml_write::write_polygon(ofs, bbox, "", "", 1.0, 3.0, 0.0, 255, 255, 255);

  for (unsigned i = 0; i < geo_polys.size(); i++) {
    std::stringstream name;
    name << "polygon_" << i;
    bkml_write::write_polygon(ofs, geo_polys[i], name.str(), name.str(), 1.0, 3.0, 0.45, 0, 0, 255);
  }
  bkml_write::close_document(ofs);
  ofs.close();

  vil_image_view<vil_rgb<vxl_byte> > out_rgb(ni, nj);
  out_rgb.fill(vil_rgb<vxl_byte>(0, 0, 0));
  vnl_random rng;
  for (unsigned i = 0; i < n_polys; i++)
  {
    vgl_polygon_scan_iterator<float> psi(valid_polys[i], false);
    vil_rgb<vxl_byte> random_color = vil_rgb<vxl_byte>((char)rng.lrand32(0,255), (char)rng.lrand32(0,255), (char)rng.lrand32(0,255));
    for (psi.reset(); psi.next(); ) {
      int y = psi.scany();
      for (int x = psi.startx(); x <= psi.endx(); ++x) {
        if ( x < 0 || y < 0 ||  x >= ni || y >= nj)
          continue;
        out_rgb(x, y) = random_color;
      }
    }
  }
#endif

  unsigned out_i = 0;
  pro.set_output_val<unsigned>(out_i, n_polys);
  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<vil_rgb<vxl_byte> >(out_rgb));

  return true;
}
