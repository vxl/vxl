#include <testlib/testlib_test.h>
#include <volm/desc/volm_desc_ex_matcher.h>

static void test_ex_matcher()
{
  // query input
  vcl_string depth_scene = "z:/projects/FINDER/test1/p1a_test1_36/p1a_test1_36.vsl";
  // index and geolocation
  vcl_string geo_hypo_a = "z:/projects/FINDER/index/geoindex_zone_17_inc_0.99_nh_200/";
  vcl_string geo_hypo_b = "z:/projects/FINDER/index/geoindex_zone_18_inc_0.99_nh_200/";
  vcl_string desc_index_folder = "d:/work/find/desc_index/radius_100_500_1000_inc_0.99_nh_200_pa_2/";
  vcl_string out_folder = "d:/work/find/volm_matcher/test1_result/output/volm_results_all_pa_2/p1a_test1_36/p1a_test1_36-VisibilityRUN/";
  float buffer_capacity = 2.0f;

  vcl_string gt_file = "z:/projects/FINDER/test1/p1a_test1_gt_locs.txt";
  float kl = 200.0f;
  float ku = 10.0f;
  float thres_value = 0.53215;
  unsigned test_id = 1;
  unsigned id = 36;
  unsigned thresc = 210;
  unsigned top_size = 1;
  // error log

  // load the depth map scene
  depth_map_scene_sptr dms = new depth_map_scene;
  vsl_b_ifstream dis(depth_scene.c_str());
  dms->b_read(dis);
  dis.close();

  // fetch the ground truth location
  vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_string, vcl_string> > > samples;
  unsigned int cnt = volm_io::read_gt_file(gt_file, samples);
  vgl_point_3d<double> gt_loc;
  gt_loc = samples[id].first;

  // create volm_tile (coast only)
  vcl_vector<volm_tile> tiles = volm_tile::generate_p1_wr2_tiles();

  // create ex_matcher
  vcl_string params_file_pre = desc_index_folder + "/desc_index_tile_0";
  volm_buffered_index_params params;
  if (!params.read_ex_param_file(params_file_pre)) {
      vcl_cerr << " param loading error " << vcl_endl;
  }
  volm_desc_matcher_sptr ex_matcher = new volm_desc_ex_matcher(dms, params.radius, params.norients, params.nlands, 0);

  // create query
  volm_desc_sptr query = ex_matcher->create_query_desc();

  // Start the matcher
#if 1
  for (unsigned tile_id = 0; tile_id < tiles.size(); tile_id++) {
    
    if (tile_id == 10)
      continue;
    vcl_cout << " start matcher for tile " << tile_id << "... " << vcl_endl;
    vcl_string geo_hypo_folder;
    if (tile_id < 8 && tile_id != 5)
      geo_hypo_folder = geo_hypo_a;
    else
      geo_hypo_folder = geo_hypo_b;

    if (!ex_matcher->matcher(query, geo_hypo_folder, desc_index_folder, buffer_capacity, tile_id))
      vcl_cerr << " matcher for tile " << tile_id << " failed" << vcl_endl;
    // save the score binary
    if (!ex_matcher->write_out(out_folder, tile_id))
      vcl_cerr << " write out for tile " << tile_id << " failed" << vcl_endl;
  }
#endif

#if 1
  // Create probability map
  float gt_score = 0.0f;
  for (unsigned tile_id = 0; tile_id < tiles.size(); tile_id++) {
    vcl_cout << " create probability for tile " << tile_id << "... " << vcl_endl;
    float gt_score_tile = 0.0f;
    vcl_string geo_hypo_folder;
    if (tile_id < 8 && tile_id != 5)
      geo_hypo_folder = geo_hypo_a;
    else
      geo_hypo_folder = geo_hypo_b;
    if (!ex_matcher->create_prob_map(geo_hypo_folder, out_folder, tile_id, tiles[tile_id], gt_loc, gt_score_tile))
      vcl_cerr << " creating probability map failed for tile " << tile_id << vcl_endl;
    vcl_cout << "in tile " << tile_id << ", gt_score = " << gt_score_tile << vcl_endl;
    if (gt_score_tile != 0)
      gt_score = gt_score_tile;
  }
  vcl_cout << "eventually, gt_score = " << gt_score << vcl_endl;
#endif

  // Create scaled probability map
  // check input threshold
  float thres_value_bef = thres_value;
  ex_matcher->check_threshold(query, thres_value);
  if (thres_value > gt_score)
    thres_value = gt_score - 1.0f/query->get_area();
  vcl_cout << " thres_value_bef = " << thres_value_bef << ", after threscheck, thres_value = " << thres_value << vcl_endl;
  vcl_cout << " create scaled probability given threshold " << thres_value << vcl_endl;
  for (unsigned tile_id = 0; tile_id < tiles.size(); tile_id++) {
    ex_matcher->create_scaled_prob_map(out_folder, tiles[tile_id], tile_id, ku, kl, thres_value);
  }

  vcl_cout << " done " << vcl_endl;

#if 0
  // Start the matcher
  float gt_score = 0;
  unsigned num_valid_bin = 0;
  vcl_vector<volm_tile> tiles = volm_tile::generate_p1_wr2_tiles();
  for (unsigned tile_id = 0; tile_id < tiles.size(); tile_id++) {
    vcl_string geo_hypo_folder;
    if (tile_id < 8 && tile_id != 5)
      geo_hypo_folder = geo_hypo_a;
    else
      geo_hypo_folder = geo_hypo_b;
    vcl_stringstream params_file;
    params_file << desc_index_folder << "/desc_index_tile_" << tile_id;
    volm_buffered_index_params params;
    if (!params.read_ex_param_file(params_file.str())) {
      vcl_cerr << " param loading error " << vcl_endl;
    }

    volm_desc_matcher_sptr ex_matcher = new volm_desc_ex_matcher(dms, params.radius, params.norients, params.nlands, 0);

    // create query
    volm_desc_sptr query = new volm_desc_ex();
    query = ex_matcher->create_query_desc();
    num_valid_bin = query->get_area();

    if (tile_id != 10) {
      if (!ex_matcher->matcher(query, geo_hypo_folder, desc_index_folder, buffer_capacity, tile_id))
      vcl_cerr << " matcher for tile " << tile_id << " failed" << vcl_endl;
      // save the score binary
      if (!ex_matcher->write_out(out_folder, tile_id))
        vcl_cerr << " write out for tile " << tile_id << " failed" << vcl_endl;
    }

    // generate probability map for this tile
    ex_matcher->create_prob_map(geo_hypo_folder, out_folder, tile_id, tiles[tile_id], gt_loc, gt_score);
    vcl_cout << "in tile " << tile_id << ", gt_score = " << gt_score << vcl_endl;

    // generate scaled probability map
    volm_desc_matcher::create_scaled_prob_map(out_folder, tiles[tile_id], tile_id, ku, kl, query->get_area(), thres_ratio);
  }

  // generate scaled probability map

  //for (unsigned tile_id = 0; tile_id < tiles.size(); tile_id++) {
  //  volm_desc_matcher::create_scaled_prob_map(out_folder, tiles[tile_id], tile_id, ku, kl, num_valid_bin, thres_ratio);
  //}
#endif

  // generate candidate lists
  vcl_stringstream prob_map_folder;
  prob_map_folder << out_folder << "/ProbMap_scaled_" << thres_value;
  vcl_stringstream cand_folder;
  cand_folder << out_folder << "/T_" << thresc ;
  if( !vul_file::is_directory(cand_folder.str()))
    vul_file::make_directory(cand_folder.str());
  volm_desc_matcher::create_candidate_list(prob_map_folder.str(), cand_folder.str(), thresc, top_size, ku, kl, thres_value, test_id, id);
}

TESTMAIN( test_ex_matcher );
