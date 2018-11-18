#include "volm_desc_matcher.h"
//:
// \file
#include <vul/vul_file.h>
#include <vgl/vgl_point_3d.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <volm/volm_candidate_list.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_intersection.h>

bool volm_desc_matcher::matcher(volm_desc_sptr const& query,
                                std::string const& geo_hypo_folder,
                                std::string const& desc_index_folder,
                                float buffer_capacity,
                                unsigned const& tile_id)
{
  // load the volm_geo_index for this tile
  std::stringstream file_name_pre;
  file_name_pre << geo_hypo_folder << "/geo_index_tile_" << tile_id;
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  std::vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);
  std::stringstream index_file_name_pre;
  index_file_name_pre << desc_index_folder << "/desc_index_tile_" << tile_id;

  // obtain the parameters for index
  volm_buffered_index_params params;
  if (!params.read_params_file(index_file_name_pre.str())) {
    std::cerr << " ERROR: can not open index parameter file: " << vul_file::strip_extension(index_file_name_pre.str()) << ".params\n";
    return false;
  }

  volm_buffered_index_sptr ind = new volm_buffered_index(params.layer_size, buffer_capacity);

  // clear score_all_ to ensure the score_all_ only stores information for current tile
  if (!score_all_.empty())
    score_all_.clear();

  // check the consistency of query size and loaded index size
  if (query->nbins() != params.layer_size) {
    std::cerr << " ERROR: created query and loaded index have different descriptor\n";
    return false;
  }

  // loop over all leaves to match each location with the query
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    std::string index_file = index_file_name_pre.str() + "_" + leaves[l_idx]->get_string() + "_" + this->get_index_type_str() + ".bin";
    if (!vul_file::exists(index_file)) {
      std::cout << " ERROR: can not find index file: " << index_file << std::endl;
      return false;
    }
    ind->initialize_read(index_file);
    vgl_point_3d<double> h_pt;

    // loop over all location in current leaf
    while (leaves[l_idx]->hyps_->get_next(0,1,h_pt)) {
      unsigned h_idx = leaves[l_idx]->hyps_->current_-1;
      // load the histogram for current location h_pt
      std::vector<unsigned char> values(params.layer_size);
      ind->get_next(values);

      volm_desc_sptr index_desc = new volm_desc(values);
#if 0
      std::cout << " location: " << h_pt << std::endl;
      std::cout << "\t: ";
      index_desc->print();
      std::cout << std::endl;
#endif
      // calculate score which measures the similarity of the query and index at current location
      float max_score = 0;
      max_score = this->score(query, index_desc);
      // save the score
      std::vector<unsigned> cam_ids;
      cam_ids.push_back(0);
      score_all_.push_back(new volm_score(l_idx, h_idx, max_score, 0, cam_ids) );
    }
    // finish current leaf
    ind->finalize();
  }
  return true;
}

bool volm_desc_matcher::write_out(std::string const& out_folder, unsigned const& tile_id)
{
  std::stringstream filename;
  filename << out_folder << "/" << this->get_index_type_str() << "_scores_tile_" << tile_id << ".bin";
  volm_score::write_scores(score_all_, filename.str());
  return true;
}

bool volm_desc_matcher::create_prob_map(std::string const& geo_hypo_folder,
                                        std::string const& out_folder,
                                        unsigned const& tile_id,
                                        volm_tile tile,
                                        vgl_point_3d<double> const& gt_loc,
                                        float& gt_score)
{
  // initialize the probability map image
  vil_image_view<float> tile_img(3601, 3601);
  tile_img.fill(-1.0f);

  // create an empty image for tile 10
  if (tile_id == 10) {
    std::string img_name = out_folder + "/ProbMap_float_" + tile.get_string() + ".tif";
    vil_save(tile_img, img_name.c_str());
    return true;
  }

  // load the geo_index for current tile
  std::stringstream file_name_pre;
  file_name_pre << geo_hypo_folder << "geo_index_tile_" << tile_id;
  if (!vul_file::exists(file_name_pre.str() + ".txt"))
    return false;
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  std::vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);

  // get the closest geolocation from the gt location
  double sec_to_meter = 21.0/0.000202;
  double gt_lon, gt_lat;
  gt_lon = gt_loc.x();  gt_lat = gt_loc.y();
  unsigned hyp_gt = 0;
  vgl_point_3d<double> gt_closest;
  volm_geo_index_node_sptr leaf_gt = volm_geo_index::get_closest(root, gt_lat, gt_lon, hyp_gt);
  // check the distance from ground trugh location to the closest in geo_index
  if (leaf_gt) {
    gt_closest = leaf_gt->hyps_->locs_[hyp_gt];
    const double x_dist = vnl_math::abs(gt_loc.x()-gt_closest.x())*sec_to_meter;
    const double y_dist = vnl_math::abs(gt_loc.y()-gt_closest.y())*sec_to_meter;
    vgl_vector_2d<double> gt_dist_vec(x_dist, y_dist);
  }
  else {
    gt_closest = gt_loc;
  }

  // load the score binary from output folder if exists
  std::stringstream score_file;
  score_file << out_folder << "/" << this->get_index_type_str() << "_scores_tile_" << tile_id << ".bin";
  // continue if no score binary exists for this tile
  if (!vul_file::exists(score_file.str())) {
    std::cerr << " WARNING: score binary result is missing for tile " << tile_id << '\n';
    return false;
  }
  std::vector<volm_score_sptr> scores;
  volm_score::read_scores(scores, score_file.str());
  // refill the image
  auto total_ind = (unsigned)scores.size();
  for (unsigned ii = 0; ii < total_ind; ii++) {
    vgl_point_3d<double> h_pt = leaves[scores[ii]->leaf_id_]->hyps_->locs_[scores[ii]->hypo_id_];
    if (gt_closest == h_pt)
      gt_score = scores[ii]->max_score_;
    //std::cout << "locs: " << h_pt.x() << ", " << h_pt.y() << ", score = " << scores[ii]->max_score_ << std::endl;
    unsigned u, v;
    if (tile.global_to_img(h_pt.x(), h_pt.y(), u, v))
      if (u < tile_img.ni() && v < tile_img.nj())
        if (scores[ii]->max_score_ > tile_img(u,v))
          tile_img(u,v) = scores[ii]->max_score_;
  }
  // save the image
  std::string img_name = out_folder + "/ProbMap_float_" + tile.get_string() + ".tif";
  vil_save(tile_img, img_name.c_str());
  return true;
}

bool volm_desc_matcher::create_empty_prob_map(std::string const& out_folder, unsigned  /*tile_id*/, volm_tile& tile)
{
  // initialize the probability map image
  vil_image_view<float> tile_img(3601, 3601);
  tile_img.fill(-1.0f);
  // save the image
  std::string img_name = out_folder + "/ProbMap_float_" + tile.get_string() + ".tif";
  vil_save(tile_img, img_name.c_str());
  return true;
}

//: need a specialized prob map generator for a random matcher, cause random matcher do not need to create a desc_index nor save binary score files
bool volm_desc_matcher::create_random_prob_map(vnl_random& rng, std::string const& geo_hypo_folder, std::string const& out_folder, unsigned tile_id, volm_tile& tile)
{

  // load the volm_geo_index for this tile
  std::stringstream file_name_pre;
  file_name_pre << geo_hypo_folder << "/geo_index_tile_" << tile_id;
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  std::vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);

  // initialize the probability map image
  vil_image_view<float> tile_img(3601, 3601);
  tile_img.fill(-1.0f);

  // generate a random score for only the pixels where there is a hyp
  for (auto & leave : leaves) {
      vgl_point_3d<double> h_pt;
      // loop over all locations in current leaf
      while (leave->hyps_->get_next(0,1,h_pt)) {
        unsigned u, v;
        if (tile.global_to_img(h_pt.x(), h_pt.y(), u, v) && tile_img(u,v) < 0) // this method checks the image boundaries
          tile_img(u,v) = (float)rng.drand32();
      }
  }

  // save the image
  std::string img_name = out_folder + "/ProbMap_float_" + tile.get_string() + ".tif";
  vil_save(tile_img, img_name.c_str());
  return true;
}


bool volm_desc_matcher::create_scaled_prob_map(std::string const& out_folder,
                                               volm_tile tile,
                                               unsigned const&  /*tile_id*/,
                                               float const& ku,
                                               float const& kl,
                                               float const& threshold)
{
  std::string img_name = out_folder + "/ProbMap_float_" + tile.get_string() + ".tif";
  if (!vul_file::exists(img_name))
    return false;
  vil_image_view<float> tile_img = vil_load(img_name.c_str());
  vil_image_view<vxl_byte> out_png(tile_img.ni(), tile_img.nj());
  out_png.fill(volm_io::UNKNOWN);
  for (unsigned i = 0; i < tile_img.ni(); i++)
    for (unsigned j = 0; j < tile_img.nj(); j++)
      if (tile_img(i, j) >= 0)
        out_png(i,j) = volm_io::scale_score_to_1_255_sig(kl, ku, threshold, tile_img(i,j));

  // save the image to specific folder
  std::stringstream prob_map_folder;
  //prob_map_folder << out_folder << "/ProbMap_scaled_" << threshold;
  prob_map_folder << out_folder;
  if( !vul_file::is_directory(prob_map_folder.str()))
    vul_file::make_directory(prob_map_folder.str());
  std::string out_img_name = prob_map_folder.str() + "/ProbMap_" + tile.get_string() + ".tif";
  vil_save(out_png, out_img_name.c_str());

  return true;
}

bool volm_desc_matcher::create_candidate_list(std::string const& prob_map_folder,
                                              std::string const& cand_root,
                                              unsigned const& threshold,
                                              unsigned const& top_size,
                                              float const& ku,
                                              float const& kl,
                                              float const& thres_ratio,
                                              std::string const& query_name,
                                              std::string const& world_str)
{
  // create volm_tiles (coast only)
  std::vector<volm_tile> tiles;
  if (world_str == "coast")             tiles = volm_tile::generate_p1_wr2_tiles();
  else if (world_str == "desert")       tiles = volm_tile::generate_p1_wr1_tiles();
  else if (world_str == "Chile")        tiles = volm_tile::generate_p1b_wr1_tiles();
  else if (world_str == "India")        tiles = volm_tile::generate_p1b_wr2_tiles();
  else if (world_str == "Jordan")       tiles = volm_tile::generate_p1b_wr3_tiles();
  else if (world_str == "Philippines")  tiles = volm_tile::generate_p1b_wr4_tiles();
  else if (world_str == "Taiwan")       tiles = volm_tile::generate_p1b_wr5_tiles();
  else {
    return false;
  }
  auto n_tile = (unsigned)tiles.size();

  float thres_scale = thres_ratio;
  double thres_value = volm_io::scale_score_to_0_1_sig(kl, ku, (float)thres_scale, threshold);
  if( !vul_file::is_directory(prob_map_folder)) {
    std::cerr << " ERROR: can not find scaled probability map folder: " << prob_map_folder << std::endl;
    return false;
  }

  // create candidate list for each tile
  std::vector<volm_candidate_list> cand_lists;
  for (unsigned t_idx = 0; t_idx < n_tile; t_idx++) {
    std::string img_name = prob_map_folder + "/ProbMap_" + tiles[t_idx].get_string() + ".tif";
    if (!vul_file::exists(img_name)) {
      vil_image_view<vxl_byte> tile_img(tiles[t_idx].ni(), tiles[t_idx].nj());
      tile_img.fill(volm_io::UNKNOWN);
      volm_candidate_list cand_list(tile_img, threshold);
      cand_lists.push_back(cand_list);
    }
    else {
      vil_image_view<vxl_byte> tile_img = vil_load(img_name.c_str());
      volm_candidate_list cand_list(tile_img, threshold);
      cand_lists.push_back(cand_list);
    }
  }

#if 0
  // visualize the candidate list
  for (unsigned t_idx = 0; t_idx < n_tile; t_idx++) {
    vil_image_view<vxl_byte> cand_img;
    std::string img_name = cand_root + "/Candidate_list_" + tiles[t_idx].get_string() + ".tif";
    cand_lists[t_idx].candidate_list_image(cand_img);
    vil_save(cand_img, img_name.c_str());
  }
#endif
  // create the map for all regions
  std::multimap<unsigned, std::pair<unsigned, unsigned>, std::greater<unsigned> > cand_map;
  for (unsigned tile_idx = 0; tile_idx < tiles.size(); tile_idx++) {
    std::vector<unsigned> region_score;
    if (!cand_lists[tile_idx].region_score(region_score)) {
      std::cerr << " ERROR: can not get best scores for candidate regions in tile " << tile_idx << '\n';
      return false;
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
  kml_name << query_name;
  std::string cam_kml = cand_root + "/" + kml_name.str() + "-CANDIDATE.kml";
  std::ofstream ofs_kml(cam_kml.c_str());

  std::cout << " there are " << cand_map.size() << " candidate regions given threshold = " << threshold << " (likelihood = " << thres_value << ")\n";
  volm_candidate_list::open_kml_document(ofs_kml,kml_name.str(),(float)thres_value);

  auto mit = cand_map.begin();
  unsigned rank = 0;
  for (; mit != cand_map.end(); ++mit) {
    unsigned tile_idx = mit->second.first;
    unsigned sh_idx = mit->second.second;
    //std::string geo_hypo_folder;
    //if (tile_idx < 8 && tile_idx != 5)
    //  geo_hypo_folder = geo_hypo_folder_a;
    //else
    //  geo_hypo_folder = geo_hypo_folder_b;
    // transfer loc_image to loc_global
    std::vector<vgl_point_2d<double> > region_loc_global;
    cand_lists[tile_idx].img_to_golbal(sh_idx, tiles[tile_idx], region_loc_global);
    // obtain the top locations
    std::vector<vgl_point_2d<double> > top_locs;
    std::vector<unsigned> top_loc_scores;
    cand_lists[tile_idx].top_locations(top_locs, top_loc_scores, tiles[tile_idx], top_size, sh_idx);
    // obtain the top cameras given the top locations
    std::vector<cam_angles> top_cameras;
    std::vector<double> right_fov;
    for (unsigned idx = 0; idx < top_locs.size(); idx++) {
      top_cameras.emplace_back(2.64, 15, 334.0, 91.34);
      right_fov.push_back(20.0);
    }
    // calculate the likelihood based on the top scores
    float likelihood = volm_io::scale_score_to_0_1_sig(kl, ku, (float)thres_scale, (unsigned char)mit->first);
    // write the region polygon and top locations
    volm_candidate_list::write_kml_regions(ofs_kml, region_loc_global, top_locs, top_cameras, right_fov, likelihood, rank++);
    for (unsigned i = 0 ; i < top_locs.size(); i++) {
      if (rank%100 == 0)
        std::cerr << " size = " << cand_map.size() << ", cnt = " << rank-1 << " score = " << mit->first << " --> tile = " << tile_idx << " sh_id = " << sh_idx
                 << " --> top_locs = " << std::setprecision(10) << top_locs[i].x() << ", " << top_locs[i].y() << " top_score = " << top_loc_scores[i] << std::endl;
    }
  }
  volm_candidate_list::close_kml_document(ofs_kml);
  return true;
}
