#ifndef volm_satellite_resources_h_
#define volm_satellite_resources_h_
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

class volm_satellite_resource
{
  public:
    volm_satellite_resource() : full_path_(""), name_(""), pair_(""), meta_(0) {}

  // ===========  binary I/O ================
  short version() const { return 0; }
  void b_write(vsl_b_ostream& os) const;
  void b_read(vsl_b_istream& is);

  public:
    vcl_string full_path_;
    vcl_string name_;
    vcl_string pair_;   // if this is a PAN img, save its MULTI pair if available, save only the name
    brad_image_metadata_sptr meta_;
};

// traverse the path recursively and construct a satellite resource for each .nitf file in the folder for a given lat-lon bounding box
class volm_satellite_resources : public vbl_ref_count
{
  public:
    //: default constructor
    volm_satellite_resources() {}

    //: x is lon and y is lat in the bbox, construct bbox with min point to be lower left and max to be upper right and as axis aligned with North-East
    volm_satellite_resources(vgl_box_2d<double>& bbox, double min_size = 1.0);
    
    //: traverse the given path recursively and add each satellite resource
    void add_path(vcl_string path);
    
    int resources_size() { return resources_.size(); }

    //: get a list of ids in the resources_ list that overlap the given rectangular region
    void query(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, vcl_string& band_str, vcl_vector<unsigned>& ids, double gsd_thres);
    //: query the resources in the given box and output the full paths to the given file
    bool query_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, unsigned& cnt, vcl_string& out_file, vcl_string& band_str, double gsd_thres);

    //: query the resources in the given box and output the full paths of randomly selected seeds to the given file, 
    //  the order of satellites to select seeds from: GeoEye1, WorldView2, WorldView1 and then any others
    bool query_seeds_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, int n_seeds, unsigned& cnt, vcl_string& out_file, vcl_string& band_str, double gsd_thres);
    
    //: get a list of pairs of ids in the resources_ list that are taken a few minutes apart from each other
    void query_pairs(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, vcl_string& sat_name, vcl_vector<vcl_pair<unsigned, unsigned> >& ids);
    //: query the resources in the given box and output the full paths of pairs to the given file
    bool query_pairs_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, unsigned& cnt, vcl_string& out_file, vcl_string& sat_name);

    //: return the full path of a satellite image given its name, if not found returns empty string
    vcl_pair<vcl_string, vcl_string> full_path(vcl_string name);

    vcl_string find_pair(vcl_string const& name);

    // ===========  binary I/O ================
    short version() const { return 0; }
    void b_write(vsl_b_ostream& os) const;
    void b_read(vsl_b_istream& is);

    //: use the corresponding global reliability for each satellite when setting weights for camera correction
    static vcl_map<vcl_string, float> satellite_geo_reliability;  

protected:
    void add_resource(vcl_string name);
    void construct_tree();
    //: add the resources in the resources_ vector to the tree
    void add_resources(unsigned start, unsigned end);

  public:
    vcl_vector<volm_satellite_resource> resources_;
    volm_geo_index2_node_sptr root_;
    double min_size_;
    vgl_box_2d<double> bbox_;

};

#include <volm/volm_satellite_resources_sptr.h>

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, volm_satellite_resources const &tc);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, volm_satellite_resources &tc);

void vsl_print_summary(vcl_ostream &os, const volm_satellite_resources &tc);

void vsl_b_read(vsl_b_istream& is, volm_satellite_resources* tc);

void vsl_b_write(vsl_b_ostream& os, const volm_satellite_resources* &tc);

void vsl_print_summary(vcl_ostream& os, const volm_satellite_resources* &tc);

void vsl_b_read(vsl_b_istream& is, volm_satellite_resources_sptr& tc);

void vsl_b_write(vsl_b_ostream& os, const volm_satellite_resources_sptr &tc);

void vsl_print_summary(vcl_ostream& os, const volm_satellite_resources_sptr &tc);


#endif
