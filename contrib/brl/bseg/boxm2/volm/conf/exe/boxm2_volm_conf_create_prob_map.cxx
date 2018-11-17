// This is contrib/brl/bseg/boxm2/volm/conf/exe/boxm2_volm_create_prob_map.cxx
//:
// \file
// \brief  executable to create probability map given configurational matcher output.  The
//         output map has 30 by 30 meter resolution and pixel values are the height score in the space of 30x30 meters
//
// \author Yi Dong
// \data September 11, 2014
// \verbatim
//   Modifications
//    <none yet>
// \endverbatim
//

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vgl/io/vgl_io_polygon.h>
#include <vgl/vgl_intersection.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_lvcs.h>
#include <bkml/bkml_parser.h>
#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_candidate_list.h>
#include <volm/conf/volm_conf_buffer.h>
#include <volm/conf/volm_conf_score.h>
#include <volm/volm_utils.h>

bool is_same(double const& x, double const& y)
{
  return (std::fabs(x-y) < 1E-6);
}

double distance_in_meter(double const& q_lon, double const& q_lat, double const& d_lon, double const& d_lat)
{
  // calculate the actual distance in meters using local lvcs
  vpgl_lvcs lvcs(q_lat, q_lon, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double lx, ly, lz;
  lvcs.global_to_local(d_lon, d_lat, 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
  return std::sqrt(lx*lx + ly*ly);
}

int main(int argc, char** argv)
{
  vul_arg<unsigned>      world_id("-world",     "ROI world id", 9999);
  vul_arg<unsigned>       tile_id("-tile",      "ROI tile id", 9999);
  vul_arg<std::string>  geo_folder("-geo",      "folder to read the geo hypotheses ", "");
  vul_arg<std::string>  index_name("-idx-name", "name of the loaded index", "");
  vul_arg<std::string>  out_folder("-out",      "output probability map image","");
  vul_arg<std::string> cand_folder("-cand",     "candidate region kml provided by previous matcher", "");
  vul_arg<float>  buffer_capacity("-buffer",   "buffer capacity for index creation (in GByte)", 2.0f);
  vul_arg<bool>       query_score("-query",    "option to return the score value at a given location", false);
  vul_arg<double>          gt_lon("-lon",      "location longitude", 0.0);
  vul_arg<double>          gt_lat("-lat",      "location latitude", 0.0);
  vul_arg_parse(argc, argv);

  // input check
  if (tile_id() == 9999 || geo_folder().compare("") == 0 || index_name().compare("") == 0 || out_folder().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::stringstream log_file;
  std::stringstream log;
  log_file << out_folder() << "/log_prob_map_tile_" << tile_id() << ".xml";

  // load geo index
  std::stringstream file_name_pre;
  file_name_pre << geo_folder() << "/geo_index_tile_" << tile_id();
  if (!vul_file::exists(file_name_pre.str()+".txt")) {
    log << "ERROR: loading geo index locations fails from file: " << file_name_pre.str() << ".txt!\n";
    volm_io::write_error_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  std::vector<volm_geo_index_node_sptr> loc_leaves;
  loc_leaves.clear();
  volm_geo_index::get_leaves_with_hyps(root, loc_leaves);

  if (query_score())
  {
    unsigned hyp_id;
    volm_geo_index_node_sptr leaf = volm_geo_index::get_closest(root, gt_lat(), gt_lon(), hyp_id);
    if (!leaf) {
      std::cout << "query location " << gt_lon() << ", " << gt_lat() << " is not in location database" << std::endl;
      return volm_io::SUCCESS;
    }
    vgl_point_3d<double> closest_pt = leaf->hyps_->locs_[hyp_id];
    // load associate score for it
    std::stringstream score_file_pre;
    score_file_pre << out_folder() << "/conf_score_tile_" << tile_id();
    std::string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name() + ".bin";
    double dist = distance_in_meter(gt_lon(), gt_lat(), closest_pt.x(), closest_pt.y());
    std::cout << "location " << std::setprecision(6) << std::fixed << gt_lon() << ", " << std::setprecision(6) << std::fixed << gt_lat()
             << " is in leaf " << leaf->extent_
             << " and closest location (" << dist << " meter away) is "
             << std::setprecision(6) << std::fixed << closest_pt.x() << ", " << std::setprecision(6) << std::fixed << closest_pt.y() << std::endl;
    if (!vul_file::exists(score_bin_file)) {
      std::cout << "No score for the query location (probably outside candidate region)" << std::endl;
      return volm_io::SUCCESS;
    }
    volm_conf_buffer<volm_conf_score> score_idx(buffer_capacity());
    std::cout << leaf->hyps_->locs_.size() << " locations are in leaf" << std::endl;
    score_idx.initialize_read(score_bin_file);
    // load the candidate list for current leaf
    bool is_cand = false;
    // load and check candidate region
    std::string outer_region_file = cand_folder() + "/cand_region_outer_" + leaf->get_string() + ".bin";
    vgl_polygon<double> cand_outer, cand_inner;
    cand_outer.clear();
    cand_inner.clear();
    if (vul_file::exists(outer_region_file)) {  // read the exterior boundaries of candidate region
      is_cand = true;
      vsl_b_ifstream ifs_out(outer_region_file);
      vsl_b_read(ifs_out, cand_outer);
      ifs_out.close();
    }
    std::string inner_region_file = cand_folder() + "/cand_region_inner_" + leaf->get_string() + ".bin";
    if (vul_file::exists(inner_region_file)) {
      vsl_b_ifstream ifs_in(inner_region_file);
      vsl_b_read(ifs_in, cand_inner);
      ifs_in.close();
    }
    // check whether the leaf is entirely covered by the candidate region
    if (cand_inner.num_sheets() == 0) {
      for (unsigned i = 0; (i < cand_outer.num_sheets() && is_cand); i++)
        if (volm_utils::poly_contains(cand_outer[i], leaf->extent_))
          is_cand = false;
    }
    // load the score
    vgl_point_3d<double> h_pt;
    while (leaf->hyps_->get_next(0,1,h_pt))
    {
      if (is_cand)
        if (!volm_candidate_list::inside_candidate_region(cand_inner, cand_outer, h_pt.x(), h_pt.y()))
          continue;
      volm_conf_score score_in;
      score_idx.get_next(score_in);
      if ( h_pt == closest_pt) {
        std::cout << "  Score = ";
        score_in.print(std::cout);
        return volm_io::SUCCESS;
      }
    }
    std::cout << "ERROR: can not find score value for the gt location!\n";
    return volm_io::SUCCESS;
  }

  // start to create probability map
  std::vector<volm_tile> tiles;
  if (!volm_tile::generate_tiles(world_id(), tiles)) {
    log << "ERROR: unknown world id " << world_id() << "!\n";
    volm_io::write_error_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  if (tile_id() >= tiles.size()) {
    log << "ERROR: unknown tile id: " << tile_id() << ", there are " << tiles.size() << " tiles in world " << world_id() << "!\n";
    volm_io::write_error_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  volm_tile tile = tiles[tile_id()];
  vil_image_view<float> tile_img(3601,3601);
  tile_img.fill(-1.0f);

  // load the score to create the probability map
  std::cout << "---------  Start to create probability map for tile " << tile_id() << " in world " << world_id() << " -------------------- " << std::endl;
  std::cout << "There are " << loc_leaves.size() << " leaves for tile " << tile_id() << std::endl;
  std::stringstream score_file_pre;
  score_file_pre << out_folder() << "/conf_score_tile_" << tile_id();
  for (const auto& leaf : loc_leaves)
  {
    std::string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name() + ".bin";
    if (!vul_file::exists(score_bin_file))
      continue;  // case where leaf is entirely outside the candidate region

    volm_conf_buffer<volm_conf_score> score_idx(buffer_capacity());
    score_idx.initialize_read(score_bin_file);

    // load the candidate region
    bool is_cand = false;
    // load and check candidate region
    std::string outer_region_file = cand_folder() + "/cand_region_outer_" + leaf->get_string() + ".bin";
    vgl_polygon<double> cand_outer, cand_inner;
    cand_outer.clear();
    cand_inner.clear();
    if (vul_file::exists(outer_region_file)) {  // read the exterior boundaries of candidate region
      is_cand = true;
      vsl_b_ifstream ifs_out(outer_region_file);
      vsl_b_read(ifs_out, cand_outer);
      ifs_out.close();
    }
    std::string inner_region_file = cand_folder() + "/cand_region_inner_" + leaf->get_string() + ".bin";
    if (vul_file::exists(inner_region_file)) {
      vsl_b_ifstream ifs_in(inner_region_file);
      vsl_b_read(ifs_in, cand_inner);
      ifs_in.close();
    }
    // check whether the leaf is entirely covered by the candidate region
    if (cand_inner.num_sheets() == 0) {
      for (unsigned i = 0; (i < cand_outer.num_sheets() && is_cand); i++)
        if (volm_utils::poly_contains(cand_outer[i], leaf->extent_))
          is_cand = false;
    }

    // load the score
    vgl_point_3d<double> h_pt;
    while (leaf->hyps_->get_next(0, 1, h_pt))
    {
      if (is_cand)
        if (!volm_candidate_list::inside_candidate_region(cand_inner, cand_outer, h_pt.x(), h_pt.y()))
          continue;
      volm_conf_score score_in;
      score_idx.get_next(score_in);
      // project to image
      unsigned u, v;
      if (tile.global_to_img(h_pt.x(), h_pt.y(), u, v))
        if (score_in.score() >= tile_img(u,v))
          tile_img(u,v) = score_in.score();
    }
  }
  // save the probability image
  std::string img_name = out_folder() + "/ProbMap_float_" + tile.get_string() + ".tif";
  vil_save(tile_img, img_name.c_str());
  return volm_io::SUCCESS;
#if 0
  // input check
  if (tile_id() == 9999 || geo_folder().compare("") == 0 || index_name().compare("") == 0 || out_folder().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::stringstream log_file;
  std::stringstream log;
  log_file << out_folder() << "/log_prob_map_tile_" << tile_id() << ".xml";
  // load the geo index
  // create the candidate polygon if exists
  vgl_polygon<double> cand_out, cand_in;
  bool is_cand = false;
  cand_out.clear();  cand_in.clear();
  if (vul_file::exists(cand_file())) {
    //cand_poly = bkml_parser::parse_polygon(cand_file());
    unsigned n_out, n_in;
    vgl_polygon<double> poly = bkml_parser::parse_polygon_with_inner(cand_file(), cand_out, cand_in, n_out, n_in);
    std::cout << "candidate regions (" << cand_out.num_sheets() << " outer sheet and " << cand_in.num_sheets() << " inner sheet)are loaded from file: "
             << cand_file() << "!!!!!!!!!!" << std::endl;
    is_cand = (cand_out.num_sheets() != 0);
  }

  // load geo index locations
  std::stringstream file_name_pre;
  file_name_pre << geo_folder() << "/geo_index_tile_" << tile_id();
  if (!vul_file::exists(file_name_pre.str() + ".txt")) {
    log << "ERROR: loading geo index locations fails from file: " << file_name_pre.str() << ".txt!\n";
    volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  std::vector<volm_geo_index_node_sptr> loc_leaves_all;
  loc_leaves_all.clear();
  volm_geo_index::get_leaves_with_hyps(root, loc_leaves_all);

  // obtain the desired leaf
  std::vector<volm_geo_index_node_sptr> loc_leaves;
  for (unsigned i = 0; i < loc_leaves_all.size(); i++)
    if (is_cand && vgl_intersection(loc_leaves_all[i]->extent_, cand_out))
      loc_leaves.push_back(loc_leaves_all[i]);
    else
      loc_leaves.push_back(loc_leaves_all[i]);

  if (query_score())
  {
    unsigned hyp_id;
    volm_geo_index_node_sptr leaf = volm_geo_index::get_closest(root, gt_lat(), gt_lon(), hyp_id);
    if (!leaf) {
      std::cout << "query location " << gt_lon() << ", " << gt_lat() << " is not in location database" << std::endl;
      return volm_io::SUCCESS;
    }
    vgl_point_3d<double> closest_pt = leaf->hyps_->locs_[hyp_id];
    // load associate score for it
    std::stringstream score_file_pre;
    score_file_pre << out_folder() << "/conf_score_tile_" << tile_id();
    std::string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name() + ".bin";
    double dist = distance_in_meter(gt_lon(), gt_lat(), closest_pt.x(), closest_pt.y());
    std::cout << "location " << std::setprecision(6) << std::fixed << gt_lon() << ", " << std::setprecision(6) << std::fixed << gt_lat()
             << " is in leaf " << leaf->extent_
             << " and closest location (" << dist << " meter away) is "
             << std::setprecision(6) << std::fixed << closest_pt.x() << ", " << std::setprecision(6) << std::fixed << closest_pt.y() << std::endl;
    if (!vul_file::exists(score_bin_file)) {
      std::cout << "No score for the query location (probably outside candidate region)" << std::endl;
      return volm_io::SUCCESS;
    }
    volm_conf_buffer<volm_conf_score> score_idx(buffer_capacity());
    std::cout << leaf->hyps_->locs_.size() << " locations are in leaf" << std::endl;
    score_idx.initialize_read(score_bin_file);
    // loop over the location and retrieve the score
    vgl_point_3d<double> h_pt;
    while (leaf->hyps_->get_next(0,1,h_pt)) {
#if 0
      if (is_cand && !volm_candidate_list::inside_candidate_region(cand_in, cand_out, h_pt.x(), h_pt.y()))
        continue;
#endif
      if (is_cand && !volm_candidate_list::inside_candidate_region(cand_out, h_pt.x(), h_pt.y()))
        continue;
      volm_conf_score score_in;
      score_idx.get_next(score_in);
      if ( h_pt == closest_pt ) {
        std::cout << "  Score = ";
        score_in.print(std::cout);
        return volm_io::SUCCESS;
      }
    }
    std::cout << "No score for the query location (probably outside candidate region)" << std::endl;
      return volm_io::SUCCESS;
  }
  // start to create the probability map
  std::vector<volm_tile> tiles;
  if (!volm_tile::generate_tiles(world_id(), tiles)) {
    log << "ERROR: unknown world id " << world_id() << "!\n";
    volm_io::write_error_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  if (tile_id() >= tiles.size())
  {
    log << "ERROR: unknown tile id: " << tile_id() << ", there are " << tiles.size() << " tiles in world " << world_id() << "!\n";
    volm_io::write_error_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_tile tile = tiles[tile_id()];
  vil_image_view<float> tile_img(3601, 3601);
  tile_img.fill(-1.0f);

  // load the score to create the probability map
  std::cout << "---------  Start to create probability map for tile " << tile_id() << " in world " << world_id() << " -------------------- " << std::endl;
  std::cout << "There are " << loc_leaves.size() << " leaves for tile " << tile_id() << std::endl;
  std::stringstream score_file_pre;
  score_file_pre << out_folder() << "/conf_score_tile_" << tile_id();
  for (unsigned i = 0; i < loc_leaves.size(); i++)
  {
    volm_geo_index_node_sptr leaf = loc_leaves[i];
    std::string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name() + ".bin";
    if (!vul_file::exists(score_bin_file))
      continue;
    volm_conf_buffer<volm_conf_score> score_idx(buffer_capacity());
    score_idx.initialize_read(score_bin_file);
    vgl_point_3d<double> h_pt;
    while (leaf->hyps_->get_next(0,1,h_pt))
    {
      if (is_cand && !volm_candidate_list::inside_candidate_region(cand_in, cand_out, h_pt.x(), h_pt.y()))
        continue;
      volm_conf_score score_in;
      score_idx.get_next(score_in);
      // project to image
      unsigned u, v;
      if (tile.global_to_img(h_pt.x(), h_pt.y(), u, v)) {
        if (score_in.score() >= tile_img(u,v)) {
          tile_img(u,v) = score_in.score();
        }
      }
    }
  }
  // save the probability image
  std::string img_name = out_folder() + "/ProbMap_float_" + tile.get_string() + ".tif";
  vil_save(tile_img, img_name.c_str());
  return volm_io::SUCCESS;
#endif
}
