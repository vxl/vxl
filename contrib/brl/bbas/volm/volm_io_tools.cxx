#include "volm_io_tools.h"
//:
// \file


#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpgl/vpgl_utm.h>
#include <bkml/bkml_write.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>


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
  vcl_string in_dir = folder + "*.tif*";  // sometimes .tif is written .tiff 
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    vcl_string filename = fn();
    volm_img_info info;
    load_lidar_img(filename, info, true);
    infos.push_back(info);
  }
}
void volm_io_tools::load_imgs(vcl_string const& folder, vcl_vector<volm_img_info>& infos, bool load_image_resource)
{
  vcl_string in_dir = folder + "*.tif*";  // sometimes .tif is written .tiff 
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    vcl_string filename = fn();
    volm_img_info info;
    load_lidar_img(filename, info, load_image_resource);
    infos.push_back(info);
  }
}

bool volm_io_tools::get_location_nlcd(vcl_vector<volm_img_info>& NLCD_imgs, double lat, double lon, double elev, unsigned char& label)
{
  bool found_it = false;
  for (unsigned i = 0; i < NLCD_imgs.size(); i++) {
    if (NLCD_imgs[i].bbox.contains(lon, lat)) {
      vil_image_view<vxl_byte> img(NLCD_imgs[i].img_r);

      // get the land type of the location
      double u, v;
      NLCD_imgs[i].cam->global_to_img(-lon, lat, elev, u, v);
      unsigned uu = (unsigned)vcl_floor(u + 0.5);
      unsigned vv = (unsigned)vcl_floor(v + 0.5);
      if (uu > 0 && vv > 0 && uu < NLCD_imgs[i].ni && vv < NLCD_imgs[i].nj) {
        label = img(uu, vv);
        found_it = true;
        break;
      }
    }   
  }
  return found_it;
}


#include <volm/volm_geo_index2.h>
#include <volm/volm_osm_objects.h>

// a method to read the binary osm object file and also contstruct the volm_geo_index2, the method returns the root of the tree
volm_geo_index2_node_sptr volm_io_tools::read_osm_data_and_tree(vcl_string geoindex_filename_pre, vcl_string osm_bin_filename, volm_osm_objects& osm_objs, double& min_size)
{
  vcl_string filename = geoindex_filename_pre + ".txt";
  volm_geo_index2_node_sptr root = volm_geo_index2::read_and_construct<volm_osm_object_ids_sptr>(filename, min_size);
  // obtain all leaves
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);
  // load the content for valid leaves
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    vcl_string bin_file = leaves[l_idx]->get_label_name(geoindex_filename_pre, "osm");
    if (!vul_file::exists(bin_file))
      continue;
    volm_geo_index2_node<volm_osm_object_ids_sptr>* ptr = dynamic_cast<volm_geo_index2_node<volm_osm_object_ids_sptr>* >(leaves[l_idx].ptr());
    ptr->contents_ = new volm_osm_object_ids(bin_file);
  }

  // load the osm bin file to get real location date with unit of lon and lat, associated with ids stored in geo_index2
  if (!vul_file::exists(osm_bin_filename)) {
    vcl_cout << "ERROR: can not find osm binary: " << osm_bin_filename << vcl_endl;
    return 0;
  }

  vsl_b_ifstream is(osm_bin_filename.c_str());
  if (!is) {
    vcl_cerr << "In volm_osm_object::volm_osm_object() -- cannot open: " << osm_bin_filename << vcl_endl;
    return 0;
  }
  osm_objs.b_read(is);
  is.close();

#if 0
  if (is_kml()) {
      vcl_stringstream kml_pts, kml_roads, kml_regions;
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


void volm_io_tools::load_aster_dem_imgs(vcl_string const& folder, vcl_vector<volm_img_info>& infos)
{
  vcl_string file_glob = folder + "//ASTGTM2_*";
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn) {
    volm_img_info info;

    vcl_string folder = fn();
    info.name = vul_file::strip_directory(folder);

    info.img_name = folder + "//" + info.name + "_dem.tif";
    

    info.img_r = vil_load(info.img_name.c_str());
    info.ni = info.img_r->ni(); info.nj = info.img_r->nj(); 
    vcl_cout << "ASTER DEM ni: " << info.ni << " nj: " << info.nj << vcl_endl;
  
    vil_image_resource_sptr img_res = vil_load_image_resource(info.img_name.c_str());
    vpgl_geo_camera *cam;
    vpgl_lvcs_sptr lvcs_dummy = new vpgl_lvcs;
    vpgl_geo_camera::init_geo_camera(img_res, lvcs_dummy, cam);

    info.cam = cam; 
    
    double lat, lon;
    cam->img_to_global(0.0, info.nj-1, lon, lat);
    vgl_point_2d<double> lower_left(lon, lat);

    vpgl_utm utm; int utm_zone; double x,y;
    utm.transform(lat, lon, x, y, utm_zone);
    vcl_cout << " zone of ASTER DEM img: " << info.name << ": " << utm_zone << " from lower left corner!\n";

    cam->img_to_global(info.ni-1, 0.0, lon, lat);
    vgl_point_2d<double> upper_right(lon, lat);
    vgl_box_2d<double> bbox(lower_left, upper_right);
    vcl_cout << "bbox: " << bbox << vcl_endl;
    info.bbox = bbox;

    infos.push_back(info);
  }
}
 