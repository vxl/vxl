// This is contrib/brl/bseg/boxm2/volm/exe/boxm2_volumetric_pinpoint.cxx
//:
// \file
// \executable to generate pin-pointed candidate regions given the volm_matcher score binary
// \author Yi Dong
// \date May 23, 2014

#include <iostream>
#include <map>
#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vnl/vnl_math.h>
#include <volm/volm_candidate_list.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>

static void error_report(std::string const& error_file, std::string const& error_msg)
{
  std::cerr << error_msg;
  volm_io::write_post_processing_log(error_file, error_msg);
}

// check whether the given point is inside the polygon (avoid using polygon contain method because we may have overlapped sheets)
static bool is_contained(vgl_polygon<double> const& polygon, vgl_point_3d<double> const& pt);
// generate a circle (points of lat/lon) given the center (lat/lon) and the radius value (in meters)
static bool generate_pin_point_circle(vgl_point_3d<double> const& center, double const& radius, std::vector<vgl_point_2d<double> >& circle);
// obtain camera angles from camera space and camera id
static bool generate_camera_angles(const volm_camera_space_sptr& cam_space, unsigned const& ni, unsigned const& nj, std::vector<unsigned> cam_ids,
                                   std::vector<cam_angles>& top_cameras, std::vector<double>& right_fovs);

class pin_pt_loc
{
public:
  // constructor
  pin_pt_loc() : loc_(vgl_point_3d<double>(0.0,0.0,0.0)), cam_id_(0) {}
  pin_pt_loc(vgl_point_3d<double> const& loc, unsigned const& cam_id) : loc_(loc), cam_id_(cam_id) {}
  // destructor
  ~pin_pt_loc() = default;

  vgl_point_3d<double> loc_;
  unsigned cam_id_;
  void print()
  { std::cout << " loc = [" << std::setprecision(5) << std::fixed << loc_.x() << ", "
                          << std::setprecision(5) << std::fixed << loc_.y() << "], cam_id = " << cam_id_ << '\n';
  }
};

//typedef std::multimap<float, pin_pt_loc, std::greater<float> > mymap;

int main(int argc, char** argv)
{
  vul_arg<std::string> out_kml("-out", "output kml file which stores the pin-pointed locations","");
  vul_arg<std::string> world_str("-world", "ROI world string, can be desert/coast/Chile/India/Jordan/Philippines/Taiwan","");
  vul_arg<std::string> score_folder("-score", "folder where score binaries reside", "");
  vul_arg<bool>       is_cam("-is_cam", "option to choose whether we write the camera into kml as photo overlay", false);
  vul_arg<std::string> cam_bin("-cam", "camera space binary","");
  vul_arg<std::string> query_img("-img", "query image size", "");
  vul_arg<std::string> geo_folder("-geo", "geo location database", "");
  vul_arg<std::string> candidate_list("-cand", "candidate list used during matching for search space reduction, if existed","");
  vul_arg<double>     radius("-radius", "pin-point circle radius (in meter)", 100.0);
  vul_arg<unsigned>   num_top_locs("-num-locs", "number of desired pinning points", 100);
  vul_arg<std::string> score_str("-score-str", "string for different score binary filename", "ps_1");
  vul_arg_parse(argc, argv);

  // input check
  if (out_kml().compare("") == 0 || world_str().compare("") == 0 || score_folder().compare("") == 0 || geo_folder().compare("") == 0)
  {
    vul_arg_display_usage_and_exit();  return volm_io::EXE_ARGUMENT_ERROR;
  }
  if (is_cam()) {
    if (cam_bin().compare("") == 0 || query_img().compare("") == 0) {
      std::cerr << " to generate camera photo overlay, query image and camera space binary is required!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
  }

  std::string log_file = vul_file::strip_extension(out_kml()) + ".xml";
  std::stringstream log;
  std::cout << "log_file = " << log_file << std::endl;
  std::cout << "out_kml = " << out_kml() << std::endl;
  std::cout << "score_folder = " << score_folder() << std::endl;
  std::cout << "camera space = " << cam_bin() << std::endl;
  std::cout << "geo_folder = " << geo_folder() << std::endl;
  std::cout << "radius = " << radius() << std::endl;
  std::cout << "num_top_locs = " << num_top_locs() << std::endl;

  // generate tiles
  std::vector<volm_tile> tiles;
  if (world_str().compare("desert") == 0)            tiles = volm_tile::generate_p1_wr1_tiles();
  else if (world_str().compare("coast") == 0)        tiles = volm_tile::generate_p1_wr2_tiles();
  else if (world_str().compare("Chile") == 0)        tiles = volm_tile::generate_p1b_wr1_tiles();
  else if (world_str().compare("India") == 0)        tiles = volm_tile::generate_p1b_wr2_tiles();
  else if (world_str().compare("Jordan") == 0)       tiles = volm_tile::generate_p1b_wr3_tiles();
  else if (world_str().compare("Philippines") == 0)  tiles = volm_tile::generate_p1b_wr4_tiles();
  else if (world_str().compare("Taiwan") == 0)       tiles = volm_tile::generate_p1b_wr5_tiles();
  else {
    log << "ERROR: unknown world region, should be \" desert, coast, Chile, India, Jordan, Philippines, Taiwan\"\n";
    error_report(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // check whether we have candidate list for the given query
  bool is_candidate = false;
  vgl_polygon<double> cand_poly;
  if (vul_file::exists(candidate_list())) {
    is_candidate = true;
    cand_poly = bkml_parser::parse_polygon(candidate_list());
    std::cout << "parse candidate list from file: " << candidate_list() << std::endl;
    std::cout << "number of sheet in the candidate poly: " << cand_poly.num_sheets() << std::endl;
  }

  // loop over each tile to load the score and sort them
  std::cout << "Start to sort all matched locations based on their matching score" << std::endl;
  std::multimap<float, pin_pt_loc, std::greater<float> > score_map;
  unsigned cnt=0;
  for (unsigned t_idx = 0; t_idx < tiles.size(); t_idx++) {
    volm_tile tile = tiles[t_idx];
    // load the geo location database
    std::stringstream file_name_pre;
    file_name_pre << geo_folder() << "geo_index_tile_" << t_idx;
    // no geo location for current tile, skip
    if (!vul_file::exists(file_name_pre.str() + ".txt"))
      continue;
    float min_size;
    volm_geo_index_node_sptr hyp_root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
    volm_geo_index::read_hyps(hyp_root, file_name_pre.str());
    if (is_candidate)
      volm_geo_index::prune_tree(hyp_root, cand_poly);
    std::vector<volm_geo_index_node_sptr> leaves;
    volm_geo_index::get_leaves_with_hyps(hyp_root, leaves);
    std::cout << "For tile " << t_idx << ", location database is loaded with " << leaves.size() << " leaves having locations" << std::endl;
    // load score binary from output folder
    std::stringstream score_bin;
    score_bin << score_folder() << score_str() << "_scores_tile_" << t_idx << ".bin";
    if (!vul_file::exists(score_bin.str())) {
      std::cout << " score file: " << score_bin.str() << " does NOT exist, ignore..." << std::endl;
      continue;
    }
    cnt++;
    std::vector<volm_score_sptr> scores;
    volm_score::read_scores(scores, score_bin.str());
    // sort all the score data
    std::cout << scores.size() << " scores is loaded from " << score_bin.str() << std::endl;
    auto total_ind = (unsigned)scores.size();
    for (unsigned i = 0; i < total_ind; i++) {
      vgl_point_3d<double> h_pt = leaves[scores[i]->leaf_id_]->hyps_->locs_[scores[i]->hypo_id_];
      std::pair<float, pin_pt_loc> tmp_pair(scores[i]->max_score_, pin_pt_loc(h_pt, scores[i]->max_cam_id_));
      score_map.insert(tmp_pair);
    }
  }

  // generate pin point locations from sorted score map
  std::cout << "There are " << score_map.size() << " locations has been matched from " << cnt << " tiles" << std::endl;
#if 0
  cnt = 0;
  for (std::map<float, pin_pt_loc>::iterator mit = score_map.begin();  mit != score_map.end();  ++mit) {
    std::cout << "rank: " << cnt++ << ", score = " << mit->first;  mit->second.print();
  }
#endif
  std::cout << "Start to generate " << num_top_locs() << " pin points out of " << score_map.size() << " matched locations..." << std::flush << std::endl;
  vgl_polygon<double> pin_pt_poly;  // contains all the pin point region.  Each sheet represents a pin-pointed circle in wgs84 unit
  std::vector<vgl_point_2d<double> > top_locs;
  std::vector<float> likelihood;
  std::vector<unsigned> cam_ids;
  auto mit = score_map.begin();
  while (pin_pt_poly.num_sheets() < num_top_locs() && mit != score_map.end())
  {
    // check whether the location has been in the pin-pointed region
    if (is_contained(pin_pt_poly, mit->second.loc_)) {
      //std::cout << "location " << mit->second.loc_ << " is inside the created pinpoint region, ignored..." << std::endl;
      ++mit;
      continue;
    }
    // generate a pin-point region for current location
    likelihood.push_back(mit->first);
    cam_ids.push_back(mit->second.cam_id_);
    top_locs.emplace_back(mit->second.loc_.x(), mit->second.loc_.y());
    std::vector<vgl_point_2d<double> > circle;
    if (!generate_pin_point_circle(mit->second.loc_, radius(), circle))
    {
      log << "ERROR: generating pin point circle for location " << mit->second.loc_ << " failed!\n";
      error_report(log_file, log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    pin_pt_poly.push_back(circle);
    ++mit;
  }

  std::cout << pin_pt_poly.num_sheets() << " pin points are created out of " << score_map.size() << " matcher locations" << std::endl;

  std::vector<cam_angles> top_cameras;
  std::vector<double> right_fovs;
  if (is_cam())
  {
    // load camera space
    if (!vul_file::exists(cam_bin())) {
      log << "ERROR: can not find camera space binary: " << cam_bin() << '\n';
      error_report(log_file, log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    vsl_b_ifstream cam_ifs(cam_bin());
    volm_camera_space_sptr cam_space = new volm_camera_space();
    cam_space->b_read(cam_ifs);
    cam_ifs.close();
    // load query image
    vil_image_resource_sptr query_image = vil_load_image_resource(query_img().c_str());
    unsigned ni = query_image->ni();  unsigned nj = query_image->nj();
    if (!generate_camera_angles(cam_space, ni, nj, cam_ids, top_cameras, right_fovs)) {
      log << "ERROR: obtain camera angle from camera space failed!, check camera ids in score binary\n";
      error_report(log_file, log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
  }
  else {
    for (unsigned i = 0; i < pin_pt_poly.num_sheets(); i++) {
      top_cameras.emplace_back(0.0, 1.0, 0.0, 90.0);
      right_fovs.push_back(1.0);
    }
  }

  // write the pin-point to kml file
  std::ofstream ofs_kml(out_kml().c_str());
  std::string kml_name = vul_file::strip_extension(vul_file::strip_directory(out_kml()));
  volm_candidate_list::open_kml_document(ofs_kml, kml_name, (float)num_top_locs());
  unsigned rank = 0;
  for (unsigned i = 0; i < pin_pt_poly.num_sheets(); i++)
    volm_candidate_list::write_kml_regions(ofs_kml, pin_pt_poly[i], top_locs[i], top_cameras[i], right_fovs[i], likelihood[i], rank++);
  volm_candidate_list::close_kml_document(ofs_kml);

  return volm_io::SUCCESS;
}

bool is_contained(vgl_polygon<double> const& polygon, vgl_point_3d<double> const& pt)
{
  vgl_point_2d<double> pt_2d(pt.x(), pt.y());
  for (unsigned i = 0; i < polygon.num_sheets(); i++)
  {
    vgl_polygon<double> single_sheet(polygon[i]);
    if (single_sheet.contains(pt_2d))
      return true;
  }
  return false;
}

bool generate_pin_point_circle(vgl_point_3d<double> const& center, double const& radius, std::vector<vgl_point_2d<double> >& circle)
{
  // construct a local lvcs
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(center.y(), center.x(), 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double deg_to_rad = vnl_math::pi/180.0;
  double d_theta = 6 * deg_to_rad;
  double theta = 0;
  while (theta < vnl_math::twopi)
  {
    double dx = radius * std::cos(theta);
    double dy = radius * std::sin(theta);
    double lon ,lat, gz;
    lvcs->local_to_global(dx, dy, 0.0, vpgl_lvcs::wgs84, lon, lat, gz);
    circle.emplace_back(lon, lat);
    theta += d_theta;
  }
  return true;
}

bool generate_camera_angles(const volm_camera_space_sptr& cam_space, unsigned const& ni, unsigned const& nj, std::vector<unsigned> cam_ids,
                            std::vector<cam_angles>& top_cameras, std::vector<double>& right_fovs)
{
  top_cameras.clear();
  right_fovs.clear();
  for (unsigned int cam_id : cam_ids)
  {
    cam_angles cam_ang = cam_space->camera_angles(cam_id);
    double head = (cam_ang.heading_ < 0) ? cam_ang.heading_ + 360.0 : cam_ang.heading_;
    double tilt = (cam_ang.tilt_ < 0) ? cam_ang.tilt_ + 360 : cam_ang.tilt_;
    double roll;
    if (cam_ang.roll_ * cam_ang.roll_ < 1E-10) roll = 0;
    else                                       roll = cam_ang.roll_;
    double tfov = cam_ang.top_fov_;
    double tv_rad = tfov / vnl_math::deg_per_rad;
    double ttr = std::tan(tv_rad);
    double rfov = std::atan( ni * ttr / nj) * vnl_math::deg_per_rad;
    top_cameras.emplace_back(roll, tfov, head, tilt);
    right_fovs.push_back(rfov);
  }
  return true;
}
