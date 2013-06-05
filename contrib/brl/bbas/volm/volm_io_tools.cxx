#include "volm_io_tools.h"
//:
// \file


#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpgl/vpgl_utm.h>
#include <bkml/bkml_write.h>
#include <vil/vil_load.h>


unsigned int volm_io_tools::northing = 0;  // WARNING: north hard-coded

void volm_img_info::save_box_kml(vcl_string out_name) { 
    vcl_ofstream ofs(out_name.c_str());
    bkml_write::open_document(ofs);
    bkml_write::write_box(ofs, name, "", bbox);
    bkml_write::close_document(ofs);
  }

bool read_box(vcl_string bbox_file, vgl_box_2d<double>& bbox) {
  char buffer[1000];
  vcl_ifstream ifs(bbox_file.c_str());
  if (!ifs.is_open()) {
    vcl_cerr << " cannot open: " << bbox_file << "!\n";
    return false;
  }

  vcl_string dummy; double top_lat, bottom_lat, left_lon, right_lon;
  for (unsigned kk = 0; kk < 22; kk++)
    ifs.getline(buffer, 1000);
  // top
  ifs.getline(buffer, 1000);
  vcl_stringstream top_edge_line(buffer);
  top_edge_line >> dummy; top_edge_line >> dummy; top_edge_line >> dummy; 
  top_edge_line >> top_lat;
  // bottom
  ifs.getline(buffer, 1000);
  vcl_stringstream bot_edge_line(buffer);
  bot_edge_line >> dummy; bot_edge_line >> dummy; bot_edge_line >> dummy; 
  bot_edge_line >> bottom_lat;
  // left
  ifs.getline(buffer, 1000);
  vcl_stringstream left_edge_line(buffer);
  left_edge_line >> dummy; left_edge_line >> dummy; left_edge_line >> dummy; 
  left_edge_line >> left_lon;
  // right
  ifs.getline(buffer, 1000);
  vcl_stringstream right_edge_line(buffer);
  right_edge_line >> dummy; right_edge_line >> dummy; right_edge_line >> dummy; 
  right_edge_line >> right_lon;

  vgl_point_2d<double> lower_left(left_lon, bottom_lat);
  vgl_point_2d<double> upper_right(right_lon, top_lat);
  bbox = vgl_box_2d<double>(lower_left, upper_right);
  //vcl_cout << "bbox: " << bbox << vcl_endl;
  return true;
}

bool volm_io_tools::load_naip_img(vcl_string const& img_folder, vcl_string const& name, vpgl_lvcs_sptr& lvcs, volm_img_info& info, bool load_resource)
{
  vcl_string filename = img_folder + "\\" + name;
  vcl_string img_name = filename + "\\" + name + ".tif";
  vcl_string tfw_name = filename + "\\" + name + ".tfw";
  if (!vul_file::exists(tfw_name) || !vul_file::exists(img_name)) 
    return false;
    
  info.name = name; info.img_name = img_name;
  vcl_string bbox_file = filename + "\\output_parameters.txt";
  if (!read_box(bbox_file, info.bbox)) {
    vcl_cerr << " cannot find: " << bbox_file << vcl_endl;
    return false;
  }
  //vcl_cout << "NAIP bbox: " << info.bbox << vcl_endl;
  // figure out utm zone
  vpgl_utm utm; int utm_zone, zone_max; double xx, yy;
  utm.transform(info.bbox.min_point().y(), info.bbox.min_point().x(), xx, yy, utm_zone); 
  utm.transform(info.bbox.max_point().y(), info.bbox.max_point().x(), xx, yy, zone_max); 
  if (utm_zone != zone_max) {
    vcl_cout << "!!!!!!!!!!!!!!!!!!!!!!!!!WARNING! img: " << img_name << " has min and max points in different UTM zones, using zone of min point!\n";

  }

  vpgl_geo_camera *cam = 0;
  if (!vpgl_geo_camera::init_geo_camera(tfw_name, lvcs, utm_zone, northing, cam))
    return false;    
  info.cam = cam;

  if (load_resource) {
    vil_image_resource_sptr img = vil_load_image_resource(img_name.c_str());
    vcl_cout << "ni: " << img->ni() <<" nj: " << img->nj() <<vcl_endl;
    info.ni = img->ni(); info.nj = img->nj(); 
  }
  return true;
}

bool volm_io_tools::load_naip_imgs(vcl_string const& img_folder, vcl_vector<volm_img_info>& imgs, bool load_resource) {
 
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs; // just the default, no concept of local coordinate system here, so won't be used

  vcl_string in_dir = img_folder + "*";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    vcl_string filename = fn();
    //vcl_cout << "filename: " << filename << vcl_endl;
    vcl_string file = vul_file::strip_directory(filename);
    
    volm_img_info info;
    if (load_naip_img(img_folder, file, lvcs, info, load_resource))
      imgs.push_back(info);
  }
  return true;
}

int volm_io_tools::load_lidar_img(vcl_string img_file, volm_img_info& info, bool load_image_resource) {
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs; // just the default, no concept of local coordinate system here, so won't be used
  
  if (load_image_resource) {
    info.img_r = vil_load(img_file.c_str());
    info.ni = info.img_r->ni(); info.nj = info.img_r->nj(); 
  } else {
    vil_image_view_base_sptr img_sptr = vil_load(img_file.c_str());
    info.ni = img_sptr->ni(); info.nj = img_sptr->nj(); 
  }

  info.name = vul_file::strip_directory(vul_file::strip_extension(img_file)); 
  info.img_name = img_file;

  vpgl_geo_camera *cam;
  vpgl_geo_camera::init_geo_camera(img_file, info.ni, info.nj, lvcs, cam);
  info.cam = cam; 
    
  double lat, lon;
  cam->img_to_global(0.0, info.nj-1, lon, lat);
  vgl_point_2d<double> lower_left(-lon, lat);

  vpgl_utm utm; int utm_zone; double x,y;
  utm.transform(lat, -lon, x, y, utm_zone);
  vcl_cout << " zone of lidar img: " << img_file << ": " << utm_zone << " from lower left corner!\n";

  cam->img_to_global(info.ni-1, 0.0, lon, lat);
  vgl_point_2d<double> upper_right(-lon, lat);
  vgl_box_2d<double> bbox(lower_left, upper_right);
  //vcl_cout << "bbox: " << bbox << vcl_endl;
  info.bbox = bbox;

  return utm_zone;
}

void volm_io_tools::load_lidar_imgs(vcl_string const& folder, vcl_vector<volm_img_info>& infos) {
  vcl_string in_dir = folder + "*.tif";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    vcl_string filename = fn();
    volm_img_info info;
    load_lidar_img(filename, info);
    infos.push_back(info);
  }
}

void volm_io_tools::load_nlcd_imgs(vcl_string const& folder, vcl_vector<volm_img_info>& infos)
{
  vcl_string in_dir = folder + "*.tif";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    vcl_string filename = fn();
    volm_img_info info;
    load_lidar_img(filename, info, true);
    infos.push_back(info);
  }
}