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
#include <vsl/vsl_binary_io.h>
#include <brad/brad_image_metadata.h>

class bwm_satellite_resource
{
  public:
    bwm_satellite_resource() : full_path_(""), name_(""), full_path_mul_pair_(""), meta_(0) {}

  public:
    vcl_string full_path_;
    vcl_string name_;
    vcl_string full_path_mul_pair_;
    brad_image_metadata_sptr meta_;
};

// traverse the path recursively and construct a satellite resource for each .nitf file in the folder for a given lat-lon bounding box
class bwm_satellite_resources : public vbl_ref_count
{
  public:
    bwm_satellite_resources(vcl_string path, double lower_left_lat, double lower_left_lon, double upper_right_lat, double upper_right_lon);

protected:
    void add_resource(vcl_string name);

  public:
    vcl_vector<bwm_satellite_resource> resources_;
    volm_geo_index2_node_sptr root_;

};

#include <bwm/algo/bwm_satellite_resources_sptr.h>

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bwm_satellite_resources const &tc);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bwm_satellite_resources &tc);

void vsl_print_summary(vcl_ostream &os, const bwm_satellite_resources &tc);

void vsl_b_read(vsl_b_istream& is, bwm_satellite_resources* tc);

void vsl_b_write(vsl_b_ostream& os, const bwm_satellite_resources* &tc);

void vsl_print_summary(vcl_ostream& os, const bwm_satellite_resources* &tc);

void vsl_b_read(vsl_b_istream& is, bwm_satellite_resources_sptr& tc);

void vsl_b_write(vsl_b_ostream& os, const bwm_satellite_resources_sptr &tc);

void vsl_print_summary(vcl_ostream& os, const bwm_satellite_resources_sptr &tc);


#endif
