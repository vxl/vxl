//:
// \file
// \executable to read the probability map and generate candidate list given a threshold from 128 to 255
//
//
// \author Yi Dong
// \date Apr 17, 2013

#include <set>
#include <iostream>
#include <functional>
#include <ios>
#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_candidate_list.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vgl/vgl_polygon.h>
#include <vil/algo/vil_find_4con_boundary.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bvrml/bvrml_write.h>
#include <vil/vil_crop.h>
#include <bkml/bkml_parser.h>
#include <bkml/bkml_write.h>
#include <vpgl/vpgl_utm.h>


bool get_top_cameras(unsigned const& tile_idx, std::string const& cam_bin,
                     unsigned const& ni, unsigned const& nj, std::string const& score_folder,
                     std::string const& geo_hypo_folder, std::vector<vgl_point_2d<double> > const& top_locs,
                     std::vector<cam_angles>& top_cameras, std::vector<double>& right_fov)
{
  unsigned top_size = top_locs.size();

  // load the score
  std::stringstream score_file;
  score_file << score_folder + "/ps_1_scores_tile_" << tile_idx << ".bin";
  if (!vul_file::exists(score_file.str()))
    return false;
  std::vector<volm_score_sptr> scores;
  volm_score::read_scores(scores, score_file.str());

  // load the volm_geo_hypo
  std::stringstream file_name_pre;
  file_name_pre << geo_hypo_folder << "/geo_index_tile_" << tile_idx;
  if (!vul_file::exists(file_name_pre.str() + ".txt"))
    return false;
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  std::vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);

  // load the camera space
  if (!vul_file::exists(cam_bin))
    return false;
  vsl_b_ifstream cam_ifs(cam_bin);
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(cam_ifs);
  cam_ifs.close();

  // obtain top cameras for each top_locs
  for (unsigned i = 0; i < top_size; i++) {
    unsigned hypo_id;
    volm_geo_index_node_sptr leaf_node = volm_geo_index::get_closest(root, top_locs[i].y(), top_locs[i].x(), hypo_id);
    unsigned cam_id;
    bool found_loc = false;
    auto vit = scores.begin();
    while (!found_loc) {
      unsigned li = (*vit)->leaf_id_, hi = (*vit)->hypo_id_;
      if (leaf_node->get_string() == leaves[li]->get_string() && hypo_id == hi) {
        found_loc = true;
        cam_id = (*vit)->max_cam_id_;
      }
      ++vit;
    }
    // store the cameras
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
    right_fov.push_back(rfov);
  }

  if (top_cameras.size() != top_size || right_fov.size() != top_size)
    return false;

  return true;
}

bool create_camera_kml(unsigned const& top_id, double const& lon, double const& lat,
                       std::vector<volm_geo_index_node_sptr> const& leaves,
                       volm_geo_index_node_sptr const& leaf, unsigned const& hypo_id,
                       std::string const& score_file, std::string const& out_dir,
                       volm_camera_space_sptr const& cam_space, unsigned ni, unsigned nj, double max_depth)
{
  // load the score
  std::vector<volm_score_sptr> scores;
  volm_score::read_scores(scores, score_file);
  // obtain top camera for location (lon, lat)
  unsigned cam_id;
  double longitude, latitude;
  auto vit = scores.begin();
  bool found = false;
  while (!found && vit != scores.end()) {
    unsigned li = (*vit)->leaf_id_, hi = (*vit)->hypo_id_;
    if (leaf->get_string() == leaves[li]->get_string() && hypo_id == hi) {
      found = true;  cam_id = (*vit)->max_cam_id_;
      longitude = leaves[li]->hyps_->locs_[hi].x();
      latitude = leaves[li]->hyps_->locs_[hi].y();
    }
    ++vit;
  }
  // if the location is not found in score binary, found the closest instead
  if (!found) {
    unsigned min_dist = 1E6;
    vgl_point_3d<double> loc = leaf->hyps_->locs_[hypo_id];
    for (vit = scores.begin(); vit != scores.end(); ++vit) {
      unsigned li = (*vit)->leaf_id_, hi = (*vit)->hypo_id_;
      vgl_point_3d<double> loc_closest = leaves[li]->hyps_->locs_[hi];
      double x_dist = std::abs(loc.x() - loc_closest.x());
      double y_dist = std::abs(loc.y() - loc_closest.y());
      vgl_vector_2d<double> gt_dist_vect(x_dist, y_dist);
      double dist = sqrt(gt_dist_vect.sqr_length());
      if (dist < min_dist) {
        cam_id = (*vit)->max_cam_id_;
        longitude = leaves[li]->hyps_->locs_[hi].x();
        latitude = leaves[li]->hyps_->locs_[hi].y();
      }
    }
  }
  cam_angles best_cam = cam_space->camera_angles(cam_id);

  std::cerr << " top " << top_id << " location " << longitude << "," << latitude << " has best camera :";
  best_cam.print();
  // write out the best camera kml
  std::stringstream cam_kml;
  cam_kml << out_dir << "/BestCamera_top_" << top_id << ".kml";
  std::stringstream kml_name;
  kml_name << "BestCamera_top_" << top_id;

  std::ofstream ofs_kml(cam_kml.str().c_str());
  bkml_write::open_document(ofs_kml);

  double head = (best_cam.heading_ < 0) ? best_cam.heading_+360.0 : best_cam.heading_;
  double tilt = (best_cam.tilt_ < 0) ? best_cam.tilt_+360.0 : best_cam.tilt_;
  double roll = (best_cam.roll_ * best_cam.roll_ < 1E-10) ? 0 : best_cam.roll_;
  double tfov = best_cam.top_fov_;
  double tv_rad = tfov / vnl_math::deg_per_rad;
  double ttr = std::tan(tv_rad);
  double rfov = std::atan( ni * ttr / nj) * vnl_math::deg_per_rad;

  bkml_write::write_photo_overlay(ofs_kml, kml_name.str(), lon, lat, cam_space->altitude(), head, tilt, roll, tfov, rfov, max_depth);
  bkml_write::close_document(ofs_kml);
  ofs_kml.close();
  return true;
}

int main(int argc,  char** argv)
{
  vul_arg<std::string> out("-out", "folder where probability map stores", "");
  vul_arg<std::string> out_kml("-out_kml", "folder where created candidate list stores","");
  vul_arg<std::string> score_folder("-score", "folder where the score binaries are","");
  vul_arg<std::string> geo_hypo("-geo", "folder where geo hypotheses for utm zone 17 are","");
  vul_arg<std::string> cam_bin("-cam", "camera space binary", "");
  vul_arg<std::string> query_img("-img", "query image", "");
  vul_arg<unsigned> test_id("-testid", "phase 1 test id", 0);
  vul_arg<unsigned> id("-imgid", "query image id", 101);
  vul_arg<unsigned> world_id("-world", "roi world id", 9999);
  vul_arg<unsigned> threshold("-thres", "threshold that used to create candidate list", 0);
  vul_arg<float> thres_scale("-scale", "threshold that used to create Probability map", 0.0f);
  vul_arg<unsigned> top_size("-top", "desired top list for each candidate list", 1);
  vul_arg<float> kl ("-kl", "parameter for nonlinear score scaling", 200.0f);
  vul_arg<float> ku ("-ku", "parameter for nonlinear score scaling", 10.0f);
  vul_arg<float> gt_score ("-gts", "ground truth score if exist", 0.0f);
  vul_arg<bool> is_camera ("-is_cam", "option to choose whether we write best cameras as photo overlay", false);
  vul_arg<bool> is_generate_top_cam_kml ("-is_top_cam", "option to choose whether we retrive top cameras from generated candidate list", false);
  vul_arg<unsigned> top_cam_num("-top_cam_num", "number of top camera kml we want to generate",10);
  vul_arg<std::string> candlist_kml("-cand_file", "generate candidate list file", "");
  vul_arg<std::string> dms_bin("-dms", "depth_map_scene binary to get the depth value for all objects", "");
  vul_arg<std::string> candidate_list("-cand", " pre defined candidate list for given query", "");  // index -- candidate list file containing polygons
  vul_arg_parse(argc, argv);
  std::cout << "argc: " << argc << std::endl;

  // given a generate candidate list kml with top locations/regions, retrive camera for the top location and store the camera for
  // rendering purpose
  if (is_generate_top_cam_kml()) {
    std::cout << " retrive the best cameras for top locations stored in the input candidate list file :" << std::endl;
    if (candlist_kml().compare("") == 0 || !vul_file::exists(candlist_kml())) {
      vul_arg_display_usage_and_exit();
      std::cerr << " input candidate list file does NOT exist\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    std::string out_dir = vul_file::dirname(candlist_kml()) + '/';
    std::stringstream log;
    std::string log_file = out_dir + "/candidate_list_top_camera_log.xml";
    std::string rational_folder = out_dir + "/rationale/";
    vul_file::make_directory(rational_folder);
    // check the input parameter
    if (cam_bin().compare("") == 0 || query_img().compare("") == 0 || score_folder().compare("") == 0 || dms_bin().compare("") == 0 || geo_hypo().compare("") == 0)
    {
      log << " ERROR: input files/folder can not be empty\n";
      vul_arg_display_usage_and_exit();
      volm_io::write_post_processing_log(log_file, log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    // load camera space
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
    // load the depth_map_scene
    if (!vul_file::exists(dms_bin())) {
      log << "ERROR: can not find the depth map scene binary file: " << dms_bin() << '\n';
      volm_io::write_post_processing_log(log_file, log.str());  std::cerr << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    depth_map_scene_sptr dms = new depth_map_scene;
    vsl_b_ifstream dms_ifs(dms_bin().c_str());
    dms->b_read(dms_ifs);
    dms_ifs.close();
    double max_depth = -10.0;
    for (unsigned o_idx = 0; o_idx < dms->scene_regions().size(); o_idx++)
      if (max_depth < dms->scene_regions()[o_idx]->min_depth())
        max_depth = dms->scene_regions()[o_idx]->min_depth();

    // obtain query image size
    if (!vul_file::exists(query_img())) {
      log << "ERROR: can not find the test query image: " << query_img() << '\n';
      volm_io::write_post_processing_log(log_file, log.str());
      std::cerr << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    vil_image_view<vxl_byte> query_image = vil_load(query_img().c_str());
    unsigned ni, nj;
    ni = query_image.ni();
    nj = query_image.nj();

    // retrieve top locations from candidate list file
    std::vector<vgl_point_3d<double> > top_locs = bkml_parser::parse_points(candlist_kml());

    // get the closest location from geolocations
    unsigned top_cams;
    if (top_cam_num() > top_locs.size())
      top_cams = top_locs.size();
    else
      top_cams = top_cam_num();
    // create tiles
    std::vector<volm_tile> tiles;
    tiles = volm_tile::generate_p1_wr2_tiles();

    for (unsigned i = 0; i < top_cams; i++) {
      double lon = top_locs[i].x(), lat = top_locs[i].y();
      // locate tile
      unsigned tile_id = tiles.size();
      for (unsigned t_idx = 0; t_idx < tiles.size(); t_idx++) {
        unsigned u, v;
        if (tiles[t_idx].global_to_img(lon, lat, u, v))
          if (u<tiles[t_idx].ni() && v<tiles[t_idx].nj())
            tile_id = t_idx;
      }
      // locate zone
      vpgl_utm utm;
      int zone_id;
      double x, y;
      utm.transform(lat, lon, x, y, zone_id);
      std::cerr << " Top " << i << " locations " << lon << ',' << lat << " is in utm zone " << zone_id << " and tile " << tile_id << std::endl;

      // construct the volm_geo_index
      std::string geo_hypo_folder = geo_hypo();
      std::stringstream file_name_pre;
      file_name_pre << geo_hypo_folder << "geo_index_tile_" << tile_id;
      if (!vul_file::exists(file_name_pre.str() + ".txt")) {
        log << " ERROR: location " << lon << ',' << lat << " is in tile " << tile_id << " but no geo locations for this tile, stop\n";
        volm_io::write_post_processing_log(log_file, log.str());  std::cerr << log.str();
        return volm_io::POST_PROCESS_FAILED;
      }
      float min_size;
      volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
      volm_geo_index::read_hyps(root, file_name_pre.str());


      // use the candidate list same as that being used in matcher to prune the tree
      // check whether we have candidate list for this query
      bool is_candidate = false;
      vgl_polygon<double> cand_poly;
      std::cout << " candidate list = " <<  candidate_list() << std::endl;
      if ( candidate_list().compare("") != 0) {
        if (!vul_file::exists(candidate_list())) {
          log << " ERROR: can not fine candidate list file: " << candidate_list() << '\n';
          std::cerr << log.str();
          volm_io::write_post_processing_log(log_file, log.str());  std::cerr << log.str();
          return volm_io::POST_PROCESS_FAILED;
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
      if (is_candidate) {
        volm_geo_index::prune_tree(root, cand_poly);
      }
      std::vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(root, leaves);
      // obtain leaf_id and hypo_id for current location
      unsigned hypo_id = 0;
      volm_geo_index_node_sptr leaf = volm_geo_index::get_closest(root, lat, lon, hypo_id);
      vgl_point_3d<double> loc_closest;
      if (leaf) {
        loc_closest = leaf->hyps_->locs_[hypo_id];
        double sec_to_meter = 21.0/0.000202;
        double x_dist = std::abs(lon - loc_closest.x())*sec_to_meter;
        double y_dist = std::abs(lat - loc_closest.y())*sec_to_meter;
        vgl_vector_2d<double> gt_dist_vect(x_dist, y_dist);
        double gt_dist = sqrt(gt_dist_vect.sqr_length());
        std::cout << " loc_closest = " << loc_closest << " with leaf id = " << leaf->get_string() << " and hypo_id = " << hypo_id << std::endl;
        if (gt_dist > 0)
          log << " NOTE: distance from the closest location " << loc_closest << " in geo location to location " << top_locs[i]
              << " is " << gt_dist << " meters\n";
      }
      else {
        log << " ERROR: location " << lon << ',' << lat << " does not exist in any leaf in tile " << tile_id << '\n';
        volm_io::write_post_processing_log(log_file, log.str());  std::cerr << log.str();
        return volm_io::POST_PROCESS_FAILED;
      }
      // create camera kml for current location
      // get the score folder
      std::stringstream score_file;
      score_file << score_folder() + "/ps_1_scores_tile_" << tile_id << ".bin";
      if (!vul_file::exists(score_file.str())) {
        log << " ERROR: can not find the score file " << score_file.str() << "\n";
        volm_io::write_post_processing_log(log_file, log.str());  std::cerr << log.str();
        return volm_io::POST_PROCESS_FAILED;
      }
      if (!create_camera_kml(i, loc_closest.x(), loc_closest.y(), leaves, leaf, hypo_id, score_file.str(), rational_folder, cam_space, ni, nj, max_depth)){
        log << "  ERROR: retrieve best camera for location " << loc_closest << " failed\n";
        volm_io::write_post_processing_log(log_file, log.str());  std::cerr << log.str();
        return volm_io::POST_PROCESS_FAILED;
      }
    } // end of loop over all top_locs

    volm_io::write_composer_log(log_file, log.str());
    std::cerr << log.str() << std::endl;
    return volm_io::SUCCESS;
  } // end of generate best camera kml files for top locations

  std::stringstream log;
  std::string log_file = out_kml() + "/candidate_list_log.xml";
  std::cout << " log_file = " << log_file << std::endl;
  // check input parameter
  if (out().compare("") == 0 || threshold() == 0 || world_id() == 9999 || cam_bin().compare("") == 0 || query_img().compare("") == 0 ||
      score_folder().compare("") == 0 || geo_hypo().compare("") == 0 || out_kml().compare("") == 0 ||
      id() > 900 || test_id() == 0 || thres_scale() == 0.0f) {
    log << " ERROR: input files/folders/arguments can not be empty\n";
    vul_arg_display_usage_and_exit();
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // obtain the query image size
  if (!vul_file::exists(query_img())) {
    log << "ERROR: can not find the test query image: " << query_img() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vil_image_view<vxl_byte> query_image = vil_load(query_img().c_str());
  unsigned ni, nj;
  ni = query_image.ni();
  nj = query_image.nj();


  // create volm_tile
  std::vector<volm_tile> tiles;
  if (!volm_tile::generate_tiles(world_id(), tiles)) {
    log << "ERROR: unknown world id: " << world_id() << "!!!\n";
    volm_io::write_post_processing_log(log_file, log.str());  std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  unsigned n_tile = tiles.size();

#if 0
  // check the input files for each tile (for phase 1b)
  for (unsigned t_idx = 0; t_idx < n_tile; t_idx++) {
    if ( (world_id() == 2 && t_idx == 0) || (world_id() == 2 && t_idx == 2) )
      continue;
    std::stringstream score_file;
    score_file << score_folder() + "/ps_1_scores_tile_" << t_idx << ".bin";
    std::stringstream file_name_pre;
    file_name_pre << geo_hypo_a() << "/geo_index_tile_" << t_idx;
    if (!vul_file::exists(file_name_pre.str() + ".txt") || !vul_file::exists(score_file.str())) {
      log << " ERROR: can not found the score file or geo_hypo for tile " << t_idx << "\n"
          << "\t score_file = " << score_file.str() << "\n\t geo_hypo file = " << file_name_pre.str() << ".txt" << "\n";
      volm_io::write_post_processing_log(log_file, log.str());
      std::cerr << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
  }
#endif

#if 0
  for (unsigned t_idx = 0; t_idx < n_tile; t_idx++) {
    if (t_idx == 10)
      continue;
    unsigned zone_id;
    if (t_idx < 8 && t_idx != 5) {
      zone_id = 17;
      std::stringstream score_file;
      score_file << score_folder() + "/ps_1_scores_tile_" << t_idx << ".bin";
      std::stringstream file_name_pre;
      file_name_pre << geo_hypo_a() << "/geo_index_tile_" << t_idx;
      if (!vul_file::exists(file_name_pre.str() + ".txt") || !vul_file::exists(score_file.str())) {
        log << " ERROR: can not found the score file or geo_hypo for tile " << t_idx << "\n"
            << "\t score_file = " << score_file.str() << "\n\t geo_hypo file = " << file_name_pre.str() << ".txt" << "\n";
        volm_io::write_post_processing_log(log_file, log.str());
        std::cerr << log.str();
        return volm_io::EXE_ARGUMENT_ERROR;
      }
    }
    else {
      zone_id = 18;
      std::stringstream score_file;
      score_file << score_folder() + "/ps_1_scores_zone_" << zone_id << "_tile_" << t_idx << ".bin";
      std::stringstream file_name_pre;
      file_name_pre << geo_hypo_b() << "geo_index_tile_" << t_idx;
      if (!vul_file::exists(file_name_pre.str() + ".txt") || !vul_file::exists(score_file.str())) {
        log << " ERROR: can not found the score file or geo_hypo for tile " << t_idx << "\n"
            << "\t score_file = " << score_file.str() << "\n\t geo_hypo file = " << file_name_pre.str() << ".txt" << "\n";
        volm_io::write_post_processing_log(log_file, log.str());
        std::cerr << log.str();
        return volm_io::EXE_ARGUMENT_ERROR;
      }
    }
  }
#endif

  // create candidate list for each tile
  std::vector<volm_candidate_list> cand_lists;
  for (unsigned t_idx = 0; t_idx < n_tile; t_idx++) {
    std::string img_name = out() + "/" + "ProbMap_" + tiles[t_idx].get_string() + ".tif";
    if (!vul_file::exists(img_name)) {
      log << " ERROR: missing prob_map: " << img_name << '\n';
      volm_io::write_post_processing_log(log_file, log.str());
      std::cerr << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    vil_image_view<vxl_byte> tile_img = vil_load(img_name.c_str());
    volm_candidate_list cand_list(tile_img, threshold());
    cand_lists.push_back(cand_list);
  }

#if 1
  // visualize the candidate list
  for (unsigned t_idx = 0; t_idx < n_tile; t_idx++) {
    vil_image_view<vxl_byte> cand_img;
    std::string img_name = out() + "/" + "Candidate_list_" + tiles[t_idx].get_string() + ".tif";
    if(!cand_lists[t_idx].candidate_list_image(cand_img)) {
      log << " ERROR: creating candidate image " << img_name << " failed\n";
      volm_io::write_post_processing_log(log_file,log.str());
      std::cerr << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    vil_save(cand_img, img_name.c_str());
  }
#endif

  // create the map for all regions
  std::multimap<unsigned, std::pair<unsigned, unsigned>, std::greater<unsigned> > cand_map;
  for (unsigned tile_idx = 0; tile_idx < tiles.size(); tile_idx++) {
    std::vector<unsigned> region_score;
    if (!cand_lists[tile_idx].region_score(region_score)) {
      log << " ERROR: can not get best scores for candidate regions in tile " << tile_idx << '\n';
      volm_io::write_post_processing_log(log_file, log.str());
      std::cerr << log.str();
      return volm_io::POST_PROCESS_FAILED;
    }
    unsigned n_sheet = cand_lists[tile_idx].num_of_regions();
    for (unsigned sh_idx = 0; sh_idx < n_sheet; sh_idx++) {
      std::pair<unsigned, unsigned> pair(tile_idx, sh_idx);
      cand_map.insert(std::pair<unsigned, std::pair<unsigned, unsigned> >(region_score[sh_idx], pair));
    }
  }

  std::cout << " create candidate list kml" << std::endl;
  // write the candidate list
  std::stringstream kml_name;
  if (id() < 10)        kml_name << "p1b_test" << test_id() << "_0" << id();
  else if (id() < 100)  kml_name << "p1b_test" << test_id() << "_" << id();

#if 0
  std::stringstream kml_name;
  if (id() < 10)
    kml_name << "p1b_test" << test_id() << "_00" << id();
  else if (id() >= 10 && id() < 100)
    kml_name << "p1b_test" << test_id() << "_0" << id();
  else
    kml_name << "p1b_test" << test_id() << "_" << id();
#endif

  std::string cam_kml = out_kml() + "/" + kml_name.str() + "-CANDIDATE.kml";
  std::ofstream ofs_kml(cam_kml.c_str());
  float thres_value = volm_io::scale_score_to_0_1_sig(kl(), ku(), thres_scale(), threshold());

  if (gt_score() != 0.0f) {
    std::cerr << " ground truth score = " << gt_score() << " , threshold likelihood = " << thres_value << std::endl;
    if (gt_score() < thres_value) {
      log << " chosen threshold " << threshold() << " is larger than ground truth score, reduce its value\n";
      std::cerr << log.str();
      volm_io::write_post_processing_log(log_file, log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
  }

  log << " there are " << cand_map.size() << " candidate regions given threshold = " << threshold() << " (likelihood = " << thres_value << ")\n";
  std::cerr << log.str();
  volm_io::write_post_processing_log(log_file, log.str());
  volm_candidate_list::open_kml_document(ofs_kml,kml_name.str(),thres_value);
  auto mit = cand_map.begin();
  unsigned rank = 0;
  for (; mit != cand_map.end(); ++mit) {
    unsigned tile_idx = mit->second.first;
    unsigned sh_idx = mit->second.second;
    std::string geo_hypo_folder = geo_hypo();
    // transfer loc_image to loc_global
    std::vector<vgl_point_2d<double> > region_loc_global;
    cand_lists[tile_idx].img_to_golbal(sh_idx, tiles[tile_idx], region_loc_global);
    // obtain the top locations
    std::vector<vgl_point_2d<double> > top_locs;
    std::vector<unsigned> top_loc_scores;
    cand_lists[tile_idx].top_locations(top_locs, top_loc_scores, tiles[tile_idx], top_size(), sh_idx);
    // obtain the top cameras given the top locations
    std::vector<cam_angles> top_cameras;
    std::vector<double> right_fov;
    if (is_camera()) {
      if (!get_top_cameras(tile_idx, cam_bin(), ni, nj, score_folder(), geo_hypo_folder, top_locs, top_cameras, right_fov)) {
        log << " ERROR: failed to fetch top cameras for tile " << tile_idx << " sheet " << sh_idx << '\n';
        volm_io::write_post_processing_log(log_file, log.str());
        std::cerr << log.str();
        return volm_io::POST_PROCESS_FAILED;
      }
    } else {
      for (unsigned idx = 0; idx < top_locs.size(); idx++) {
        top_cameras.emplace_back(2.64, 15, 334.0, 91.34);
        right_fov.push_back(20.0);
      }
    }
    // calculate the likelihood based on the top scores
    float likelihood = volm_io::scale_score_to_0_1_sig(kl(), ku(), thres_scale(), (unsigned char)mit->first);
    // write the region polygon and top locations
    volm_candidate_list::write_kml_regions(ofs_kml, region_loc_global, top_locs, top_cameras, right_fov, likelihood, rank++);

#if 1
    //if (top_locs.size() != top_loc_scores.size()) {
    //  std::cout << " --> tile = " << tile_idx << " sh_id = " << sh_idx << " top_locs = " << top_locs.size() << " score = " << top_locs.size() << std::endl;
    //}
    for (unsigned i = 0 ; i < top_locs.size(); i++) {
      if (rank%500 == 0)
        std::cerr << " size = " << cand_map.size() << ", cnt = " << rank-1 << " score = " << mit->first << " --> tile = " << tile_idx << " sh_id = " << sh_idx
                 << " --> top_locs = " << std::setprecision(10) << top_locs[i].x() << ", " << top_locs[i].y() << " top_score = " << top_loc_scores[i] << std::endl;
    }
#endif
  }
  volm_candidate_list::close_kml_document(ofs_kml);

#if 0
  // screen output
  for (unsigned t_idx = 0; t_idx < n_tile; t_idx++) {
    std::cout << " For tile " << t_idx << ", there are " << cand_lists[t_idx].num_of_regions() << " regions above threshold " << threshold() << std::endl;
  }
  // best scores
  for (unsigned t_idx = 0; t_idx < n_tile; t_idx++) {
    std::vector<unsigned> scores;
    if( !cand_lists[t_idx].region_score(scores))
      return volm_io::POST_PROCESS_FAILED;
    std::cout << " For tile " << t_idx << ", region scores = ";
    for (std::vector<unsigned>::iterator vit = scores.begin(); vit != scores.end(); ++vit)
      std::cout << *vit << ' ';
    std::cout << std::endl;
  }
  // candidate list map
  mit = cand_map.begin();
  for(; mit != cand_map.end(); ++mit) {
    std::cout << " region score = " << mit->first << " --> tile_id = " << mit->second.first << " , sheet_id = " << mit->second.second << std::endl;
    std::vector<vgl_point_2d<int> > top_loc_image;
    std::vector<unsigned> top_loc_scores;
    cand_lists[mit->second.first].top_locations(top_loc_image, top_loc_scores,top_size(),mit->second.second);
    for (unsigned i = 0; i < top_size(); i++) {
      std::cout << "\t top_locs = " << top_loc_image[i] << " score = " << top_loc_scores[i] << std::endl;
    }
  }
#endif

  return volm_io::SUCCESS;
}
