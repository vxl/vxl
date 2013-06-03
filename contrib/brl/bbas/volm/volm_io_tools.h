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

#include <vcl_string.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_intersection.h>

class volm_img_info 
{
public:
  volm_img_info() {}
  bool intersects(vgl_polygon<double> poly) { return vgl_intersection(bbox, poly); }
  bool intersects(vgl_box_2d<double> other) { return vgl_intersection(bbox, other).area() > 0; }
  void save_box_kml(vcl_string out_name);

  unsigned ni, nj;
  vpgl_geo_camera* cam;   // in UTM for NAIP imgs
  vgl_box_2d<double> bbox;   // in lon (x), lat (y).  lower left corner is (0,nj), upper right corner is (ni, 0)
  vcl_string name;
  vcl_string img_name;
};

class volm_io_tools 
{
public:

  static unsigned int northing; // will hard code to 0  // WARNING: north hard-coded

  static bool load_naip_img(vcl_string const& img_folder, vcl_string const& name, vpgl_lvcs_sptr& lvcs, volm_img_info& info, bool load_resource = false);

  static bool load_naip_imgs(vcl_string const& img_folder, vcl_vector<volm_img_info>& imgs, bool load_resource = false);

  static int load_lidar_img(vcl_string img_file, volm_img_info& info);

  static void load_lidar_imgs(vcl_string const& folder, vcl_vector<volm_img_info>& infos);

  static void load_nlcd_imgs(vcl_string const& folder, vcl_vector<volm_img_info>& infos);

};

#endif // volm_io_tools_h_
