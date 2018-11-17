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


#include <iostream>
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <volm/volm_geo_index2_sptr.h>
#include <vsl/vsl_binary_io.h>
#include <brad/brad_image_metadata.h>
#include <vil/vil_image_view.h>
#include <volm/volm_satellite_resources_sptr.h>

class volm_satellite_resource
{
  public:
    volm_satellite_resource() : full_path_(""), name_(""), pair_(""), meta_(nullptr) {}

  // ===========  binary I/O ================
  short version() const { return 0; }
  void b_write(vsl_b_ostream& os) const;
  void b_read(vsl_b_istream& is);

  public:
    std::string full_path_;
    std::string name_;
    std::string pair_;   // if this is a PAN img, save its MULTI pair if available, save only the name
    brad_image_metadata_sptr meta_;
};

// traverse the path recursively and construct a satellite resource for each .nitf file in the folder for a given lat-lon bounding box
class volm_satellite_resources : public vbl_ref_count
{
  public:
    //: default constructor
    volm_satellite_resources() = default;

    //: x is lon and y is lat in the bbox, construct bbox with min point to be lower left and max to be upper right and as axis aligned with North-East
    volm_satellite_resources(vgl_box_2d<double>& bbox, double min_size = 1.0, bool eliminate_same = false);

    //: traverse the given path recursively and add each satellite resource
    void add_path(std::string path);

    int resources_size() { return resources_.size(); }

    //: get a list of ids in the resources_ list that overlap the given rectangular region
    void query(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, const std::string& band_str, std::vector<unsigned>& ids, double gsd_thres);
    //: query the resources in the given box and output the full paths to the given file
    bool query_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, unsigned& cnt, std::string& out_file, const std::string& band_str, double gsd_thres);

    //: query the resources in the given box and output the full paths of randomly selected seeds to the given file,
    //  the order of satellites to select seeds from: GeoEye1, WorldView2, WorldView1 and then any others
    bool query_seeds_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, int n_seeds, unsigned& cnt, std::string& out_file, std::string& band_str, double gsd_thres);

    //: get a list of pairs of ids in the resources_ list that are taken a few minutes apart from each other
    unsigned query_pairs(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, std::string& sat_name, float GSD_threshold, std::vector<std::pair<unsigned, unsigned> >& ids);
    //: query the resources in the given box and output the full paths of pairs to the given file
    bool query_pairs_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, float GSD_threshold, unsigned& cnt, std::string& out_file, std::string& sat_name);

    void rasterize(const vgl_polygon<double> &bounds, vil_image_view<bool> &mask);

    void rasterize(const vgl_box_2d<double> &bbox_clipped, const vgl_polygon<double> &bounds, vil_image_view<bool> &mask);

    vgl_polygon<double> calculate_convex_hull(const vil_image_view<bool>& mask, unsigned off_x=0, unsigned off_y=0);

    double compactness(const vgl_polygon<double>& poly, const vil_image_view<bool>& mask);

    void convert_to_local_footprints(vpgl_lvcs_sptr& lvcs, std::vector<vgl_polygon<double> >& lvcs_footprints,
        const std::vector<vgl_polygon<double> >& footprints, float downsample_factor=1.0);

    void convert_to_global_footprints(std::vector<vgl_polygon<double> >& footprints, const vpgl_lvcs_sptr& lvcs,
        const std::vector<vgl_polygon<double> >& lvcs_footprints, float downsample_factor);

    // rasterise the footprints of each nitf into a heatmap
    void compute_footprints_heatmap(vil_image_view<unsigned>& heatmap, vgl_box_2d<double>& image_window,
        const std::vector<vgl_polygon<double> >& footprints);

    void query_resources(std::vector<vgl_polygon<double> >& footprints, std::vector<unsigned>& footprint_ids,
        const volm_satellite_resources_sptr& res, const std::string& kml_file, const std::string& band="PAN", double gsd_thres=10.0);

    void highly_overlapping_resources(std::vector<std::string>& overlapping_res, const volm_satellite_resources_sptr& res,
        const std::string& kml_file, float downsample_factor, const std::string& band="PAN", double gsd_thres=10.0);

    void highly_overlapping_resources(std::vector<unsigned>& overlapping_ids, const std::vector<vgl_polygon<double> >& footprints,
        float downsample_factor);

    void highly_intersecting_resources(std::vector<std::string>& overlapping_res, const volm_satellite_resources_sptr& res,
        const std::string& kml_file, int k=3, int l=5, const std::string& band="PAN", double gsd_thres=10.0);

    void highly_intersecting_resources(std::vector<unsigned>& overlapping_ids,
        const std::vector<vgl_polygon<double> >& footprints, unsigned k=3, unsigned l=5);

    void ind_combinations(std::vector<std::vector<unsigned> >& combs, unsigned N, unsigned K);

    //: return the full path of a satellite image given its name, if not found returns empty string
    std::pair<std::string, std::string> full_path(const std::string& name);

    //: find the image PAN/MULTI band pair given the satellite image name
    //: Note the PAN and MULTI band generally has shifted footprint and tolerance here is used to set the threshold that PAN/MULTI band needs to have footprint shift less than
    //  10.0 meters (experiments shows a 30.0 meter for most of the pairs)
    std::string find_pair(std::string const& name, double const& tol = 10.0);

    // ===========  binary I/O ================
    short version() const { return 0; }
    void b_write(vsl_b_ostream& os) const;
    void b_read(vsl_b_istream& is);

    //: use the corresponding global reliability for each satellite when setting weights for camera correction
    static std::map<std::string, float> satellite_geo_reliability;

protected:
    void add_resource(const std::string& name);
    void construct_tree();
    //: add the resources in the resources_ vector to the tree
    void add_resources(unsigned start, unsigned end);
    //: check the time difference between two images (images have time difference less than given time interval (in seconds) are treated as same time)
    bool same_time(volm_satellite_resource const& res_a, volm_satellite_resource const& res_b, float const& t_diff_in_sec = 10);
    //: compare the lat, lon bounding boxes of satellite resource. treat as Euclidean coordinate system
    bool same_extent(volm_satellite_resource const& res_a, volm_satellite_resource const& res_b, double const& tol = 1.0);
    //: compare the viewing elevation and azimuth of two satellite resource
    bool same_view(volm_satellite_resource const& res_a, volm_satellite_resource const& res_b, double const& tol = 0.1);

  public:
    std::vector<volm_satellite_resource> resources_;
    volm_geo_index2_node_sptr root_;
    double min_size_;
    vgl_box_2d<double> bbox_;
    bool eliminate_same_;

};

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, volm_satellite_resources const &tc);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, volm_satellite_resources &tc);

void vsl_print_summary(std::ostream &os, const volm_satellite_resources &tc);

void vsl_b_read(vsl_b_istream& is, volm_satellite_resources* tc);

void vsl_b_write(vsl_b_ostream& os, const volm_satellite_resources* &tc);

void vsl_print_summary(std::ostream& os, const volm_satellite_resources* &tc);

void vsl_b_read(vsl_b_istream& is, volm_satellite_resources_sptr& tc);

void vsl_b_write(vsl_b_ostream& os, const volm_satellite_resources_sptr &tc);

void vsl_print_summary(std::ostream& os, const volm_satellite_resources_sptr &tc);


#endif
