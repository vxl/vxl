//This is brl/bbas/volm/volm_io_tools.h
#ifndef volm_io_tools_h_
#define volm_io_tools_h_
//:
// \file
// \brief This file holds several volm img I/O oriented classes
//
// \author Ozge C. Ozcanli
// \date May 31, 2013
// \verbatim
//  Modifications
//    none
// \endverbatim

#include <string>
#include <utility>
#include <iostream>
#include <iomanip>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_intersection.h>
#include <vil/vil_image_view_base.h>
#include <volm/volm_geo_index2_sptr.h>
#include <volm/volm_osm_objects.h>
#include <vgl/vgl_area.h>

class volm_img_info
{
public:
  volm_img_info() = default;
  bool intersects(vgl_polygon<double> poly) { return vgl_intersection(bbox, poly); }
  bool intersects(vgl_box_2d<double> other) { return vgl_area(vgl_intersection(bbox, other)) > 0; }
  bool contains(vgl_point_2d<double> point) const { return bbox.contains(point); }
  bool contains(double const& lon, double const& lat) const { return this->contains(vgl_point_2d<double>(lon, lat)); }
  bool valid_pixel(int uu, int vv) const { return (uu >= 0 && vv >= 0 && uu < (int)ni && vv < (int)nj) ? true : false; }
  void save_box_kml(const std::string& out_name);

  unsigned ni, nj;
  vpgl_geo_camera* cam;   // in UTM for NAIP imgs
  vgl_box_2d<double> bbox;   // in lon (x), lat (y).  lower left corner is (0,nj), upper right corner is (ni, 0)
  std::string name;
  std::string img_name;
  vil_image_view_base_sptr img_r;
};

class volm_io_tools
{
public:

  static unsigned int northing; // will hard code to 0  // WARNING: north hard-coded

  static bool load_naip_img(std::string const& img_folder, std::string const& name, vpgl_lvcs_sptr& lvcs, volm_img_info& info, bool load_resource = false);

  static bool load_naip_imgs(std::string const& img_folder, std::vector<volm_img_info>& imgs, bool load_resource = false);

  // load vxl_byte image, the associate is either loaded from img filename or from a camera tfw file
  static int load_lidar_img(const std::string& img_file, volm_img_info& info,
                            bool load_resource = false,
                            bool is_cam_global = false,
                            bool load_cam_from_tfw = false,
                            std::string const& cam_tfw_file = "");

  //: load lidar image and camera from geo tiff image header: use glob: folder//lidar_*.tif
  static void load_lidar_imgs(std::string const& folder, std::vector<volm_img_info>& infos);

  static void load_nlcd_imgs(std::string const& folder, std::vector<volm_img_info>& infos);

  // load vxl_byte images using load_lidar_img, if camera are loaded from tfw file, it assumes the camera file has same filename as image file
  static void load_imgs(std::string const& folder, std::vector<volm_img_info>& infos, bool load_resource = false,
                        bool is_cam_global = false,
                        bool load_cam_from_tfw = false);

  static bool get_location_nlcd(std::vector<volm_img_info>& infos, double lat, double lon, double elev, unsigned char& label);

  //: expend a list of line segment to a polygon given a width
  static bool expend_line(std::vector<vgl_point_2d<double> > line, double const& width, vgl_polygon<double>& poly);

  // a method to read the binary osm object file and also contstruct the volm_geo_index2, the method returns the root and the min_size of the tree
  static volm_geo_index2_node_sptr read_osm_data_and_tree(const std::string& geoindex_filename_pre, const std::string& osm_bin_filename, volm_osm_objects& osm_objs, double& min_size);

  //: load a geotiff file (usually .tif) and read its ortho camera info from its header, puts a dummy lvcs to vpgl_geo_cam object so lvcs is not valid
  static void load_geotiff_image(const std::string& filename, volm_img_info& info, bool load_cam_from_name = false);

  //: load a satellite height map and read its ortho camera from image header, if image header is missing, load associated camera file
  static bool load_satellite_height_map(std::string const& filename, volm_img_info& info, bool const& load_cam_from_file = false);

  //: uses glob: folder//ASTGTM2_*/*_dem.tif
  static void load_aster_dem_imgs(std::string const& folder, std::vector<volm_img_info>& infos);

  //: uses glob: folder//scene_*/*.tif and folder//scene_*/*.tfw
  static bool load_satellite_height_imgs(std::string const& folder, std::vector<volm_img_info>& infos, bool const& load_cam_from_file = true, std::string const& keywords = "");

  //: uses glob: folder//Geocover*.tif
  static void load_geocover_imgs(std::string const& folder, std::vector<volm_img_info>& infos);

  //: uses glob: folder//Urextent*.tif
  static void load_urban_imgs(std::string const& folder, std::vector<volm_img_info>& infos);

  static bool find_min_max_height(vgl_point_2d<double>& lower_left, vgl_point_2d<double>& upper_right, std::vector<volm_img_info>& infos, double& min, double& max);

  //: use the following method to get the multiplier for conversion of meters to degrees, uses vpgl_lvcs internally
  static double meter_to_seconds(double lat, double lon);

  //: returns the points from the list that lie inside the box and also containes the intersection points
  static bool line_inside_the_box(vgl_box_2d<double> const& bbox, std::vector<vgl_point_2d<double> > const& line, std::vector<vgl_point_2d<double> >& line_in);

  //: find junctions on a road from a road network
  static bool search_junctions(std::vector<vgl_point_2d<double> > const& road, volm_land_layer const& road_prop,
                               std::vector<std::vector<vgl_point_2d<double> > > net, std::vector<volm_land_layer> net_props,
                               std::vector<vgl_point_2d<double> >& cross_pts, std::vector<volm_land_layer>& cross_props, std::vector<volm_land_layer>& cross_geo_props);

};

#endif // volm_io_tools_h_
