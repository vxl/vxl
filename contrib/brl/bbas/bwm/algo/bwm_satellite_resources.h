#ifndef bwm_satellite_resources_h_
#define bwm_satellite_resources_h_
//:
// \file
//  A class in the form of a quad tree to store/access all the satellite image resources 
//  other utilities using this class, e.g. to generate site.xml files to open in bwm_main
//
// Author     Ozge C. Ozcanli
// \date      Aug 09, 2013
//
//======================================================================


#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vgl/vgl_box_2d.h>
#include <volm/volm_geo_index2_sptr.h>

class bwm_satellite_resource
{
  public:
    bwm_satellite_resource() : full_path_(""), name_(""), cloud_coverage_(-1.0f), full_path_mul_pair_(""), sat_name_("")  {}

  public:
    vcl_string full_path_;
    vcl_string name_;
    float cloud_coverage_;
    vgl_box_2d<double> extent_;  // using wgs84
    vcl_string full_path_mul_pair_;
    vcl_string sat_name_; // e.g. ikonos, wv2, etc.
};

// traverse the path recursively and construct a satellite resource for each .nitf file in the folder for a given lat-lon bounding box
class bwm_satellite_resources : public vbl_ref_count
{
  public:
    bwm_satellite_resources(vcl_string path, double lower_left_lat, double lower_left_lon, double upper_right_lat, double upper_right_lon);

  public:
    volm_geo_index2_node_sptr root_;

};


#endif
