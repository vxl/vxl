#include <testlib/testlib_test.h>
#include <boxm2/volm/conf/boxm2_volm_conf_matcher.h>
#include <volm/conf/volm_conf_query.h>
#include <volm/conf/volm_conf_buffer.h>
#include <vul/vul_timer.h>
#include <bkml/bkml_parser.h>

static void test_volm_conf_matcher()
{
  // load the candidate region
  vcl_string cand_file = "d:/work/find/conf_matcher_expt/p1a_test1_40/p1a_test1_40_Candidate_test.kml";
  vgl_polygon<double> cand_poly;
  cand_poly.clear();
  bool is_cand;
  if (vul_file::exists(cand_file)) {
    cand_poly = bkml_parser::parse_polygon(cand_file);
    is_cand = (cand_poly.num_sheets() != 0);
  }
  vcl_cout << "candidate region has " << cand_poly[0].size() << " vertices and is loaded from " << cand_file << vcl_endl;
  // load the depth_map_scene
  vcl_string depth_scene_file = "d:/work/find/conf_matcher_expt/p1a_test1_40/p1a_test1_40.vsl";
  if (!vul_file::exists(depth_scene_file)) {
    vcl_cout << "can not find file: " << depth_scene_file << vcl_endl;
    return;
  }
  depth_map_scene_sptr dms = new depth_map_scene;
  vsl_b_ifstream is(depth_scene_file.c_str());
  dms->b_read(is);
  is.close();

  unsigned ni = dms->ni();
  unsigned nj = dms->nj();
  vcl_cout << "query image size: " << ni << "x" << nj << vcl_endl;
  vcl_cout << "input depth map scene has following labeled object:" << vcl_endl;
  unsigned num_of_ref_objects = 0;
  if (dms->sky().size()) {
    vcl_cout << "----- sky -----" << vcl_endl;
    for (unsigned i = 0; i < dms->sky().size(); i++) {
      vcl_cout << "name = " << dms->sky()[i]->name() << ", is_ref = " << dms->sky()[i]->is_ref() << ", land = sky, orientation = invalid, min_dist = 1E6" << vcl_endl;
    }
  }
  if (dms->ground_plane().size()) {
    vcl_cout << "----- ground -----" << vcl_endl;
    for (unsigned i = 0; i < dms->ground_plane().size(); i++) {
      vcl_cout << "name = " << dms->ground_plane()[i]->name() << ", is_ref = " << dms->ground_plane()[i]->is_ref()
               << ", land = " << volm_osm_category_io::volm_land_table[dms->ground_plane()[i]->land_id()].name_ << ", orient = " << dms->ground_plane()[i]->orient_type()
               << ", min_dist = " << dms->ground_plane()[i]->min_depth() << vcl_endl;
      if (dms->ground_plane()[i]->is_ref())
        num_of_ref_objects++;
    }
  }
  if (dms->scene_regions().size()) {
    vcl_cout << "----- label object -----" << vcl_endl;
    for (unsigned i = 0; i < dms->scene_regions().size(); i++) {
      vcl_cout << "name = " << dms->scene_regions()[i]->name() << ", is_ref = " << dms->scene_regions()[i]->is_ref()
               << ", land = " << volm_osm_category_io::volm_land_table[dms->scene_regions()[i]->land_id()].name_ << ", orient = " << dms->scene_regions()[i]->orient_type()
               << ", min_dist = " << dms->scene_regions()[i]->min_depth() << vcl_endl;
      if (dms->scene_regions()[i]->is_ref())
        num_of_ref_objects++;
    }
  }
  // create a camera space
  double head_mid=67.0,   head_radius=0.0,  head_inc=2.0;
  double tilt_mid=87.70,   tilt_radius=3.0,  tilt_inc=2.0;
  double roll_mid=-0.74,  roll_radius=0.0,  roll_inc=0.0;
  //double top_fov_vals[] = {3.0,  4.0, 5.0, 12.0, 17.0, 18.0,19.0, 20.0, 24.0};
  double top_fov_vals[] = {5.3};
  vcl_vector<double> fovs;
  fovs.push_back(top_fov_vals[0]);


  double altitude = 3.0;
  volm_camera_space_sptr csp = new volm_camera_space(fovs, altitude, ni, nj,
                                                     head_mid, head_radius, head_inc,
                                                     tilt_mid, tilt_radius, tilt_inc,
                                                     roll_mid, roll_radius, roll_inc);

  if (dms->ground_plane().size() > 0)  // enforce ground plane constraint if user specified a ground plane
  {
    camera_space_iterator cit = csp->begin();
    for ( ; cit != csp->end(); ++cit) {
      unsigned current = csp->cam_index();
      vpgl_perspective_camera<double> cam = csp->camera(); // camera at current state of iterator
      bool success = true;
      for (unsigned i = 0; success && i < dms->ground_plane().size(); i++)
        success = dms->ground_plane()[i]->region_ground_2d_to_3d(cam);
      if (success) // add this camera
        csp->add_camera_index(current);
    }
  }
  else
    csp->generate_full_camera_index_space();

  // create a query
  // construct a volm_conf_queue

  unsigned tol_in_pixel = 25.0;
  volm_conf_query_sptr query = new volm_conf_query(csp, dms, tol_in_pixel);

  vcl_cout << "configurational query reference object list: ";
  vcl_vector<vcl_string> ref_object_names = query->ref_obj_name();
  for (vcl_vector<vcl_string>::iterator vit = ref_object_names.begin(); vit != ref_object_names.end(); ++vit)
    vcl_cout << (*vit) << ", ";
  vcl_cout << vcl_endl;

  vcl_cout << "configurational query camera list: " << vcl_endl;
  vcl_vector<vcl_string> cam_string = query->camera_strings();
  for (vcl_vector<vcl_string>::iterator vit = cam_string.begin();  vit != cam_string.end();  ++vit)
    vcl_cout << "\t" << (*vit) << vcl_endl;
  vcl_cout << query->cameras().size() << " perspective cameras are created." << vcl_endl;
  TEST("configurational query cameras", query->cameras().size(), query->camera_angles().size());
  TEST("configurational query reference objects", query->ref_obj_name().size(), num_of_ref_objects);

  vcl_cout << "configurational query has following reference object: " << vcl_endl;
  vcl_vector<vcl_string> query_ref_obj = query->ref_obj_name();
  for (vcl_vector<vcl_string>::iterator vit = query_ref_obj.begin();  vit != query_ref_obj.end();  ++vit)
    vcl_cout << " " << (*vit);
  vcl_cout << vcl_endl;

  vcl_cout << "configurational query has following configuration object: " << vcl_endl;
  vcl_vector<vcl_map<vcl_string, volm_conf_object_sptr> > conf_objs = query->conf_objects();
  for (unsigned i = 0; i < conf_objs.size(); i++) {
    vcl_cout << "\t camera: " << cam_string[i] << vcl_endl;
    for (vcl_map<vcl_string, volm_conf_object_sptr>::iterator mit = conf_objs[i].begin();  mit != conf_objs[i].end(); ++mit) {
      vcl_cout << "\t\t obj name: " << mit->first << "\t\t" ;  mit->second->print(vcl_cout);
    }
  }

  // visualize the configurational query
  vcl_string out_folder = "d:/work/find/conf_matcher_expt/p1a_test1_40_Conf2dRUN/";
  vcl_string img_file = "d:/work/find/conf_matcher_expt/p1a_test1_40/p1a_test1_40.jpg";
  query->visualize_ref_objs(img_file, out_folder);
  query->generate_top_views(out_folder);

  // --------------------------------   index   ---------------------------------------
  // load geo location
  vcl_string geo_folder = "v:/p1a_related/p1a_index/gt_only/geoindex_p1a_wr2/";
  unsigned tile_id = 3;
  int leaf_id = -1;
  vcl_stringstream file_name_pre;
  file_name_pre << geo_folder << "/geo_index_tile_" << tile_id;
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  vcl_vector<volm_geo_index_node_sptr> loc_leaves_all;
  loc_leaves_all.clear();
  volm_geo_index::get_leaves_with_hyps(root, loc_leaves_all);

  // prune leaves by the candidate region
  vcl_vector<volm_geo_index_node_sptr> loc_leaves;
  if (leaf_id >= 0 && leaf_id < (int)loc_leaves_all.size()) {
    if (is_cand && vgl_intersection(loc_leaves_all[leaf_id]->extent_, cand_poly))
      loc_leaves.push_back(loc_leaves_all[leaf_id]);
  }
  else
  {
    for (unsigned i = 0; i < loc_leaves_all.size(); i++)
      if (is_cand && vgl_intersection(loc_leaves_all[i]->extent_, cand_poly))
        loc_leaves.push_back(loc_leaves_all[i]);
  }

  unsigned num_locs = 0;
  for (unsigned i = 0; i < loc_leaves.size(); i++) {
    num_locs += loc_leaves[i]->hyps_->locs_.size();
    vcl_cout << "leaf " << i << ": " << loc_leaves[i]->extent_ << ", num of locs: " << loc_leaves[i]->hyps_->locs_.size() << vcl_endl;
  }
  vcl_string index_folder = "v:/p1a_related/p1a_index/gt_only/geoindex_conf_2d_radius_1500/";

  // create a matcher
  vul_timer t;
  boxm2_volm_conf_matcher matcher(query, tile_id, loc_leaves, index_folder, out_folder, cand_poly);
  vcl_string index_name = "conf_2d";
  int matched_loc = matcher.conf_match_cpp(index_name, leaf_id);
  TEST("configuration matcher", matched_loc != 0, true);
  vcl_cout << "Matching " << matched_loc << " consumes " << t.all()/(1000.0*60.0) << " seconds" << vcl_endl;

  // read the score binary into the file
  vcl_cout << "Reading matching scores: " << vcl_endl;
  loc_leaves_all.clear();
  loc_leaves.clear();
  volm_geo_index_node_sptr root_in = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
  volm_geo_index::read_hyps(root_in, file_name_pre.str());
  volm_geo_index::get_leaves_with_hyps(root, loc_leaves_all);
  // prune leaves by the candidate region
  if (leaf_id >= 0 && leaf_id < (int)loc_leaves_all.size()) {
    if (is_cand && vgl_intersection(loc_leaves_all[leaf_id]->extent_, cand_poly))
      loc_leaves.push_back(loc_leaves_all[leaf_id]);
  }
  else
  {
    for (unsigned i = 0; i < loc_leaves_all.size(); i++)
      if (is_cand && vgl_intersection(loc_leaves_all[i]->extent_, cand_poly))
        loc_leaves.push_back(loc_leaves_all[i]);
  }


  volm_geo_index::get_leaves_with_hyps(root_in, loc_leaves);

  vcl_stringstream score_file_pre;
  score_file_pre << out_folder << "/conf_score_tile_" << tile_id;
  for (unsigned i = 0; i < loc_leaves.size(); i++)
  {
    volm_geo_index_node_sptr leaf = loc_leaves[i];
    vcl_string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name + ".bin";
    volm_conf_buffer<volm_conf_score> score_idx(2.0f);
    score_idx.initialize_read(score_bin_file);
    vgl_point_3d<double> h_pt;
    while (leaf->hyps_->get_next(0, 1, h_pt))
    {
      if (is_cand && !cand_poly.contains(h_pt.x(), h_pt.y()))
        continue;
      vcl_cout << "location: " << h_pt.x() << ", " << h_pt.y() << " (tile " << tile_id << "): -->";
      volm_conf_score score_in;
      score_idx.get_next(score_in);
      score_in.print(vcl_cout);
    }
    score_idx.finalize();
  }
  

}

TESTMAIN( test_volm_conf_matcher );