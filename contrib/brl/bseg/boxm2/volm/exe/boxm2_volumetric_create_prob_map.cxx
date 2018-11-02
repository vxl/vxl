//:
// \file
// \executable to create probability map and evaluate ROI from volm_matcher scores
// Given a result (score) for test query image, generate paorbability map for given tiles
// and report the score for the ground truth loc, associated with its best camera
// \author Yi Dong
// \date Feb 18, 2013

#include <set>
#include <iostream>
#include <iomanip>
#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>


int main(int argc, char** argv)
{
  vul_arg<std::string> cam_bin("-cam", "camera psace which is use to give the camera giving the maximum score");
  vul_arg<std::string> img("-img", "query images, to get the image size", "");
  vul_arg<std::string> geo_hypo_folder("-hypo", "folder to read the geo hypotheses","");
  vul_arg<std::string> candidate_list("-cand", "candidate list for given query (txt file)", "");  // index -- candidate list file containing polygons
  vul_arg<std::string> out("-out", "job output folder", "");
  vul_arg<unsigned>   pass_id("-pass", "from pass 0 to pass 1", 1);
  vul_arg<std::string> world_region("-world", "ROI world region (can be desert, coast, Chile, India, Jordan, Philippines, Taiwan)", "");
  vul_arg<std::string> gt_file("-gt_locs", "file with the gt locs of all test cases", "");        // ground truth file -- used for testing purpose
  vul_arg<unsigned>   img_id("-id", "id of the test image", 1E6);                                // image id -- used for testing purpose
  vul_arg_parse(argc, argv);

  if (out().compare("") == 0 || geo_hypo_folder().compare("") == 0 || cam_bin().compare("") == 0 ||
      world_region().compare("") == 0 || img().compare("") == 0 || pass_id() > 2)
  {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::stringstream log;
  std::string log_file = out() + "/create_prob_map_log.xml";

  // check whether we have candidate list for this query
  bool is_candidate = false;
  vgl_polygon<double> cand_poly;
  std::cout << " candidate list = " <<  candidate_list() << std::endl;
  if ( candidate_list().compare("") != 0) {
    if (!vul_file::exists(candidate_list())) {
      log << " ERROR: can not fine candidate list file: " << candidate_list() << '\n';
      volm_io::write_post_processing_log(log_file, log.str());
      std::cerr << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    else {
      // parse polygon from kml
      is_candidate = true;
      cand_poly = bkml_parser::parse_polygon(candidate_list());
      std::cout << " candidate list is parsed from file: " << candidate_list() << std::endl;
      std::cout << " number of sheet in the candidate poly " << cand_poly.num_sheets() << std::endl;
    }
  }
  else {
    std::cout << " NO candidate list for this query image, full index space is considered" << std::endl;
    is_candidate = false;
  }

  // create tiles
  std::vector<volm_tile> tiles;
  if (world_region().compare("desert") == 0)
    tiles = volm_tile::generate_p1_wr1_tiles();
  else if (world_region().compare("coast") == 0)
    tiles = volm_tile::generate_p1_wr2_tiles();
  else if (world_region().compare("Chile")  == 0)
    tiles = volm_tile::generate_p1b_wr1_tiles();
  else if (world_region().compare("India") == 0)
    tiles = volm_tile::generate_p1b_wr2_tiles();
  else if (world_region().compare("Jordan") == 0)
    tiles = volm_tile::generate_p1b_wr3_tiles();
  else if (world_region().compare("Philippines") == 0)
    tiles = volm_tile::generate_p1b_wr4_tiles();
  else if (world_region().compare("Taiwan") == 0)
    tiles = volm_tile::generate_p1b_wr5_tiles();
  else {
    log << "ERROR: unknown world region, should be \" desert, coast, Chile, India, Jordan, Philippines, Taiwan\"\n";
    volm_io::write_post_processing_log(log_file, log.str());  std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // initialize the Prob_map image if the prob_map doesn't exist
  // if the image exists, load the image instead
  std::vector<vil_image_view<float> > tile_imgs;
  for (auto & tile : tiles) {
    std::string img_name = out() + "/" + "ProbMap_float_" + tile.get_string() + ".tif";
    if (vul_file::exists(img_name)) {
      // load the image
      vil_image_view<float> out_img = vil_load(img_name.c_str());
      tile_imgs.push_back(out_img);
    }
    else {
      // create the image
      vil_image_view<float> out_img(3601, 3601);
      out_img.fill(-1.0f);
      tile_imgs.push_back(out_img);
    }
  }

  // look for the location and camera which provides max_score
  vgl_point_3d<double> max_score_loc;
  unsigned max_score_cam_id;
  float max_score = 0.0f;

  for (unsigned i = 0; i < tiles.size(); i++) {
    volm_tile tile = tiles[i];
    // read in the volm_geo_index for tile i
    std::stringstream file_name_pre;
    file_name_pre << geo_hypo_folder() << "geo_index_tile_" << i;
    // no geolocation for tile i exists, continue
    if (!vul_file::exists(file_name_pre.str() + ".txt")) {
      continue;
    }
    float min_size;
    volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
    volm_geo_index::read_hyps(root, file_name_pre.str());
    if (is_candidate) {
      volm_geo_index::prune_tree(root, cand_poly);
    }
    std::vector<volm_geo_index_node_sptr> leaves;
    volm_geo_index::get_leaves_with_hyps(root, leaves);
    // load score binary from output folder if exists
    std::stringstream score_file;
    score_file << out() << "ps_1_scores_tile_" << i << ".bin";
    // continue if no score binary exists for this tile
    if (!vul_file::exists(score_file.str()))
      continue;
    std::vector<volm_score_sptr> scores;
    volm_score::read_scores(scores, score_file.str());
    // refill the image
    unsigned total_ind = scores.size();
    for (unsigned ii = 0; ii < total_ind; ii++) {
      vgl_point_3d<double> h_pt = leaves[scores[ii]->leaf_id_]->hyps_->locs_[scores[ii]->hypo_id_];

      // look for location and camera giving maximum score
      if (scores[ii]->max_score_ > max_score) {
        max_score = scores[ii]->max_score_;
        max_score_cam_id = scores[ii]->max_cam_id_;
        max_score_loc = h_pt;
      }
      unsigned u, v;
      if (tile.global_to_img(h_pt.x(), h_pt.y(), u, v)) {
#if 0
      std::cout << " total_ind = " << total_ind << " ii = " << ii << " leaf_id = " << scores[ii]->leaf_id_ << ", hypo_id = " << scores[ii]->hypo_id_
               << std::setprecision(10) << " lon = " << h_pt.x() << " , lat = " << h_pt.y() << ", pixel = (" << u << "," << v << ")"
               << ", score = " << scores[ii]->max_score_ << std::endl;
#endif
        if (u < tile.ni() && v < tile.nj()) {
          // check if this is the highest values for this pixel
          if (scores[ii]->max_score_ > tile_imgs[i](u,v))
            tile_imgs[i](u,v) = scores[ii]->max_score_;
        }
      }
    }
  } // end of tiles

#if 0
  // save the location and camera which give maximum score
  if (!vul_file::exists(cam_bin())) {
    log << "ERROR: can not find camera_space binary: " << cam_bin() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vsl_b_ifstream cam_ifs(cam_bin());
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(cam_ifs);
  cam_ifs.close();
  if (!vul_file::exists(img())) {
    log << "ERROR: can not find the test query image: " << img() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vil_image_view<vxl_byte> query_img = vil_load(img().c_str());

  cam_angles max_cam_ang = cam_space->camera_angles(max_score_cam_id);

  std::string cam_kml = out() + "/MaxScoreCamera.kml";
  std::ofstream ofs_kml(cam_kml.c_str());

  std::stringstream kml_name;
  kml_name << "p1a_test1_" << id();
  bkml_write::open_document(ofs_kml);

  double head = (max_cam_ang.heading_ < 0) ? max_cam_ang.heading_ + 360.0 : max_cam_ang.heading_;
  double tilt = (max_cam_ang.tilt_ < 0) ? max_cam_ang.tilt_ + 360 : max_cam_ang.tilt_;
  double roll;
  if (max_cam_ang.roll_ * max_cam_ang.roll_ < 1E-10) roll = 0;
  else                                               roll = max_cam_ang.roll_;
  double tfov = max_cam_ang.top_fov_;
  double tv_rad = tfov / vnl_math::deg_per_rad;
  double ttr = std::tan(tv_rad);
  double rfov = std::atan( query_img.ni() * ttr / query_img.nj() ) * vnl_math::deg_per_rad;

  bkml_write::write_photo_overlay(ofs_kml, kml_name.str(), max_score_loc.x(), max_score_loc.y(), cam_space->altitude(),
                                  head, tilt, roll, tfov, rfov);
  bkml_write::close_document(ofs_kml);
  ofs_kml.close();
#endif

  // save the ProbMap image
  for (unsigned i = 0; i < tiles.size(); i++) {
    std::string img_name = out() + "/" + "ProbMap_float_" + tiles[i].get_string() + ".tif";
    vil_save(tile_imgs[i], img_name.c_str());
  }

  if (!vul_file::exists(cam_bin())) {
    log << "ERROR: can not find camera_space binary: " << cam_bin() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vsl_b_ifstream cam_ifs(cam_bin());
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(cam_ifs);
  cam_ifs.close();
  cam_angles max_cam_ang = cam_space->camera_angles(max_score_cam_id);
  // output the ground truch score if gt location is given
  // read gt locations if exists
  if (vul_file::exists(gt_file())) {
    double gt_lon, gt_lat;
    gt_lon = 0.0;  gt_lat = 0.0;
    std::vector<std::pair<vgl_point_3d<double>, std::pair<std::pair<std::string, int>, std::string> > > samples;
    volm_io::read_gt_file(gt_file(), samples);
    if (img_id() != 1E6 && img_id() < samples.size()) {
      gt_lon = samples[img_id()].first.x();
      gt_lat = samples[img_id()].first.y();
    }
    if (gt_lon != 0.0 && gt_lat != 0.0) {
      for (unsigned i = 0; i < tiles.size(); i++) {
        unsigned u, v;
        if (tiles[i].global_to_img(gt_lon, gt_lat, u, v)) {
          if (u < tiles[i].ni() && v < tiles[i].nj()) {
            log << "\t GT location: " << img_id() << ", "
            << gt_lon << ", " << gt_lat << " is at pixel: "
            << u << ", " << v << " in tile " << i << " and has value: "
            << tile_imgs[i](u, v)
            << " max score for this test_img = " << max_score
            << " given by camera " << max_cam_ang.get_string()
            << " at location " << std::setprecision(12) << max_score_loc.x() << ", " << std::setprecision(12) << max_score_loc.y()
            << '\n';
            volm_io::write_post_processing_log(log_file, log.str());
            std::cout << log.str();
          }
        }
      }
    }
  }
  else {
    log << " max score for this test_img = " << max_score
        << " given by camera " << max_cam_ang.get_string()
        << " at location " << std::setprecision(12) << max_score_loc.x() << ", " << std::setprecision(12) << max_score_loc.y()
        << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cout << log.str();
  }

}

#if 0
// generate gt hypos
int main(int argc,  char** argv)
{
  vul_arg<std::string> cam_bin("-cam", "camera space which is used to give the camera which provides maximum score");
  vul_arg<std::string> img("-img", "query images, to get the image size", "");
  vul_arg<std::string> gt_file("-gt_locs", "file with the gt locs of all test cases", "");
  vul_arg<std::string> geo_hypo_folder("-hypo", "folder to read the geo hypotheses","");
  vul_arg<unsigned> zone_id("-zone", "zone_id", 0);
  vul_arg<std::string> candidate_list("-cand", "candidate list for given query (txt file)", "");  // index -- candidate list file containing polygons
  vul_arg<std::string> out("-out", "job output folder", "");
  vul_arg<unsigned> id("-id", "id of the test image", 6);
  vul_arg<unsigned> pass_id("-pass", "from pass 0 to pass 1", 1);
  vul_arg_parse(argc, argv);

  std::cout << "argc: " << argc << std::endl;
  std::stringstream log;
  std::string log_file = out() + "/create_prob_map_log.xml";
  if (out().compare("") == 0 ||
      geo_hypo_folder().compare("") == 0 ||
      gt_file().compare("") == 0 ||
      cam_bin().compare("") == 0 ||
      img().compare("") == 0 ||
      pass_id() > 2 ||
      zone_id() == 0 ||
      id() > 100)
  {
    log << "EXE_ARGUMENT_ERROR!\n";
    vul_arg_display_usage_and_exit();
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // read gt location, i.e., lat and lon
  if (!vul_file::exists(gt_file())) {
    log << "ERROR : can not find ground truth position file -->" << gt_file() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::vector<std::pair<vgl_point_3d<double>, std::pair<std::pair<std::string, int>, std::string> > > samples;
  unsigned int cnt = volm_io::read_gt_file(gt_file(), samples);
  if (id() >= cnt) {
    log << "ERROR: gt_file " << gt_file() << " does not contain test id: " << id() << "!\n";
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // check whether we have candidate list for this query
  bool is_candidate = false;
  vgl_polygon<double> cand_poly;
  std::cout << " candidate list = " <<  candidate_list() << std::endl;
  if ( candidate_list().compare("") != 0) {
    if (!vul_file::exists(candidate_list())) {
      log << " ERROR: can not fine candidate list file: " << candidate_list() << '\n';
      volm_io::write_post_processing_log(log_file, log.str());
      std::cerr << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    else {
      // parse polygon from kml
      is_candidate = true;
      cand_poly = bkml_parser::parse_polygon(candidate_list());
      std::cout << " candidate list is parsed from file: " << candidate_list() << std::endl;
      std::cout << " number of sheet in the candidate poly " << cand_poly.num_sheets() << std::endl;
    }
  }
  else {
    std::cout << " NO candidate list for this query image, full index space is considered" << std::endl;
    is_candidate = false;
  }

  // create tiles
  std::vector<volm_tile> tiles;
  if (samples[id()].second.second == "desert")
    tiles = volm_tile::generate_p1_wr1_tiles();
  else if (samples[id()].second.second == "coast")
    tiles = volm_tile::generate_p1_wr2_tiles();
  else if (samples[id()].second.second == "Chile")
    tiles = volm_tile::generate_p1b_wr1_tiles();
  else if (samples[id()].second.second == "India")
    tiles = volm_tile::generate_p1b_wr2_tiles();
  else if (samples[id()].second.second == "Jordan")
    tiles = volm_tile::generate_p1b_wr3_tiles();
  else if (samples[id()].second.second == "Phillippines")
    tiles = volm_tile::generate_p1b_wr4_tiles();
  else if (samples[id()].second.second == "Taiwan")
    tiles = volm_tile::generate_p1b_wr5_tiles();
  else {
    log << "ERROR: cannot find ROI for image id " << id() << "\n";
    volm_io::write_post_processing_log(log_file, log.str());  std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }


  // initialize the Prob_map image if the prob_map doesn't exist
  // if the image exists, load the image instead
  std::vector<vil_image_view<float> > tile_imgs;
  for (unsigned i = 0 ; i < tiles.size(); i++) {
    std::string img_name = out() + "/" + "ProbMap_float_" + tiles[i].get_string() + ".tif";
    if (vul_file::exists(img_name)) {
      // load the image
      vil_image_view<float> out_img = vil_load(img_name.c_str());
      tile_imgs.push_back(out_img);
    }
    else {
      // create the image
      vil_image_view<float> out_img(3601, 3601);
      out_img.fill(-1.0f);
      tile_imgs.push_back(out_img);
    }
  }

  // look for the location and camera which provides max_score
  vgl_point_3d<double> max_score_loc;
  unsigned max_score_cam_id;
  float max_score = 0.0f;

  for (unsigned i = 0; i < tiles.size(); i++) {
    volm_tile tile = tiles[i];
    // read in the volm_geo_index for tile i
    std::stringstream file_name_pre;
    file_name_pre << geo_hypo_folder() << "geo_index_tile_" << i;
    // no geolocation for tile i exists, continue
    if (!vul_file::exists(file_name_pre.str() + ".txt")) {
      continue;
    }

    float min_size;
    volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
    volm_geo_index::read_hyps(root, file_name_pre.str());
    if (is_candidate) {
      volm_geo_index::prune_tree(root, cand_poly);
    }
    std::vector<volm_geo_index_node_sptr> leaves;
    volm_geo_index::get_leaves_with_hyps(root, leaves);

    // load score binary from output folder if exists
    std::stringstream score_file;
    score_file << out() << "ps_1_scores_tile_" << i << ".bin";
    // continue if no score binary exists for this tile
    if (!vul_file::exists(score_file.str()))
      continue;
    std::vector<volm_score_sptr> scores;
    volm_score::read_scores(scores, score_file.str());
    // refill the image
    unsigned total_ind = scores.size();
    for (unsigned ii = 0; ii < total_ind; ii++) {
      vgl_point_3d<double> h_pt = leaves[scores[ii]->leaf_id_]->hyps_->locs_[scores[ii]->hypo_id_];

      // look for location and camera giving maximum score
      if (scores[ii]->max_score_ > max_score) {
        max_score = scores[ii]->max_score_;
        max_score_cam_id = scores[ii]->max_cam_id_;
        max_score_loc = h_pt;
      }
      unsigned u, v;
      if (tile.global_to_img(h_pt.x(), h_pt.y(), u, v)) {
#if 1
      std::cout << " total_ind = " << total_ind << " ii = " << ii << " leaf_id = " << scores[ii]->leaf_id_ << ", hypo_id = " << scores[ii]->hypo_id_
               << std::setprecision(10) << " lon = " << h_pt.x() << " , lat = " << h_pt.y() << ", pixel = (" << u << "," << v << ")"
               << ", score = " << scores[ii]->max_score_ << std::endl;
#endif
        if (u < tile.ni() && v < tile.nj()) {
          // check if this is the highest values for this pixel
          if (scores[ii]->max_score_ > tile_imgs[i](u,v))
            tile_imgs[i](u,v) = scores[ii]->max_score_;
        }
      }
    }
  } // end of tiles

#if 0
  // save the location and camera which give maximum score
  if (!vul_file::exists(cam_bin())) {
    log << "ERROR: can not find camera_space binary: " << cam_bin() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vsl_b_ifstream cam_ifs(cam_bin());
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(cam_ifs);
  cam_ifs.close();
  if (!vul_file::exists(img())) {
    log << "ERROR: can not find the test query image: " << img() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vil_image_view<vxl_byte> query_img = vil_load(img().c_str());

  cam_angles max_cam_ang = cam_space->camera_angles(max_score_cam_id);

  std::string cam_kml = out() + "/MaxScoreCamera.kml";
  std::ofstream ofs_kml(cam_kml.c_str());

  std::stringstream kml_name;
  kml_name << "p1a_test1_" << id();
  bkml_write::open_document(ofs_kml);

  double head = (max_cam_ang.heading_ < 0) ? max_cam_ang.heading_ + 360.0 : max_cam_ang.heading_;
  double tilt = (max_cam_ang.tilt_ < 0) ? max_cam_ang.tilt_ + 360 : max_cam_ang.tilt_;
  double roll;
  if (max_cam_ang.roll_ * max_cam_ang.roll_ < 1E-10) roll = 0;
  else                                               roll = max_cam_ang.roll_;
  double tfov = max_cam_ang.top_fov_;
  double tv_rad = tfov / vnl_math::deg_per_rad;
  double ttr = std::tan(tv_rad);
  double rfov = std::atan( query_img.ni() * ttr / query_img.nj() ) * vnl_math::deg_per_rad;

  bkml_write::write_photo_overlay(ofs_kml, kml_name.str(), max_score_loc.x(), max_score_loc.y(), cam_space->altitude(),
                                  head, tilt, roll, tfov, rfov);
  bkml_write::close_document(ofs_kml);
  ofs_kml.close();
#endif

  // save the ProbMap image and output the grount truth score
  for (unsigned i = 0; i < tiles.size(); i++) {
    std::string img_name = out() + "/" + "ProbMap_float_" + tiles[i].get_string() + ".tif";
    vil_save(tile_imgs[i],img_name.c_str());
    unsigned u, v;
    if (tiles[i].global_to_img(samples[id()].first.x(), samples[id()].first.y(), u, v) ) {
      if (u < tiles[i].ni() && v < tiles[i].nj()) {
        log << "\t GT location: " << id() << ", "
            << samples[id()].first.x() << ", "
            << samples[id()].first.y() << " is at pixel: "
            << u << ", " << v << " in tile " << i << " and has value: "
            << tile_imgs[i](u, v)
            << " max score for this test_img = " << max_score
            //<< " given by camera " << max_cam_ang.get_string()
            << " at location " << max_score_loc.x() << ", " << max_score_loc.y()
            << '\n';
        volm_io::write_post_processing_log(log_file, log.str());
        std::cout << log.str();
      }
    }
  }

  return volm_io::SUCCESS;
}
#endif
