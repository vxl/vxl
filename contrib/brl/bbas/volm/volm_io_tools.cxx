#include <iostream>
#include <algorithm>
#include "volm_io_tools.h"
//:
// \file


#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpgl/vpgl_utm.h>
#include <bkml/bkml_write.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_line_segment_3d.h>


unsigned int volm_io_tools::northing = 0;  // WARNING: north hard-coded

static double eps = 1.0e-5;
inline bool near_zero(double x) { return x < eps && x > -eps; }
inline bool near_equal(double x, double y) { return near_zero(x-y); }
bool near_eq_pt(vgl_point_2d<double> a, vgl_point_2d<double> b)
{
  return (near_equal(a.x(),b.x()) && near_equal(a.y(), b.y()));
}


void volm_img_info::save_box_kml(const std::string& out_name) {
    std::ofstream ofs(out_name.c_str());
    bkml_write::open_document(ofs);
    bkml_write::write_box(ofs, name, "", bbox);
    bkml_write::close_document(ofs);
  }

bool read_box(const std::string& bbox_file, vgl_box_2d<double>& bbox) {
  char buffer[1000];
  std::ifstream ifs(bbox_file.c_str());
  if (!ifs.is_open()) {
    std::cerr << " cannot open: " << bbox_file << "!\n";
    return false;
  }

  std::string dummy; double top_lat, bottom_lat, left_lon, right_lon;
  for (unsigned kk = 0; kk < 22; kk++)
    ifs.getline(buffer, 1000);
  // top
  ifs.getline(buffer, 1000);
  std::stringstream top_edge_line(buffer);
  top_edge_line >> dummy; top_edge_line >> dummy; top_edge_line >> dummy;
  top_edge_line >> top_lat;
  // bottom
  ifs.getline(buffer, 1000);
  std::stringstream bot_edge_line(buffer);
  bot_edge_line >> dummy; bot_edge_line >> dummy; bot_edge_line >> dummy;
  bot_edge_line >> bottom_lat;
  // left
  ifs.getline(buffer, 1000);
  std::stringstream left_edge_line(buffer);
  left_edge_line >> dummy; left_edge_line >> dummy; left_edge_line >> dummy;
  left_edge_line >> left_lon;
  // right
  ifs.getline(buffer, 1000);
  std::stringstream right_edge_line(buffer);
  right_edge_line >> dummy; right_edge_line >> dummy; right_edge_line >> dummy;
  right_edge_line >> right_lon;

  vgl_point_2d<double> lower_left(left_lon, bottom_lat);
  vgl_point_2d<double> upper_right(right_lon, top_lat);
  bbox = vgl_box_2d<double>(lower_left, upper_right);
  //std::cout << "bbox: " << bbox << std::endl;
  return true;
}

bool volm_io_tools::load_naip_img(std::string const& img_folder, std::string const& name, vpgl_lvcs_sptr& lvcs, volm_img_info& info, bool load_resource)
{
  std::string filename = img_folder + "\\" + name;
  std::string img_name = filename + "\\" + name + ".tif";
  std::string tfw_name = filename + "\\" + name + ".tfw";
  if (!vul_file::exists(tfw_name) || !vul_file::exists(img_name))
    return false;

  info.name = name; info.img_name = img_name;
  std::string bbox_file = filename + "\\output_parameters.txt";
  if (!read_box(bbox_file, info.bbox)) {
    std::cerr << " cannot find: " << bbox_file << std::endl;
    return false;
  }
  //std::cout << "NAIP bbox: " << info.bbox << std::endl;
  // figure out utm zone
  vpgl_utm utm; int utm_zone, zone_max; double xx, yy;
  utm.transform(info.bbox.min_point().y(), info.bbox.min_point().x(), xx, yy, utm_zone);
  utm.transform(info.bbox.max_point().y(), info.bbox.max_point().x(), xx, yy, zone_max);
  if (utm_zone != zone_max) {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!WARNING! img: " << img_name << " has min and max points in different UTM zones, using zone of min point!\n";

  }

  vpgl_geo_camera *cam = nullptr;
  if (!vpgl_geo_camera::init_geo_camera(tfw_name, lvcs, utm_zone, northing, cam))
    return false;
  info.cam = cam;

  if (load_resource) {
    vil_image_resource_sptr img = vil_load_image_resource(img_name.c_str());
    std::cout << "ni: " << img->ni() <<" nj: " << img->nj() <<std::endl;
    info.ni = img->ni(); info.nj = img->nj();
  }
  return true;
}

bool volm_io_tools::load_naip_imgs(std::string const& img_folder, std::vector<volm_img_info>& imgs, bool load_resource) {

  vpgl_lvcs_sptr lvcs = new vpgl_lvcs; // just the default, no concept of local coordinate system here, so won't be used

  std::string in_dir = img_folder + "*";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    std::string filename = fn();
    //std::cout << "filename: " << filename << std::endl;
    std::string file = vul_file::strip_directory(filename);

    volm_img_info info;
    if (load_naip_img(img_folder, file, lvcs, info, load_resource))
      imgs.push_back(info);
  }
  return true;
}

int volm_io_tools::load_lidar_img(const std::string& img_file, volm_img_info& info, bool load_image_resource,
                                  bool is_cam_global,
                                  bool load_cam_from_tfw, std::string const& cam_tfw_file)
{
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

  vpgl_geo_camera *cam = nullptr;
  // try to load camera from image header first
  vil_image_resource_sptr img_res = vil_load_image_resource(info.img_name.c_str());
  vpgl_lvcs_sptr lvcs_dummy = new vpgl_lvcs;
  vpgl_geo_camera::init_geo_camera(img_res, lvcs_dummy, cam);

  if (!cam) {
    if (!is_cam_global && !load_cam_from_tfw) {
      vpgl_geo_camera::init_geo_camera(img_file, info.ni, info.nj, lvcs, cam);
      info.cam = cam;
    } else if (load_cam_from_tfw) {
      vpgl_geo_camera::init_geo_camera(cam_tfw_file, lvcs, 0, 0, cam);
      info.cam = cam;
    } else if (is_cam_global) {
      vpgl_geo_camera::init_geo_camera_from_filename(img_file, info.ni, info.nj, lvcs, cam);
      info.cam = cam;
    }
  }
  else
    info.cam = cam;

  // obtain the bounding box of current image
  std::string name = vul_file::strip_directory(img_file);
  name = name.substr(name.find_first_of('_')+1, name.size());

  std::string n_coords = name.substr(0, name.find_first_of('_'));
  std::string hemisphere, direction;
  std::size_t n = n_coords.find('N');
  if (n < n_coords.size())
    hemisphere = "N";
  else
    hemisphere = "S";
  n = n_coords.find('E');
  if (n < n_coords.size())
    direction = "E";
  else
    direction = "W";

  double lat, lon;
  cam->img_to_global(0.0, info.nj-1, lon, lat);
  if (!is_cam_global) {
    if (direction == "W")
      lon = -lon;
    if (hemisphere == "S")
      lat = -lat;
  }
  vgl_point_2d<double> lower_left(lon, lat);

  vpgl_utm utm; int utm_zone; double x,y;
  utm.transform(lat, lon, x, y, utm_zone);
  std::cout << " zone of lidar img: " << img_file << ": " << utm_zone << " from lower left corner: " << lon << ',' << lat << "!\n";

  cam->img_to_global(info.ni-1, 0.0, lon, lat);
  if (!is_cam_global) {
    if (direction == "W")
      lon = -lon;
    if (hemisphere == "S")
      lat = -lat;
  }
  vgl_point_2d<double> upper_right(lon, lat);
  vgl_box_2d<double> bbox(lower_left, upper_right);
  std::cout << "bbox: " << bbox << std::endl;
  info.bbox = bbox;

  return utm_zone;
}

void volm_io_tools::load_lidar_imgs(std::string const& folder, std::vector<volm_img_info>& infos) {
  std::string in_dir = folder + "/lidar_*.tif";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    std::string filename = fn();
    volm_img_info info;
    volm_io_tools::load_lidar_img(filename, info, true, true, false);
    infos.push_back(info);
  }
}

void volm_io_tools::load_nlcd_imgs(std::string const& folder, std::vector<volm_img_info>& infos)
{
  std::string in_dir = folder + "*.tif*";  // sometimes .tif is written .tiff
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    std::string filename = fn();
    volm_img_info info;
    load_geotiff_image(filename, info, true);
    infos.push_back(info);
  }
}
void volm_io_tools::load_imgs(std::string const& folder, std::vector<volm_img_info>& infos, bool load_image_resource, bool is_cam_global, bool load_cam_from_tfw)
{
  std::string in_dir = folder + "/*.tif";  // sometimes .tif is written .tiff
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    std::string filename = fn();
    std::string cam_tfw_file = vul_file::strip_extension(filename) + ".tfw";
    volm_img_info info;
    load_lidar_img(filename, info, load_image_resource, is_cam_global, load_cam_from_tfw, cam_tfw_file);
    infos.push_back(info);
  }
  // also load .tiff
  in_dir = folder + "/*.tiff";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    std::string filename = fn();
    std::string cam_tfw_file = vul_file::strip_extension(filename) + ".tfw";
    volm_img_info info;
    load_lidar_img(filename, info, load_image_resource, is_cam_global, load_cam_from_tfw, cam_tfw_file);
    infos.push_back(info);
  }
}

bool volm_io_tools::get_location_nlcd(std::vector<volm_img_info>& NLCD_imgs, double lat, double lon, double elev, unsigned char& label)
{
  bool found_it = false;
  for (auto & NLCD_img : NLCD_imgs) {
    if (NLCD_img.bbox.contains(lon, lat)) {
      vil_image_view<vxl_byte> img(NLCD_img.img_r);

      // get the land type of the location
      double u, v;
      NLCD_img.cam->global_to_img(lon, lat, elev, u, v);
      //NLCD_imgs[i].cam->global_to_img(-lon, lat, elev, u, v);
      auto uu = (unsigned)std::floor(u + 0.5);
      auto vv = (unsigned)std::floor(v + 0.5);
      if (uu > 0 && vv > 0 && uu < NLCD_img.ni && vv < NLCD_img.nj) {
        label = img(uu, vv);
        found_it = true;
        break;
      }
    }
  }
  return found_it;
}

bool volm_io_tools::expend_line(std::vector<vgl_point_2d<double> > line, double const& w, vgl_polygon<double>& poly)
{
  std::vector<vgl_point_2d<double> > sheet;
  std::vector<vgl_point_2d<double> > pts_u;
  std::vector<vgl_point_2d<double> > pts_d;
  unsigned n_pts = line.size();
  for (unsigned i = 0; i < n_pts-1; i++) {
    vgl_point_2d<double> s, e;
    s = line[i];  e = line[i+1];
    if (near_eq_pt(s,e))
      continue;
    vgl_line_2d<double> seg(s, e);
    vgl_vector_2d<double> n = seg.normal();
    vgl_point_2d<double> su, sd, eu, ed;
    su = s + 0.5*w*n;  sd = s - 0.5*w*n;
    pts_u.push_back(su);  pts_d.push_back(sd);
    if (i == n_pts-2) {
      eu = e + 0.5*w*n;  ed = e - 0.5*w*n;
      pts_u.push_back(eu);  pts_d.push_back(ed);
    }
  }
  // rearrange the point list
  sheet.reserve(pts_u.size());
for (auto i : pts_u)
    sheet.push_back(i);
  for (int i = pts_d.size()-1; i >=0; i--)
    sheet.push_back(pts_d[i]);
  poly.push_back(sheet);
  return true;
}


#include <volm/volm_geo_index2.h>
#include <volm/volm_osm_objects.h>

// a method to read the binary osm object file and also contstruct the volm_geo_index2, the method returns the root of the tree
volm_geo_index2_node_sptr volm_io_tools::read_osm_data_and_tree(const std::string& geoindex_filename_pre, const std::string& osm_bin_filename, volm_osm_objects& osm_objs, double& min_size)
{
  std::string filename = geoindex_filename_pre + ".txt";
  volm_geo_index2_node_sptr root = volm_geo_index2::read_and_construct<volm_osm_object_ids_sptr>(filename, min_size);
  // obtain all leaves
  std::vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);
  // load the content for valid leaves
  for (auto & leave : leaves) {
    std::string bin_file = leave->get_label_name(geoindex_filename_pre, "osm");
    if (!vul_file::exists(bin_file))
      continue;
    auto* ptr = dynamic_cast<volm_geo_index2_node<volm_osm_object_ids_sptr>* >(leave.ptr());
    ptr->contents_ = new volm_osm_object_ids(bin_file);
  }

  // load the osm bin file to get real location date with unit of lon and lat, associated with ids stored in geo_index2
  if (!vul_file::exists(osm_bin_filename)) {
    std::cout << "ERROR: can not find osm binary: " << osm_bin_filename << std::endl;
    return nullptr;
  }

  vsl_b_ifstream is(osm_bin_filename.c_str());
  if (!is) {
    std::cerr << "In volm_osm_object::volm_osm_object() -- cannot open: " << osm_bin_filename << std::endl;
    return nullptr;
  }
  osm_objs.b_read(is);
  is.close();

#if 0
  if (is_kml()) {
      std::stringstream kml_pts, kml_roads, kml_regions;
      kml_pts << out_pre() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_pts.kml";
      kml_roads << out_pre() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_roads.kml";
      kml_regions << out_pre() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_regions.kml";
      osm_objs.write_pts_to_kml(kml_pts.str());
      osm_objs.write_lines_to_kml(kml_roads.str());
      osm_objs.write_polys_to_kml(kml_regions.str());
  }
#endif

  return root;
}

//: load a geotiff file with .tif file and read its ortho camera info from its header, puts a dummy lvcs to vpgl_geo_cam object so lvcs is not valid
//  even though it reads the camera from filename with N/S and W/E distinction, it constructs a camera in global WGS84 coordinates, so the global coordinates should be used to fetch pixels, (i.e. no need to make them always positive)
void volm_io_tools::load_geotiff_image(const std::string& filename, volm_img_info& info, bool load_cam_from_name)
{
  info.img_name = filename;
  info.name = vul_file::strip_directory(info.img_name);
  info.name = vul_file::strip_extension(info.name);

  info.img_r = vil_load(info.img_name.c_str());
  info.ni = info.img_r->ni(); info.nj = info.img_r->nj();

  vpgl_geo_camera *cam;
  vpgl_lvcs_sptr lvcs_dummy = new vpgl_lvcs;
  if (load_cam_from_name) {
    vpgl_geo_camera::init_geo_camera_from_filename(filename, info.ni, info.nj, lvcs_dummy, cam); // constructs in global WGS84 (no distinction of N/S or W/E)
    std::cout << cam->trans_matrix() << std::endl;
  } else {
    vil_image_resource_sptr img_res = vil_load_image_resource(info.img_name.c_str());
    vpgl_geo_camera::init_geo_camera(img_res, lvcs_dummy, cam);
  }

  info.cam = cam;

  double lat, lon;
  cam->img_to_global(0.0, info.nj-1, lon, lat);
  vgl_point_2d<double> lower_left(lon, lat);

  vpgl_utm utm; int utm_zone; double x,y;
  utm.transform(lat, lon, x, y, utm_zone);
  cam->img_to_global(info.ni-1, 0.0, lon, lat);
  vgl_point_2d<double> upper_right(lon, lat);
  vgl_box_2d<double> bbox(lower_left, upper_right);
  info.bbox = bbox;
}

void volm_io_tools::load_aster_dem_imgs(std::string const& folder, std::vector<volm_img_info>& infos)
{
  std::string file_glob = folder + "//*.tif";
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn) {
    volm_img_info info;
    volm_io_tools::load_geotiff_image(fn(), info);
    infos.push_back(info);
    //volm_img_info info;
    //std::string folder = fn();
    //std::string file_glob2 = folder + "//" + "*_dem*.tif";
    //for (vul_file_iterator fn2 = file_glob2.c_str(); fn2; ++fn2) {
    //  volm_io_tools::load_geotiff_image(fn2(), info);
    //  infos.push_back(info);
    //}
  }
}

bool volm_io_tools::load_satellite_height_map(std::string const& filename, volm_img_info& info, bool const& load_cam_from_file)
{
  info.img_name = filename;
  info.name = vul_file::strip_directory(info.img_name);
  info.name = vul_file::strip_extension(info.name);

  info.img_r = vil_load(info.img_name.c_str());
  info.ni = info.img_r->ni();  info.nj = info.img_r->nj();
  std::cout << "satellite height image ni: " << info.ni << " nj: " << info.nj << std::endl;
  // load the camera (either from a tfw file or from image header)
  vpgl_geo_camera *cam;
  vpgl_lvcs_sptr lvcs_dummy = new vpgl_lvcs;
  if (load_cam_from_file) {
    std::string cam_file = vul_file::dirname(filename) + "/" + info.name + "_geo.tfw";
    if (!vul_file::exists(cam_file)) {
      std::cerr << " can not find camera file: " << cam_file << "!\n";
      return false;
    }
    vpgl_geo_camera::init_geo_camera(cam_file, lvcs_dummy, 0, 0, cam);
  }
  else { // load from image header
    vil_image_resource_sptr img_res = vil_load_image_resource(info.img_name.c_str());
    vpgl_geo_camera::init_geo_camera(img_res, lvcs_dummy, cam);
  }

  // defined the bbox of the image, use lower_left corner to define utm and northing of camera
  double lat, lon;
  cam->img_to_global(0.0, info.nj-1, lon, lat);
  vgl_point_2d<double> lower_left(lon, lat);
  vpgl_utm utm;
  int utm_zone; double x, y;
  utm.transform(lat, lon, x, y, utm_zone);
  std::cout << " zone of satellite height image " << info.name << ": " << utm_zone << " from lower left corner!\n";
  cam->img_to_global(info.ni-1,0.0, lon, lat);
  vgl_point_2d<double> upper_right(lon, lat);
#if 0
  int utm_zone_ur;
  utm.transform(lat, lon, x, y, utm_zone_ur);
  if (utm_zone_ur != utm_zone)
    std::cout << " warning: satellite height image " << info.name << " crosses two utm zone: " << utm_zone << ", " << utm_zone_ur << std::endl;
  unsigned northing = 1;
  if (upper_right.x() < 0 && lower_left.x() < 0)
    northing = 0;
  if (upper_right.x() * lower_left.x() < 0)
    std::cout << " warning: satellite height image " << info.name << " crosses the Equator (set it to be northing)" << std::endl;
  cam->set_utm(utm_zone, northing);
#endif
  info.cam = cam;
  vgl_box_2d<double> bbox(lower_left, upper_right);
  std::cout << "bbox: " << bbox << std::endl;
  info.bbox = bbox;

  return true;
}

bool volm_io_tools::load_satellite_height_imgs(std::string const& folder, std::vector<volm_img_info>& infos, bool const& load_cam_from_file, std::string const& keywords)
{
  std::string file_glob;
  if (keywords.compare("") == 0)
    file_glob = folder + "//scene_*.tif";
  else
    file_glob = folder + "//scene_*" + keywords + ".tif";
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn) {
    volm_img_info info;
    if (!volm_io_tools::load_satellite_height_map(fn(), info, load_cam_from_file))
      return false;
    infos.push_back(info);
  }
  return true;
}

void volm_io_tools::load_geocover_imgs(std::string const& folder, std::vector<volm_img_info>& infos)
{
  std::string file_glob = folder + "//*.tif";
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn) {
    volm_img_info info;
    volm_io_tools::load_geotiff_image(fn(), info, false);  // last argument true so load camera from the file name
    infos.push_back(info);
  }
}

void volm_io_tools::load_urban_imgs(std::string const& folder, std::vector<volm_img_info>& infos)
{
  std::string file_glob = folder + "//Urextent_*.tif";
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn) {
    volm_img_info info;
    volm_io_tools::load_geotiff_image(fn(), info, true);
    infos.push_back(info);
  }
}

void crop_and_find_min_max(std::vector<volm_img_info>& infos, unsigned img_id, int i0, int j0, int crop_ni, int crop_nj, double& min, double& max)
{
#if 0
  vil_image_view<vxl_int_16> img(infos[img_id].img_r);
  vil_image_view<vxl_int_16> img_crop = vil_crop(img, i0, crop_ni, j0, crop_nj);
  for (unsigned ii = 0; ii < img_crop.ni(); ii++)
    for (unsigned jj = 0; jj < img_crop.nj(); jj++) {
      if (min > img_crop(ii, jj)) min = img_crop(ii, jj);
      if (max < img_crop(ii, jj)) max = img_crop(ii, jj);
    }
#endif
  if (auto* img = dynamic_cast<vil_image_view<vxl_int_16>*>(infos[img_id].img_r.ptr())) {
    vil_image_view<vxl_int_16> img_crop = vil_crop(*img, i0, crop_ni, j0, crop_nj);
    for (unsigned ii = 0; ii < img_crop.ni(); ii++)
      for (unsigned jj = 0; jj < img_crop.nj(); jj++) {
        if (min > img_crop(ii, jj)) min = img_crop(ii, jj);
        if (max < img_crop(ii, jj)) max = img_crop(ii, jj);
      }
  }
  else if (auto* img = dynamic_cast<vil_image_view<float>*>(infos[img_id].img_r.ptr()) ) {
    vil_image_view<float> img_crop = vil_crop(*img, i0, crop_ni, j0, crop_nj);
    for (unsigned ii = 0; ii < img_crop.ni(); ii++)
      for (unsigned jj = 0; jj < img_crop.nj(); jj++) {
        if (min > img_crop(ii, jj)) min = img_crop(ii, jj);
        if (max < img_crop(ii, jj)) max = img_crop(ii, jj);
      }
  }


}

bool volm_io_tools::find_min_max_height(vgl_point_2d<double>& lower_left, vgl_point_2d<double>& upper_right, std::vector<volm_img_info>& infos, double& min, double& max)
{
  // find the image of all four corners
  std::vector<std::pair<unsigned, std::pair<int, int> > > corners;
  std::vector<vgl_point_2d<double> > pts;
  pts.emplace_back(lower_left.x(), upper_right.y());
  pts.emplace_back(upper_right.x(), lower_left.y());
  pts.push_back(lower_left);
  pts.push_back(upper_right);

  for (auto & pt : pts) {
    // find the image
    for (unsigned j = 0; j < (unsigned)infos.size(); j++) {
      double u, v;
      infos[j].cam->global_to_img(pt.x(), pt.y(), 0, u, v);
      int uu = (int)std::floor(u+0.5);
      int vv = (int)std::floor(v+0.5);
      if (uu < 0 || vv < 0 || uu >= (int)infos[j].ni || vv >= (int)infos[j].nj)
        continue;
      std::pair<unsigned, std::pair<int, int> > pp(j, std::pair<int, int>(uu, vv));
      corners.push_back(pp);
      break;
    }
  }
  if (corners.size() != 4) {
    std::cerr << "Cannot locate all 4 corners among these DEM tiles!\n";
    return false;
  }
  // case 1: all corners are in the same image
  if (corners[0].first == corners[1].first) {
    // crop the image
    int i0 = corners[0].second.first;
    int j0 = corners[0].second.second;
    int crop_ni = corners[1].second.first-corners[0].second.first+1;
    int crop_nj = corners[1].second.second-corners[0].second.second+1;
    crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);
    return true;
  }
  // case 2: two corners are in the same image
  if (corners[0].first == corners[2].first && corners[1].first == corners[3].first) {
    // crop the first image
    int i0 = corners[0].second.first;
    int j0 = corners[0].second.second;
    int crop_ni = infos[corners[0].first].ni - corners[0].second.first;
    int crop_nj = corners[2].second.second-corners[0].second.second+1;
    crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);

    // crop the second image
    i0 = 0;
    j0 = corners[3].second.second;
    crop_ni = corners[3].second.first + 1;
    crop_nj = corners[1].second.second-corners[3].second.second+1;
    crop_and_find_min_max(infos, corners[1].first, i0, j0, crop_ni, crop_nj, min, max);
    return true;
  }
  // case 3: two corners are in the same image
  if (corners[0].first == corners[3].first && corners[1].first == corners[2].first) {
    // crop the first image
    int i0 = corners[0].second.first;
    int j0 = corners[0].second.second;
    int crop_ni = corners[3].second.first - corners[0].second.first + 1;
    int crop_nj = infos[corners[0].first].nj - corners[0].second.second;
    crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);

    // crop the second image
    i0 = corners[2].second.first;
    j0 = 0;
    crop_ni = corners[1].second.first - corners[2].second.first + 1;
    crop_nj = corners[2].second.second + 1;
    crop_and_find_min_max(infos, corners[1].first, i0, j0, crop_ni, crop_nj, min, max);
    return true;
  }
  // case 4: all corners are in a different image
  // crop the first image, image of corner 0
  int i0 = corners[0].second.first;
  int j0 = corners[0].second.second;
  int crop_ni = infos[corners[0].first].ni - corners[0].second.first;
  int crop_nj = infos[corners[0].first].nj - corners[0].second.second;
  crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);

  // crop the second image, image of corner 1
  i0 = 0;
  j0 = 0;
  crop_ni = corners[1].second.first + 1;
  crop_nj = corners[1].second.second + 1;
  crop_and_find_min_max(infos, corners[1].first, i0, j0, crop_ni, crop_nj, min, max);

  // crop the third image, image of corner 2
  i0 = corners[2].second.first;
  j0 = 0;
  crop_ni = infos[corners[2].first].ni - corners[2].second.first;
  crop_nj = corners[2].second.second + 1;
  crop_and_find_min_max(infos, corners[2].first, i0, j0, crop_ni, crop_nj, min, max);

  // crop the fourth image, image of corner 3
  i0 = 0;
  j0 = corners[3].second.second;
  crop_ni = corners[3].second.first + 1;
  crop_nj = infos[corners[3].first].nj - corners[3].second.second;
  crop_and_find_min_max(infos, corners[3].first, i0, j0, crop_ni, crop_nj, min, max);

  return true;
}

//: use the following method to get the multiplier for conversion of meters to degrees, uses vpgl_lvcs internally
double volm_io_tools::meter_to_seconds(double lat, double lon)
{
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lat, lon, 0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double lon1, lat1, lon2, lat2, gz;
  lvcs->local_to_global(0, 0, 0, vpgl_lvcs::wgs84, lon1, lat1, gz);
  lvcs->local_to_global(1, 0, 0, vpgl_lvcs::wgs84, lon2, lat2, gz);
  double dif_lon = lon2-lon1;
  double dif_lat = lat2-lat1;
  double dif = std::sqrt(dif_lon*dif_lon + dif_lat*dif_lat);  // 1 meter is this many degrees in this area
  return dif;
}

bool find_intersect(vgl_box_2d<double> const& bbox, vgl_point_2d<double> const& s, vgl_point_2d<double> e, vgl_point_2d<double>& pt)
{
  vgl_line_2d<double> line(s, e);
  vgl_point_2d<double> pi0, pi1;
  if (!vgl_intersection(bbox, line, pi0, pi1))
    return false;
  double x1 = s.x(),   y1 = s.y();
  double x2 = e.x(),   y2 = e.y();
  double xp = pi0.x(), yp = pi0.y();
  double d1p = (xp-x1)*(xp-x1) + (yp-y1)*(yp-y1);
  double d2p = (xp-x2)*(xp-x2) + (yp-y2)*(yp-y2);
  double d12 = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
  double diff = std::sqrt(d1p) + std::sqrt(d2p) - std::sqrt(d12);
  if (diff < 1E-5) {
    pt = pi0;
    return true;
  }
  xp = pi1.x();  yp = pi1.y();
  d1p = (xp-x1)*(xp-x1) + (yp-y1)*(yp-y1);
  d2p = (xp-x2)*(xp-x2) + (yp-y2)*(yp-y2);
  d12 = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
  diff = std::sqrt(d1p) + std::sqrt(d2p) - std::sqrt(d12);
  if (diff < 1E-5) {
    pt = pi1;
    return true;
  }
  return false;
}

bool volm_io_tools::line_inside_the_box(vgl_box_2d<double> const& bbox, std::vector<vgl_point_2d<double> > const& line, std::vector<vgl_point_2d<double> >& road)
{
  // obtain points that lie inside the bounding box
  std::vector<vgl_point_2d<double> > line_in = vgl_intersection(line, bbox);
  if (line_in.empty())
    return false;
  for (auto i : line_in)
    road.push_back(i);

  // find the intersection points
  for (auto curr_pt : line_in) {
    auto vit = std::find(line.begin(), line.end(), curr_pt);
    if (vit == line.begin() ) {
      vgl_point_2d<double> next = *(vit+1);
      if (bbox.contains(next))
        continue;
      else {
        vgl_point_2d<double> intersect_pt;
        if (!find_intersect(bbox, curr_pt, next, intersect_pt))
          return false;
        // insert the intersect after current point
        auto it = std::find(road.begin(), road.end(), curr_pt);
        road.insert(it+1, intersect_pt);
      }
    }
    else if (vit == line.end()-1) {
      vgl_point_2d<double> prev = *(vit-1);
      if (bbox.contains(prev))
        continue;
      else {
        vgl_point_2d<double> intersect_pt;
        if (!find_intersect(bbox, prev, curr_pt,intersect_pt))
          return false;
        // insert the intersect point before current point
        auto it = std::find(road.begin(), road.end(), curr_pt);
        road.insert(it, intersect_pt);
      }
    }
    else if (vit != line.end()) {
      vgl_point_2d<double> prev = *(vit-1);
      vgl_point_2d<double> next = *(vit+1);
      if (bbox.contains(next) && bbox.contains(prev))
        continue;
      else if (bbox.contains(next)) {
        vgl_point_2d<double> intersect_pt;
        if (!find_intersect(bbox, prev, curr_pt, intersect_pt))
          return false;
        auto it = std::find(road.begin(), road.end(), curr_pt);
        road.insert(it, intersect_pt);
      }
      else if (bbox.contains(prev)) {
        vgl_point_2d<double> intersect_pt;
        if (!find_intersect(bbox, curr_pt, next, intersect_pt))
          return false;
        // insert the intersect after current point
        auto it = std::find(road.begin(), road.end(), curr_pt);
        road.insert(it+1, intersect_pt);
      }
      else {
        vgl_point_2d<double> intersect_pt;
        if (!find_intersect(bbox, curr_pt, next, intersect_pt))
          return false;
        // find and insert the intersection after current point
        auto it = std::find(road.begin(), road.end(), curr_pt);
        road.insert(it+1, intersect_pt);
        // find and insert the intersection before current point
        if (!find_intersect(bbox, prev, curr_pt,intersect_pt))
          return false;
        it = std::find(road.begin(), road.end(), curr_pt);
        road.insert(it, intersect_pt);
      }
    }
    else
      return false;
  }
  return true;
}

void form_line_segment_from_pts(std::vector<vgl_point_2d<double> > const& road, std::vector<vgl_line_segment_2d<double> >& road_seg)
{
  unsigned num_pts  = road.size();
  unsigned num_segs = num_pts - 1;
  for (unsigned i = 0; i < num_segs; i++) {
    vgl_point_2d<double> s = road[i];  vgl_point_2d<double> e = road[i+1];
    if (near_eq_pt(s,e))
      continue;
    road_seg.emplace_back(s, e);
  }
#if 0
  // define a 2d line fit
  unsigned min_pts = 3;
  double tol = 5.0;  // in pixel unit
  vgl_fit_lines_2d<double> fitter(min_pts, tol);

  unsigned num_pts  = road.size();
  unsigned num_segs = num_pts - 1;
  if (num_pts <= min_pts) {
    for (unsigned i = 0; i < num_segs; i++) {
      vgl_point_2d<double> s = road[i];  vgl_point_2d<double> e = road[i+1];
      road_seg.push_back(vgl_line_segment_2d<double>(s, e));
    }
  }
  else {
    fitter.add_curve(road);
    fitter.fit();
    road_seg = fitter.get_line_segs();
    if (road_seg.empty()) {
      // fitting failed form the segment directly
      road_seg.clear();
      for (unsigned i = 0; i < num_segs; i++) {
        vgl_point_2d<double> s = road[i];  vgl_point_2d<double> e = road[i+1];
        road_seg.push_back(vgl_line_segment_2d<double>(s, e));
      }
    }
  }
  fitter.clear();
#endif
}

void find_junctions(vgl_line_segment_2d<double> const& seg,
                    volm_land_layer const& seg_prop,
                    std::vector<vgl_line_segment_2d<double> > const& lines,
                    volm_land_layer const& line_prop,
                    std::vector<vgl_point_2d<double> >& cross_pts,
                    std::vector<volm_land_layer>& cross_prop)
{
  vgl_line_segment_3d<double> l1(vgl_point_3d<double>(seg.point1().x(), seg.point1().y(), 0.0), vgl_point_3d<double>(seg.point2().x(), seg.point2().y(), 0.0));

  unsigned n_seg = lines.size();
  for (unsigned i = 0; i < n_seg; i++) {
    vgl_line_segment_3d<double> l2(vgl_point_3d<double>(lines[i].point1().x(), lines[i].point1().y(), 0.0),
                                   vgl_point_3d<double>(lines[i].point2().x(), lines[i].point2().y(), 0.0));
    vgl_point_3d<double> pt;
    // check intersection
    if (!vgl_intersection(l1, l2, pt)) {
      // check whether connect at end point
      if (near_eq_pt(seg.point1(), lines[i].point1()))
        pt.set(seg.point1().x(), seg.point1().y(), 0.0);
      else if (near_eq_pt(seg.point1(), lines[i].point2()))
        pt.set(seg.point1().x(), seg.point1().y(), 0.0);
      else if (near_eq_pt(seg.point2(), lines[i].point1()))
        pt.set(seg.point2().x(), seg.point2().y(), 0.0);
      else if (near_eq_pt(seg.point2(), lines[i].point2()))
        pt.set(seg.point2().x(), seg.point2().y(), 0.0);
      else
        continue;
    }
    cross_pts.emplace_back(pt.x(), pt.y());
    std::pair<int,int> key(seg_prop.id_, line_prop.id_);
    cross_prop.push_back(volm_osm_category_io::road_junction_table[key]);
  }
}

unsigned count_line_start_from_cross(vgl_point_2d<double> const& cross_pt,
                                     std::vector<vgl_point_2d<double> > const& rd,
                                     std::vector<std::vector<vgl_point_2d<double> > > const& net)
{
  unsigned cnt = 0;
  // check current road first
  vgl_point_2d<double> s = *(rd.begin());  vgl_point_2d<double> e = *(rd.end()-1);
  if ( near_eq_pt(cross_pt, s) || near_eq_pt(cross_pt, e))
    cnt++;
  for (const auto & i : net) {
    s = *(i.begin());  e = *(i.end()-1);
    if ( near_eq_pt(cross_pt, s) || near_eq_pt(cross_pt, e))
      cnt++;
  }
  return cnt;
}

bool volm_io_tools::search_junctions(std::vector<vgl_point_2d<double> > const& road, volm_land_layer const& road_prop,
                                     std::vector<std::vector<vgl_point_2d<double> > > net, std::vector<volm_land_layer> net_props,
                                     std::vector<vgl_point_2d<double> >& cross_pts, std::vector<volm_land_layer>& cross_props,
                                     std::vector<volm_land_layer>& cross_geo_props)
{
  unsigned n_rds = net.size();

  // form the line segment for each road in the network
  std::vector<vgl_line_segment_2d<double> > road_seg;
  form_line_segment_from_pts(road, road_seg);

  std::vector<std::vector<vgl_line_segment_2d<double> > > net_segs;
  for (unsigned r_idx = 0; r_idx < n_rds; r_idx++) {
    std::vector<vgl_line_segment_2d<double> > seg;
    form_line_segment_from_pts(net[r_idx], seg);
    net_segs.push_back(seg);
  }

  // find the cross for each segment
  unsigned n_seg = road_seg.size();
  for (unsigned s_idx = 0; s_idx < n_seg; s_idx++) {
    vgl_line_segment_2d<double> curr_seg = road_seg[s_idx];
    for (unsigned r_idx = 0; r_idx < n_rds; r_idx++) {
      std::vector<vgl_line_segment_2d<double> > curr_net_seg = net_segs[r_idx];
      std::vector<vgl_point_2d<double> > pt;
      std::vector<volm_land_layer> prop;
      find_junctions(curr_seg, road_prop, curr_net_seg, net_props[r_idx], pt, prop);
      if (pt.empty())
        continue;
      if (prop.size() != pt.size())
        return false;
      for (unsigned p_idx = 0; p_idx < pt.size(); p_idx++)
        if (std::find(cross_pts.begin(), cross_pts.end(), pt[p_idx]) == cross_pts.end()) {
          cross_pts.push_back(pt[p_idx]);  cross_props.push_back(prop[p_idx]);  cross_geo_props.push_back(volm_osm_category_io::volm_land_table_name["4_way"]);
        }
    }
  }

   //check whether the cross pt are T_section
   //principle, count the number of roads whose end points are on the cross_pt
   //if there is 1 or 3 lines start from this cross pt, the cross is a T_section
  for (unsigned c_idx = 0; c_idx < cross_pts.size(); c_idx++) {
    unsigned num_lines = count_line_start_from_cross(cross_pts[c_idx], road, net);
    if (num_lines == 1 || num_lines == 3) {
      cross_props[c_idx] = volm_osm_category_io::volm_land_table_name["T_section"];
    }
  }
  // define the geometric properties of intersections
  for (unsigned c_idx = 0; c_idx < cross_pts.size(); c_idx++) {
    unsigned num_lines = count_line_start_from_cross(cross_pts[c_idx], road, net);
    if (num_lines == 1 || num_lines == 3) {
      cross_geo_props[c_idx] = volm_osm_category_io::volm_land_table_name["T_section"];
    }
    if (num_lines == 2) {
      cross_geo_props[c_idx] = volm_osm_category_io::volm_land_table_name["2_way"];
    }
  }

  return true;
}
