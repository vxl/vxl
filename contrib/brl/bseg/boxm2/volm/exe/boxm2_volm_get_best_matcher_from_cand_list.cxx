//:
// \file
// \executable to read the candidate region and find the best match in that region from volm_matcher
//  Note the input candidate kml file should include numbers of candidate region expressed as polygons
//
//
// \author Yi Dong
// \date Aug. 24, 2013

#include <algorithm>
#include <bkml/bkml_parser.h>
#include <bkml/bkml_write.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vgl/vgl_intersection.h>
#include <vil/vil_load.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_io.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void error_report(std::string error_file, const std::string& error_msg)
{
  std::cerr << error_msg;
  volm_io::write_post_processing_log(std::move(error_file), error_msg);
}

unsigned zone_id(unsigned tile_id)
{
  if (tile_id < 8 && tile_id != 5)
    return 17;
  else if(tile_id == 5)
    return 18;
  else if(tile_id >= 8 && tile_id <= 13)
    return 18;
  else
    return 1000;
}

bool best_match(vgl_polygon<double> const& poly, std::vector<volm_geo_index_node_sptr> const &leaves, std::string const& score_file,
                float& max_score, vgl_point_2d<double>& best_location, unsigned& best_cam_id)
{
  // get the leaves that intersect with the candidate region
  std::vector<unsigned> leaf_ids;
  for(unsigned l_idx = 0; l_idx < leaves.size(); l_idx++)
    if (vgl_intersection(leaves[l_idx]->extent_, poly))
      leaf_ids.push_back(l_idx);
  // load the score
  std::vector<volm_score_sptr> scores;
  volm_score::read_scores(scores, score_file);
  auto vit = scores.begin();
  for (; vit != scores.end(); ++vit) {
    unsigned li = (*vit)->leaf_id_;
    if (std::find(leaf_ids.begin(), leaf_ids.end(), li) == leaf_ids.end())
      continue;
    unsigned hi = (*vit)->hypo_id_;
    vgl_point_3d<double> loc = leaves[li]->hyps_->locs_[hi];
    if (!poly.contains(loc.x(), loc.y()))
      continue;
    float score = (*vit)->max_score_;
    if (score > max_score) {
      max_score = score;
      best_location.set(loc.x(), loc.y());
      best_cam_id = (*vit)->max_cam_id_;
    }
  }
  return true;
}

int main(int argc, char** argv)
{
  vul_arg<unsigned> world_id("-world", "world id of ROI", 100);
  vul_arg<unsigned> test_id("-testid", "test ids", 0);
  vul_arg<unsigned> img_id("-imgid", "query image id", 1000);
  vul_arg<std::string> cam_bin("-cam", "camera space binary", "");
  vul_arg<std::string> query_img("-img", "query image", "");
  vul_arg<std::string> dms_bin("-dms", "depth_map_scene binary to get the depth value for all objects", "");
  vul_arg<std::string> geo_hypo_a("-geoa", "folder where geo hypotheses for utm zone 17 are","");
  vul_arg<std::string> geo_hypo_b("-geob", "folder where geo hypotheses for utm zone 18 are","");
  vul_arg<std::string> score_folder("-score", "folder where the score binaries are","");
  vul_arg<std::string> candlist_kml("-cand_file", "generate candidate list file", "");
  vul_arg<std::string> candidate_list("-cand", " pre defined candidate list for given query", "");  // index -- candidate list file containing polygons
  vul_arg<unsigned> top_cam_num("-top_cam_num", "number of top camera kml we want to generate",10);

  vul_arg_parse(argc, argv);

  int ii = 0;
  std::cout << " start " << std::endl;
  int jj = 1;

  if (candlist_kml().compare("")==0 || score_folder().compare("") == 0 || world_id() == 100 ||
      cam_bin().compare("")==0 || query_img().compare("")==0 || dms_bin().compare("") == 0 ||
      geo_hypo_a().compare("") == 0|| geo_hypo_b().compare("") == 0 || score_folder().compare("") == 0 ||
      test_id() == 0 || img_id() == 1000)
  {
    vul_arg_display_usage_and_exit();
    std::cerr << " input parameter is missing \n";
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::string out_dir = vul_file::dirname(candlist_kml());
  std::stringstream log;
  std::string log_file = out_dir + "/candidate_list_top_camera_log.xml";
  std::string rationale_folder = out_dir + "/rationale/";
  vul_file::make_directory(rationale_folder);

  // load camera space
  if (!vul_file::exists(cam_bin())) {
    log << "ERROR: can not find camera_space binary: " << cam_bin() << '\n';
    error_report(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vsl_b_ifstream cam_ifs(cam_bin());
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(cam_ifs);
  cam_ifs.close();
  // load the depth_map_scene
  if (!vul_file::exists(dms_bin())) {
    log << "ERROR: can not find the depth map scene binary file: " << dms_bin() << '\n';
    error_report(log_file, log.str());  return volm_io::EXE_ARGUMENT_ERROR;
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
    error_report(log_file, log.str());  return volm_io::EXE_ARGUMENT_ERROR;
  }
  vil_image_view<vxl_byte> query_image = vil_load(query_img().c_str());
  unsigned ni, nj;
  ni = query_image.ni();
  nj = query_image.nj();

  // create volm_tile
  std::vector<volm_tile> tiles;/* = volm_tile::generate_p1_wr2_tiles();*/
  if (world_id() == 1)      tiles = volm_tile::generate_p1b_wr1_tiles();
  else if (world_id() == 2) tiles = volm_tile::generate_p1b_wr2_tiles();
  else if (world_id() == 3) tiles = volm_tile::generate_p1b_wr3_tiles();
  else if (world_id() == 4) tiles = volm_tile::generate_p1b_wr4_tiles();
  else if (world_id() == 5) tiles = volm_tile::generate_p1b_wr5_tiles();
  else {
    log << " ERROR: unknown world id " << world_id() << " only 1 to 5 is allowed\n";
    error_report(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // check the score binary files in advance
  for (unsigned t_idx = 0; t_idx < tiles.size(); t_idx++) {
    if (t_idx == 10) continue;
    std::stringstream score_file;
    score_file << score_folder() << "/ps_1_scores_tile_" << t_idx << ".bin";
    if (!vul_file::exists(score_file.str())) {
      log << " WARNING: can not find score file: " << score_file.str() << '\n';
      error_report(log_file, log.str());
      //return volm_io::EXE_ARGUMENT_ERROR;
    }
  }

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
  std::cout << " ============================  START ========================= " << std::endl;
  // get candidate regions from candidate list file
  vgl_polygon<double> cand_regions = bkml_parser::parse_polygon(candlist_kml());

  // for each candidate region, search for the best match
  unsigned num_regions = top_cam_num();
  if (top_cam_num() > cand_regions.num_sheets()) num_regions = cand_regions.num_sheets();
  std::cout << " \t candidate file contains " <<  num_regions << " regions" << std::endl;
  for (unsigned r_idx = 0; r_idx < num_regions; r_idx ++) {
    vgl_polygon<double> poly(cand_regions[r_idx]);
    // check the intersected tile
    std::vector<unsigned> tile_ids;
    std::vector<unsigned> zone_ids;
    for (unsigned t_idx = 0; t_idx < tiles.size(); t_idx++)
      if (vgl_intersection(tiles[t_idx].bbox_double(), poly)) {
        tile_ids.push_back(t_idx);  zone_ids.push_back(zone_id(t_idx));
      }
    std::cout << " \t region " << r_idx << " intersects with tile: ";
    for (unsigned int tile_id : tile_ids)
      std::cout << tile_id << ' ';
    std::cout << std::endl;
    // get the best camera and location for current region
    unsigned best_cam_id;
    vgl_point_2d<double> best_location;
    float max_score = -1E6;
    for (unsigned i = 0; i < tile_ids.size(); i++) {
      std::string geo_hypo_folder;
      if (zone_ids[i] == 17)
        geo_hypo_folder = geo_hypo_a();
      else if(zone_ids[i] == 18)
        geo_hypo_folder = geo_hypo_b();
      else {
        log << "ERROR: unknown tile_id " << tile_ids[i] << " and zone_id " << zone_ids[i] << '\n';
        error_report(log_file, log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }
      // load associated volm_geo_index
      std::stringstream file_name_pre;
      file_name_pre << geo_hypo_folder << "geo_index_tile_" << tile_ids[i];
      if (!vul_file::exists(file_name_pre.str()+".txt")) {
        log << "ERROR: can not find volm_geo_index txt file: " << file_name_pre.str() << '\n';
        error_report(log_file, log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }
      float min_size;
      volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
      volm_geo_index::read_hyps(root, file_name_pre.str());
      if (is_candidate)
        volm_geo_index::prune_tree(root, cand_poly);
      std::vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(root, leaves);

      std::stringstream score_file;
      score_file << score_folder() << "/ps_1_scores_tile_" << tile_ids[i] << ".bin";
      if (!vul_file::exists(score_file.str()))
        continue;
      // search for the best match for current tile
      if (!best_match(poly, leaves, score_file.str(), max_score, best_location, best_cam_id)) {
        log << "ERROR: searching of the best match for region " << r_idx << " in tile " << tile_ids[i] << '\n';
        error_report(log_file, log.str());
        return volm_io::POST_PROCESS_FAILED;
      }
    }
    // write out the camera kml
    cam_angles best_cam = cam_space->camera_angles(best_cam_id);
    std::cout << " \t after searching region " << r_idx << " has best maximum " << max_score << " at location "
             << std::setprecision(12) << best_location.x() << std::setprecision(12) << best_location.y() << " and camera ";
    best_cam.print();
    double head = (best_cam.heading_ < 0) ? best_cam.heading_+360.0 : best_cam.heading_;
    double tilt = (best_cam.tilt_ < 0) ? best_cam.tilt_+360.0 : best_cam.tilt_;
    double roll = (best_cam.roll_ * best_cam.roll_ < 1E-10) ? 0 : best_cam.roll_;
    double tfov = best_cam.top_fov_;
    double tv_rad = tfov / vnl_math::deg_per_rad;
    double ttr = std::tan(tv_rad);
    double rfov = std::atan( ni * ttr / nj) * vnl_math::deg_per_rad;

    std::stringstream cam_kml;
    if (r_idx == 0)
      cam_kml << rationale_folder << "/BestCamera_top_" << r_idx << ".kml";
    else
      cam_kml << rationale_folder << "/BestCamera_top_" << r_idx-1 << ".kml";
    std::stringstream kml_name;
    kml_name << "BestCamera_top_" << r_idx;

    std::ofstream ofs_kml(cam_kml.str().c_str());
    bkml_write::open_document(ofs_kml);
    bkml_write::write_photo_overlay(ofs_kml, kml_name.str(), best_location.x(), best_location.y(), cam_space->altitude(), head, tilt, roll, tfov, rfov, max_depth);
    bkml_write::write_location(ofs_kml, best_location, kml_name.str());
    bkml_write::close_document(ofs_kml);
    ofs_kml.close();
  }


  return volm_io::SUCCESS;
}
