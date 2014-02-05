//This is brl/bbas/volm/volm_io.h
#ifndef volm_io_h_
#define volm_io_h_
//:
// \file
// \brief This file holds several volm I/O oriented classes
//
// \author Ozge C. Ozcanli
// \date September 18, 2012
// \verbatim
//  Modifications
//   Yi Dong - October 25 2012 - modify the read_labelme to construct depth_map_scene from xml file
// \endverbatim

#include <vcl_string.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <vcl_set.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include "volm_category_io.h"

class volm_weight;
//: A class to hold xml file io methods for volumetric matchers
// Units are in meters
class volm_orient_table
{
 public:
  static vcl_map<vcl_string, depth_map_region::orientation> ori_id;

  // list of the possible values for indexed orientations from the reference world
  static vcl_map<int, vil_rgb<vxl_byte> >  ori_index_colors;
};

class volm_attributes
{
 public:
  volm_attributes() : id_(0), name_(""), color_(vil_rgb<vxl_byte>(0,0,0)) {}
  volm_attributes(unsigned char id, vcl_string name, vil_rgb<vxl_byte> color): id_(id), name_(name), color_(color) {}
  bool contains(vcl_string name);
  unsigned char id_;
  vcl_string name_;
  vil_rgb<vxl_byte> color_;
};

class volm_label_table
{
 public:
  static unsigned number_of_labels_;
  enum label_values {INVALID = 0, SAND = 31, WATER = 11, DEVELOPED_LOW = 22, DEVELOPED_MED = 23, DEVELOPED_HIGH = 24, DEVELOPED_OPEN = 21, WETLAND = 95, WOODY_WETLAND = 90, BUILDING = 100, FORT = 106, PIER = 114, BUILDING_TALL = 119};
  static vcl_map<int, volm_attributes > land_id ;
  static vcl_string land_string(unsigned char id);
  //: pass the id of the class labeled in the query (volm_attribute.id_)
  static vil_rgb<vxl_byte> get_color(unsigned char id);
  static unsigned char get_id_closest_name(vcl_string name);
  static unsigned compute_number_of_labels();
};

//: A class to transfer bae labeled object category to volm_label_table and orientation
class volm_category_attribute
{
 public:
  volm_category_attribute() : lnd_("invalid"), ori_("porous"), is_active_(0) {}
  volm_category_attribute(vcl_string lnd, vcl_string ori, unsigned is_active) : lnd_(lnd), ori_(ori), is_active_(is_active) {}
  static void read_category(vcl_map<vcl_string, volm_category_attribute> & category_table, vcl_string fname);
  vcl_string lnd_;
  vcl_string ori_;
  unsigned is_active_;
};


//: A class to hold the fallback categories for the labelled landtype id
class volm_fallback_label
{
 public:
  //: key -- assigned land_id in depth_map_scene, element -- an array with 4-elements defines the possible fallback land_id for given land type
  static vcl_map<unsigned char, vcl_vector<unsigned char> > fallback_id;
  static vcl_map<unsigned char, vcl_vector<float> > fallback_weight;
  static void size(unsigned char& fallback_size) { fallback_size = (unsigned char)volm_fallback_label::fallback_id[0].size(); }
  static void print_id(unsigned char id)
  {
    vcl_cout << '[';
    for (vcl_vector<unsigned char>::iterator vit = volm_fallback_label::fallback_id[id].begin(); vit != volm_fallback_label::fallback_id[id].end(); ++vit)
      //vcl_cout << volm_label_table::land_string(*vit) << ", ";
      vcl_cout << volm_osm_category_io::volm_land_table[*vit].name_ << ", ";
    vcl_cout << ']';
  }
  static void print_wgt(unsigned char id)
  {
    vcl_cout << '[';
    for (vcl_vector<float>::iterator vit = volm_fallback_label::fallback_weight[id].begin(); vit != volm_fallback_label::fallback_weight[id].end(); ++vit)
      vcl_cout << vcl_setprecision(3) << *vit << ' ';
    vcl_cout << ']';
  }
  static void print_fallback_table();
};

void volm_io_extract_values(unsigned char combined_value, unsigned char& orientation_value, unsigned char& label_value);

class volm_io_expt_params
{
 public:
  float fov_inc, tilt_inc, roll_inc, head_inc, vmin, solid_angle, dmax, cap_angle, point_angle, top_angle, bottom_angle;
  void read_params(vcl_string params_file);
};

class volm_io
{
 public:
  //: warning: always add to the end of this error code list, python script on the server has a hard copy of some of these values, they should not be changed
  enum VOLM_ERROR_CODES {SUCCESS, EXE_ARGUMENT_ERROR, EXE_RUNNING, CAM_FILE_IO_ERROR, MATCHER_EXE_STARTED, MATCHER_EXE_FINISHED, MATCHER_EXE_FAILED, 
                         COMPOSE_STARTED, DEPTH_SCENE_FILE_IO_ERROR, LABELME_FILE_IO_ERROR, GEO_INDEX_FILE_MISSING, 
                         SCORE_FILE_MISSING, EXE_STARTED, EXE_MATCHER_FAILED, COMPOSE_HALT, 
                         PRE_PROCESS_STARTED, PRE_PROCESS_FAILED, PRE_PROCESS_FINISHED, 
                         POST_PROCESS_FAILED, POST_PROCESS_HALT};

  //: scale value is STRONG_POSITIVE-STRONG_NEGATIVE
  enum VOLM_IMAGE_CODES {UNEVALUATED = 0, STRONG_NEGATIVE = 1, UNKNOWN = 127, STRONG_POSITIVE = 255, SCALE_VALUE = 254};

  static bool write_status(vcl_string out_folder, int status_code, int percent=0, vcl_string log_message = "", vcl_string status_file="status.xml");
  static bool write_log(vcl_string out_folder, vcl_string log);
  static bool write_composer_log(vcl_string out_folder, vcl_string log);
  static bool write_post_processing_log(vcl_string log_file, vcl_string log);

  //: return true if MATCHER_EXE_FINISHED, otherwise return false
  static bool check_matcher_status(vcl_string out_folder);

  static bool read_camera(vcl_string kml_file,
                          unsigned const& ni, unsigned const& nj,
                          double& heading,   double& heading_dev,
                          double& tilt,      double& tilt_dev,
                          double& roll,      double& roll_dev,
                          double& top_fov,   double& top_fov_dev,
                          double& altitude, double& lat, double& lon);

  static bool read_labelme(vcl_string xml_file, vcl_string category_file, depth_map_scene_sptr& depth_scene, vcl_string& img_category);

  //: parser for reading xml tags to create depth_map_scene
  static bool read_query_tags(vcl_string xml_file,
                              depth_map_scene_sptr& depth_scene,
                              vcl_vector<volm_weight>& weights,
                              vcl_string& world_region,
                              unsigned& img_ni,
                              unsigned& img_nj,
                              vcl_string& query_name);

  //: piecewise linear s.t. [1,127) -> [0,t), [127,255] -> [t,1]
  static float scale_score_to_0_1(unsigned char pix_value, float threshold);
  //: piecewise non-linear s.t. [1,255] -> [0,1]
  static float scale_score_to_0_1_sig(float const& k1, float const &ku, float const& threshold, unsigned char score);
  //: piecewise linear s.t. [0,t) -> [1,127), [t,1] -> [127,255]"
  static unsigned char scale_score_to_1_255(float threshold, float score);
  //: piecewise non-linear s.t. using sigmoid function 255/(1+exp(-k(s-t)))
  static unsigned char scale_score_to_1_255_sig(float const& kl, float const & ku, float const& threshold, float const& score);
  //: piecewise linear s.t. [0,t) -> [1,63), [t,1] -> [63,127]"
  static unsigned char scale_score_to_1_127(float threshold, float score);

  //: read the specific polygon format given by python parser for candidate list processing
  static void read_polygons(vcl_string poly_file, vgl_polygon<double>& out);
  static void convert_polygons(vgl_polygon<double> const& in, vgl_polygon<float>& out);
  static void convert_polygons(vgl_polygon<float> const& in, vgl_polygon<double>& out);

  static int read_gt_file(vcl_string gt_file, vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_pair<vcl_string, int>, vcl_string> > >& samples);
  static bool read_ray_index_data(vcl_string path, vcl_vector<unsigned char>& data);

  //: read the building footpring file
  static bool read_building_file(vcl_string file, vcl_vector<vcl_pair<vgl_polygon<double>, vgl_point_2d<double> > >& builds, vcl_vector<double>& heights);

  //: read the sme labels
  static bool read_sme_file(vcl_string file, vcl_vector<vcl_pair<vgl_point_2d<double>, int> >& objects);
  static bool write_sme_kml(vcl_string file, vcl_vector<vcl_pair<vgl_point_2d<double>, int> >& objects);
  static bool write_sme_kml_type(vcl_string file, vcl_string type_name, vcl_vector<vcl_pair<vgl_point_2d<double>, int> >& objects);
};

class volm_rationale
{
 public:
  float lat;
  float lon;
  float elev;
  unsigned index_id; // also index in score.bin and cam.bin files
  unsigned cam_id;   // index for camera in volm_query
  vcl_string index_file;
  vcl_string score_file;

  static bool write_top_matches(vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > >& top_matches, vcl_string& filename);
  static bool read_top_matches(vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > >& top_matches, vcl_string& filename);
};

bool operator>(const vcl_pair<float, volm_rationale>& a, const vcl_pair<float, volm_rationale>& b);

//: A class to store the highest score for each location
// * \p leaf_id     ----> id of the leaf in leaves vector
// * \p hypo_id     ----> local id of the hypothesis in the leaf
// * \p max_score_  ----> highest score for current location
// * \p max_cam_id_ ----> the camera id associated with the highest score
// * \p cam_id      ----> vector of camera ids whose score is higher than defined threshold

class volm_score : public vbl_ref_count
{
 public:
  volm_score () {}
  volm_score(unsigned leaf_id, unsigned hypo_id) : leaf_id_(leaf_id), hypo_id_(hypo_id) {}
  volm_score(unsigned const& leaf_id, unsigned const& hypo_id, float const& max_score, unsigned const& max_cam_id, vcl_vector<unsigned> const& cam_id)
    : leaf_id_(leaf_id), hypo_id_(hypo_id), max_score_(max_score), max_cam_id_(max_cam_id), cam_id_(cam_id) {}
  ~volm_score() {}
  unsigned leaf_id_;
  unsigned hypo_id_;
  float    max_score_;
  unsigned max_cam_id_;
  vcl_vector<unsigned> cam_id_;

  //: version
  unsigned version() const {return 1;}

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

  static void write_scores(vcl_vector<vbl_smart_ptr<volm_score> >& scores, vcl_string const& file_name);
  static void read_scores(vcl_vector<vbl_smart_ptr<volm_score> >& scores, vcl_string const& file_name);
};

//: A class to store the highest score for each location
// Each class defines the weight parameters for a given depth_map_region
class volm_weight
{
 public:
  volm_weight () {}
  volm_weight (vcl_string const& w_name, vcl_string const& w_typ, float const& w_ori, float const& w_lnd, float const& w_ord, float const& w_dst, float const w_obj)
    : w_name_(w_name), w_typ_(w_typ), w_ori_(w_ori), w_lnd_(w_lnd), w_ord_(w_ord), w_dst_(w_dst), w_obj_(w_obj) {}
  ~volm_weight() {}
  //: name of the depth_map_region
  vcl_string w_name_;
  //: type of the depth_map_region, i.e.: ground, sky or others
  vcl_string w_typ_;
  //: weight parameter for orientation attributes
  float w_ori_;
  //: weight parameter for land type
  float w_lnd_;
  //: weight parameter for relative order
  float w_ord_;
  //: weight parameter for minimum distance
  float w_dst_;
  //: weight parameter for current object relative to all other objects in the query_image
  float w_obj_;

  //: check validity of weight parameters
  static bool check_weight(vcl_vector<volm_weight> const& weight);
  //: functions that reads the weight parameters from weight_param.txt
  static void read_weight(vcl_vector<volm_weight>& weights, vcl_string const& file_name);
  //: functions that implements a default equal weight parameters, given the depth_map_scene
  static void equal_weight(vcl_vector<volm_weight>& weights, depth_map_scene_sptr dms);
};

#include <vbl/vbl_smart_ptr.h>
typedef vbl_smart_ptr<volm_score> volm_score_sptr;

#endif // volm_io_h_
