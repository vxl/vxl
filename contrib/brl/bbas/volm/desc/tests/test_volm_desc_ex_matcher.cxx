#include <testlib/testlib_test.h>
#include <volm/desc/volm_desc_ex_matcher.h>

static void test_ex_matcher()
{
  // query input
  vcl_string depth_scene = "z:/projects/FINDER/test1/p1a_test1_20/p1a_test1_20.vsl";
  // index and geolocation
  vcl_string geo_hypo_a = "z:/projects/FINDER/index/geoindex_zone_17_inc_0.99_nh_200/";
  vcl_string geo_hypo_b = "z:/projects/FINDER/index/geoindex_zone_18_inc_0.99_nh_200/";
  vcl_string desc_index_folder = "d:/work/find/desc_index/radius_100_500_1000_inc_0.99_nh_200_pa_2/";
  vcl_string out_folder = "d:/work/find/volm_matcher/test1_result/output/volm_results_all_pa_2/p1a_test1_20/p1a_test1_20-VisibilityRUN/";
  float buffer_capacity = 2.0f;

  vcl_string gt_file = "z:/projects/FINDER/test1/p1a_test1_gt_locs.txt";
  float kl = 200.0f;
  float ku = 10.0f;
  double thres_ratio = 0.5;
  unsigned test_id = 1;
  unsigned id = 20;
  unsigned thresc = 130;
  unsigned top_size = 2;
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

  // Start the matcher
  float gt_score = 0;
  vcl_vector<volm_tile> tiles = volm_tile::generate_p1_wr2_tiles();
  for (unsigned tile_id = 0; tile_id < tiles.size(); tile_id++) {
    if (tile_id != 1 && tile_id != 6)
      continue;
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

    if (!ex_matcher->matcher(query, geo_hypo_folder, desc_index_folder, buffer_capacity, tile_id))
      vcl_cerr << " matcher for tile " << tile_id << " failed" << vcl_endl;

    // save the score binary
    if (!ex_matcher->write_out(out_folder, tile_id))
      vcl_cerr << " write out for tile " << tile_id << " failed" << vcl_endl;

    // generate probability map for this tile
    ex_matcher->create_prob_map(geo_hypo_folder, out_folder, tile_id, tiles[tile_id], gt_loc, gt_score);
    vcl_cout << "gt_score = " << vcl_endl;
  }

  

  // generate scaled probability map

  // generate candidate lists
}

TESTMAIN( test_ex_matcher );
