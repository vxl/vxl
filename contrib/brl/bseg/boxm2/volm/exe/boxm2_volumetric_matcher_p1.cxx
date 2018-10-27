//:
// \file
// \brief executable to use object/ray based matcher for query image using obj_min_dist, obj_order, ground_depth, sky_region and orientation
// \author Yi Dong
// \date Feb 05, 2013


#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vpgl/vpgl_utm.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <boxm2/volm/boxm2_volm_matcher_p1.h>
#include <bbas/bocl/bocl_manager.h>
#include <bbas/bocl/bocl_device.h>
#include <bkml/bkml_parser.h>

int main(int argc, char** argv)
{
  // input
  vul_arg<std::string> cam_bin("-cam", "camera space binary", "");                                // query -- camera space binary
  vul_arg<std::string> dms_bin("-dms", "depth_map_scene binary", "");                             // query -- depth map scene
  vul_arg<std::string> sph_bin("-sph", "spherical shell binary", "");                             // query -- spherical shell container binary
  vul_arg<std::string> query_bin("-query", "query binary file", "");                              // query -- query binary file
  vul_arg<std::string> weight_file("-wgt", "weight parameters for query", "");                    // query -- weight parameter file
  vul_arg<std::string> geo_hypo_folder("-hypo", "folder to read the geo hypotheses", "");         // index -- folder to read the hypos for each leaf
  vul_arg<std::string> geo_index_folder("-geo", "folder to read the geo index", "");              // index -- folder to read the index for each location
  vul_arg<std::string> candidate_list("-cand", "candidate list for given query (txt file)", "");  // index -- candidate list file containing polygons
  vul_arg<float>      buffer_capacity("-buff", "index buffer capacity (GB)", 1.0f);              // index -- buffer capacity
  vul_arg<unsigned>   tile_id("-tile", "ID of the tile that current matcher consdier", 3);       // matcher -- tile id
  vul_arg<unsigned>   zone_id("-zone", "ID of the utm zone of current tile",17);                 // matcher -- zone id
  vul_arg<unsigned>   dev_id("-gpuid", "device used for current matcher", 0);                    // matcher -- device id
  vul_arg<float>      threshold("-thres", "threshold for choosing valid cameras (0~1)", 0.4f);   // matcher -- threshold for choosing cameras
  vul_arg<unsigned>   max_cam_per_loc("-max_cam", "maximum number of cameras to be saved", 200); // matcher -- output related
  vul_arg<std::string> out_folder("-out", "output folder where score binary is stored", "");      // matcher -- output folder
  vul_arg<bool>       logger("-logger", "designate one of the exes as logger", false);           // matcher -- log file generation
  vul_arg<int>        num_locs("-num-locs", "number of location passed into GPU at each kernel launch", -1);  // matcher -- number of locations passed to GPU cache per kernel launching
  vul_arg<unsigned>   gt_id("-gt", "test image id (40 or 83)", 0);                                      // for experiments
  vul_arg<std::string> gt_file("-gt_locs", "file with the gt locs of all test cases", "");               // for experiments

#if 0
  vul_arg<bool>       use_orient("-ori", "choose to use orientation attribute", false);          // matcher -- matcher option
  vul_arg<bool>        use_ps0("-ps0", "choose to use pass 0 regional matcher", false);
  vul_arg<bool>        use_ps1("-ps1", "choose to use pass 1 obj_based order matcher", false);
  vul_arg<bool>        use_ps2("-ps2", "choose to use pass 2 obj_based orient matcher", false);
  vul_arg<bool>        gt_out("-gt", "choose to output camera score for ground truth locations", false);  // for testing purpose
  vul_arg<unsigned>    gt_l_id("-gtl", "leaf id for the ground truth location",0);                         // for testing purpose
  vul_arg<unsigned>    gt_h_id("-gth", "hypo id for the ground truth location",0);                         // for testing purpose
#endif

  vul_arg_parse(argc, argv);
  bool is_last_pass = false; // no previous matcher output

  std::stringstream log;
  bool do_log = false;
  if (logger())
    do_log = true;
  // check the input parameters
  if ( cam_bin().compare("") == 0 ||
       dms_bin().compare("") == 0 ||
       sph_bin().compare("") == 0 ||
       geo_hypo_folder().compare("") == 0 ||
       geo_index_folder().compare("") == 0 ||
       query_bin().compare("") == 0 ||
       out_folder().compare("") == 0 )
  {
    log << " ERROR: input file/folders can not be empty\n";
    std::cerr << log.str();
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  if ( gt_id() != 0 && gt_file().compare("") == 0) {
    log << " ERROR: require file with the gt locs for test image id " << gt_id() << '\n';
    std::cerr << log.str();
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::stringstream status_xml;  status_xml << "status_tile_" << tile_id()<< "_gpu_" << dev_id() << ".xml";

  // check the consistency of tile_id and zone_id
  // for coast --- zone 18 contains only tile 8 to tile 14 and zone 17 contains only tile 0 to tile 8
  // for desert --- all tiles (4 tiles) are in zone 11
  //if (tile_id() > 8 && zone_id() == 17 ||
  //    tile_id() < 8 && tile_id() != 5 && zone_id() == 18 ||
  //    tile_id() > 3 && zone_id() == 11 )
  //{
  //  log << " ERROR: inconsistency between tile_id and utm zone_id, tile_id = " << tile_id() << ", zone_id = " << zone_id() << '\n';
  //  if (do_log) { volm_io::write_log(out_folder(), log.str()); }
  //  std::cerr << log.str();
  //  volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
  //  return volm_io::EXE_ARGUMENT_ERROR;
  //}


  // load geo_index
  std::stringstream file_name_pre;
  file_name_pre << geo_hypo_folder() << "geo_index_tile_" << tile_id();
  std::cout << " geo_index_hyps_file = " << file_name_pre.str() + ".txt" << std::endl;
  if (!vul_file::exists(file_name_pre.str() + ".txt")) {
    log << " ERROR: gen_index_folder is wrong (missing last slash/ ?), no geo_index_files found in " << geo_hypo_folder() << '\n';
    if (do_log) { volm_io::write_log(out_folder(), log.str()); }
    std::cerr << log.str();
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, "", status_xml.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  // check whether we have candidate list for this query
  bool is_candidate = false;
  vgl_polygon<double> cand_poly;
  std::cout << " candidate list = " <<  candidate_list() << std::endl;
  if ( candidate_list().compare("") != 0) {
    if (!vul_file::exists(candidate_list())) {
      log << " ERROR: can not fine candidate list file: " << candidate_list() << '\n';
      if (do_log)  volm_io::write_composer_log(out_folder(), log.str());
      std::cerr << log.str();
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, "", status_xml.str());
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
  // check the parsed candidate polygon to ensure its usage
  if (is_candidate) {
    unsigned ns = cand_poly.num_sheets();
    for (unsigned s = 0; s < ns; s++) {
      vgl_point_2d<double> start = cand_poly[s][0];
      vgl_point_2d<double>  last = cand_poly[s][cand_poly[s].size()-1];
      if ( (start-last).length() < 1E-5 )
        cand_poly[s].pop_back();
    }
  }


  // prune the tree, only leaves with non-zero hypos are left
  if (is_candidate)
    volm_geo_index::prune_tree(root, cand_poly);
  std::vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);

  // read in the parameter, create depth_interval
  std::stringstream params_file;
  params_file << geo_index_folder() << "geo_index_tile_" << tile_id() << "_index.params";

  boxm2_volm_wr3db_index_params params;
  if (!params.read_params_file(params_file.str())) {
    log << " ERROR: cannot read params file from " << params_file.str() << '\n';
    if (do_log)  volm_io::write_log(out_folder(), log.str());
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, "", status_xml.str());
    std::cout << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);
  // construct depth_interval table for pass 1 matcher
  std::map<double, unsigned char>& depth_interval_map = sph->get_depth_interval_map();
  std::vector<float> depth_interval;
  auto iter = depth_interval_map.begin();
  for (; iter != depth_interval_map.end(); ++iter)
    depth_interval.push_back((float)iter->first);

  // load spherical shell
  if ( !vul_file::exists(sph_bin()) ) {
    log << " ERROR: can not find spherical shell binary: " << sph_bin() << '\n';
    if (do_log) volm_io::write_log(out_folder(), log.str());
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, "", status_xml.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  else {
    std::cerr << "loading spherical shell from " << sph_bin() << '\n';
  }
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container();

  vsl_b_ifstream is_sph(sph_bin());
  sph_shell->b_read(is_sph);
  is_sph.close();
  if (sph_shell->get_container_size() != params.layer_size) {
    log << " ERROR: The loaded spherical shell has different layer size from the index\n";
    if (do_log) volm_io::write_log(out_folder(), log.str());
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, "", status_xml.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // load camera space
  if (!vul_file::exists(cam_bin())) {
    std::cerr << " ERROR: camera_space binary --> " << cam_bin() << " can not be found!\n";
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, "", status_xml.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vsl_b_ifstream ifs_cam(cam_bin());
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(ifs_cam);
  ifs_cam.close();

  // check depth_map_scene binary
  if (!vul_file::exists(dms_bin())) {
    std::cerr << " ERROR: depth map scene binary can not be found ---> " << dms_bin() << '\n';
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, "", status_xml.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // load the volm_query
#if 0
  if (!vul_file::exists(query_bin())) {
    std::cerr << " ERROR: volm_query binary can not be found ---> " << query_bin() << '\n';
    volm_io::write_status(out_folder(), volm_io::DEPTH_SCENE_FILE_IO_ERROR);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
#endif
  volm_query_sptr query = new volm_query(query_bin(), cam_space, dms_bin(), sph_shell, sph);

  // screen output of query
  unsigned total_size = query->obj_based_query_size_byte();
  std::cout << "\n==================================================================================================\n"
           << "\t\t  1. Create query from given camera space and Depth map scene\n"
           << "\t\t  " << dms_bin() << '\n'
           << "\t\t  generate query has " << query->get_cam_num() << " cameras "
           << " and " << (float)total_size/1024 << " Kbyte in total\n"
           << "==================================================================================================\n\n"

           << " The spherical shell for current query has parameters: point_angle = " << query->sph_shell()->point_angle()
           << ", top_angle = "    << query->sph_shell()->top_angle()
           << ", bottom_angle = " << query->sph_shell()->bottom_angle()
           << ", size = " << query->get_query_size() << '\n'
           << " The depth interval used for current query has size " << depth_interval.size()
           << ", max depth = " << depth_interval[depth_interval.size()-1] << std::endl;

  depth_map_scene_sptr dm = query->depth_scene();
  std::cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << std::endl;
  if (dm->sky().size()) {
    std::cout << " -------------- SKYs --------------" << std::endl;
    for (unsigned i = 0; i < dm->sky().size(); i++)
      std::cout << "\t name = " << (dm->sky()[i]->name())
               << ",\t depth = " << 254
               << ",\t orient = " << (int)query->sky_orient()
               << std::endl;
  }
  if (dm->ground_plane().size()) {
    std::cout << " -------------- GROUND PLANE --------------" << std::endl;
    for (unsigned i = 0; i < dm->ground_plane().size(); i++)
      std::cout << "\t name = " << dm->ground_plane()[i]->name()
               << ",\t depth = " << dm->ground_plane()[i]->min_depth()
               << ",\t orient = " << dm->ground_plane()[i]->orient_type()
               << ",\t land_id = " << dm->ground_plane()[i]->land_id()
               << ",\t land_name = " << volm_osm_category_io::volm_land_table[dm->ground_plane()[i]->land_id()].name_
               << std::endl;
  }
  std::vector<depth_map_region_sptr> dmr = query->depth_regions();
  if (dmr.size()) {
    std::cout << " -------------- DEPTH REGIONS --------------" << std::endl;
    for (auto & i : dmr) {
      std::cout << "\t\t " <<  i->name()  << " region "
               << ",\t\t min_depth = " << i->min_depth()
               << " ---> interval = " << (int)sph->get_depth_interval(i->min_depth())
               << ",\t\t max_depth = " << i->max_depth()
               << ",\t\t order = " << i->order()
               << ",\t\t orient = " << i->orient_type()
               << ",\t\t NLCD_id = " << i->land_id()
               << ",\t\t land_name = " << volm_osm_category_io::volm_land_table[i->land_id()].name_
               << std::endl;
    }
  }

  // read (or create) weight parameters for depth_map_scene
  std::vector<volm_weight> weights;
  if (vul_file::exists(weight_file()) ) {
    // read the weight parameter from pre-loaded
    volm_weight::read_weight(weights, weight_file());
    // check whether the loaded weight parameters satisfy the requirement, if not, create default equal weight parameters
    if (!volm_weight::check_weight(weights)) {
      weights.clear();
      volm_weight::equal_weight(weights, dm);
    }
  }
  else {
    // create equal weight parameter for all objects
    volm_weight::equal_weight(weights, dm);
  }

  std::cout << "\n==================================================================================================\n"
           << "\t\t  2. Weight parameters used are as following\n";
  for (auto & weight : weights)
    std::cout << ' ' << weight.w_typ_
             << ' ' << weight.w_ori_
             << ' ' << weight.w_lnd_
             << ' ' << weight.w_dst_
             << ' ' << weight.w_ord_
             << ' ' << weight.w_obj_ << std::endl;
  std::cout << "==================================================================================================\n\n";


  // define the device that will be used
  bocl_manager_child &mgr = bocl_manager_child::instance();
  if (dev_id() >= (unsigned)mgr.numGPUs()) {
    log << " GPU is " << dev_id() << " is invalid, only " << mgr.numGPUs() << " are available\n";
    if (do_log)  volm_io::write_log(out_folder(), log.str());
    std::cerr << log.str();
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, "", status_xml.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::cout << "\n==================================================================================================\n"
           << "\t\t  3. Following device is used for volm_matcher\n"
           << "\t\t  " << mgr.gpus_[dev_id()]->info() << '\n'
           << "==================================================================================================\n"

           << "\n==================================================================================================\n"
           << "\t\t  4. Start volumetric matching with following matchers\n"
           << "==================================================================================================\n" << std::endl;

  // start pass 1 volm_matcher
  boxm2_volm_matcher_p1 obj_ps1_matcher(cam_space, query, leaves, buffer_capacity(), geo_index_folder(), tile_id(),
                                        depth_interval, cand_poly, mgr.gpus_[dev_id()], is_candidate, is_last_pass, out_folder(),
                                        threshold(), max_cam_per_loc(), weights);

  if (!obj_ps1_matcher.volm_matcher_p1(num_locs())) {
    log << " ERROR: pass 1 volm_matcher failed for geo_index " << params_file.str() << '\n';
    if (do_log) volm_io::write_log(out_folder(), log.str());
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, "", status_xml.str());
    std::cerr << log.str();
    return volm_io::MATCHER_EXE_FAILED;
  }

  // write the score output binary
  std::cout << "\n==================================================================================================\n"
           << "\t\t  5. Generate output for pass 1 matcher and store it in\n"
           << "\t\t     " << out_folder() << '\n'
           << "==================================================================================================\n" << std::endl;
  std::stringstream out_fname_bin;
  out_fname_bin << out_folder() << "ps_1_scores_tile_" << tile_id() << ".bin";
#if 0
  std::stringstream out_fname_txt;
  out_fname_txt << out_folder() << "ps_1_scores_tile_" << tile_id() << ".txt";
#endif
  if (!obj_ps1_matcher.write_matcher_result(out_fname_bin.str())) {
    log << " ERROR: writing output failed for pass 1 ray_based matcher\n";
    if (do_log) volm_io::write_log(out_folder(), log.str());
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, "", status_xml.str());
    std::cerr << log.str();
    return volm_io::MATCHER_EXE_FAILED;
  }

  // output the ground truth score for all cameras
  if (gt_id() != 0 && gt_id() < 100) {
    //std::vector<std::pair<vgl_point_3d<double>, std::pair<std::string, std::string> > > samples;
    std::vector<std::pair<vgl_point_3d<double>, std::pair<std::pair<std::string, int>, std::string> > > samples;
    volm_io::read_gt_file(gt_file(), samples);
    // obtain the zone_id and tile_id from gt_locs
    int gt_utm_id;
    double x, y;
    vpgl_utm utm;
    utm.transform(samples[gt_id()].first.y(), samples[gt_id()].first.x(), x, y, gt_utm_id);

    std::vector<volm_tile> tiles;
    if (samples[gt_id()].second.second == "desert")
      tiles = volm_tile::generate_p1_wr1_tiles();
    else
      tiles = volm_tile::generate_p1_wr2_tiles();
    unsigned gt_tile_id = tiles.size();
    for (unsigned i = 0; i < tiles.size(); i++) {
      unsigned u, v;
      if (tiles[i].global_to_img(samples[gt_id()].first.x(), samples[gt_id()].first.y(), u, v) )
        if (u < tiles[i].ni() && v < tiles[i].nj())
          gt_tile_id = i;
    }

    if ( gt_tile_id != tile_id() || gt_utm_id != zone_id() ) {
      std::cerr << " GT location " << gt_id() << samples[gt_id()].first.y() << ", " << samples[gt_id()].first.x()
               << " is in zone " << gt_utm_id << " and tile " << gt_tile_id << '\n';
      std::cerr << " Current matcher runs on zone " << zone_id() << " tile " << tile_id() << '\n';
      std::cerr << " Ignore score output along the camera space " << std::endl;
    } else {
      vgl_point_3d<double> gt_loc;
      gt_loc = samples[gt_id()].first;
      double gt_lon, gt_lat;
      gt_lon = gt_loc.x();  gt_lat = gt_loc.y();
      unsigned gt_h_id = 0;
      volm_geo_index_node_sptr leaf_gt = volm_geo_index::get_closest(root, gt_lat, gt_lon, gt_h_id);

      // obtain the leaf_id and hypo_id for test images
      unsigned gt_l_id = 0;
      for (unsigned i = 0; i < leaves.size(); i++)
        if (leaf_gt->get_string() == leaves[i]->get_string() ) { gt_l_id = i; i = leaves.size(); }

      std::cout << "\n==================================================================================================\n"
                << "\t\t  6. write out the score for " << max_cam_per_loc() << " cameras which is higher than " << threshold() << "\n"
                << "\t\t     " << out_folder() << '\n'
                << "\t\t for GT location " << gt_id() << samples[gt_id()].first.y() << ", " << samples[gt_id()].first.x()
                << ", is in utm zone " << gt_utm_id << " and tile " << gt_tile_id << '\n'
                << "\t\t Associated leaf is " << leaf_gt->get_string() << "(leaf_id " << gt_l_id << ")  and "<< " hypo_id = "
                << gt_h_id << "(" << leaves[gt_l_id]->hyps_->locs_[gt_h_id] << ")\n"
                << "==================================================================================================\n" << std::endl;
      std::stringstream gt_score_txt;
      gt_score_txt << out_folder() << "ps_1_gt_l_" << gt_l_id << "_h_" << gt_h_id << "_cam_scores.txt";
      if (!obj_ps1_matcher.write_gt_cam_score(gt_l_id, gt_h_id, gt_score_txt.str())) {
        log << " ERROR: writing output failed --> can not find ground truth leaf_id " << gt_l_id << ", hypo_id " << gt_h_id << '\n';
        if (do_log)  volm_io::write_log(out_folder(), log.str());
        std::cerr << log.str();
        volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, "", status_xml.str());
        return volm_io::MATCHER_EXE_FAILED;
      }
    }
  }


#if 0
  if (use_ps0()) {
    std::cout << " we will use pass 0, i.e. regional matcher... TO be implemented" << std::endl;
    is_last_pass = true;
  }
  else {
    std::cout << " regional matcher (pass 0) is avoided" << std::endl;
  }

  // start pass 1 matcher
  if (use_ps1()) {
    boxm2_volm_matcher_p1 obj_order_matcher(query, leaves, buffer_capacity(), geo_index_folder(), tile_id(),
                                            depth_interval, cand_poly, mgr.gpus_[dev_id()], is_candidate, is_last_pass, out_folder(),
                                            threshold(), max_cam_per_loc() ,use_orient());
    if (! obj_order_matcher.volm_matcher_p1()) {
      log << " ERROR: pass 1 volm_matcher failed for geo_index " << index_file << '\n';
      if (do_log) {
        volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FAILED);
      }
      std::cerr << log.str();
      return volm_io::MATCHER_EXE_FAILED;
    }
    // output will be a probability map
    std::cout << "\n==================================================================================================\n"
             << "\t\t  5. Generate output for pass 1 matcher and store it in\n"
             << "\t\t     " << out_folder() << '\n'
             << "==================================================================================================\n" << std::endl;
    std::stringstream out_fname_bin;
    out_fname_bin << out_folder() << "ps_1_scores_tile_" << tile_id() << ".bin";
    std::stringstream out_fname_txt;
    out_fname_txt << out_folder() << "ps_1_scores_tile_" << tile_id() << ".txt";
    if (!obj_order_matcher.write_matcher_result(out_fname_bin.str())) {
      log << " ERROR: writing output failed for pass 1 ray_based matcher\n";
      if (do_log) {
        volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FAILED);
        volm_io::write_log(out_folder(), log.str());
      }
      std::cerr << log.str();
      return volm_io::MATCHER_EXE_FAILED;
    }
    // output the camera score for desired ground truth location
    if (gt_out()) {
      std::stringstream gt_score_txt;
      gt_score_txt << out_folder() << "ps_1_gt_l_" << gt_l_id() << "_h_" << gt_h_id() << "_cam_scores.txt";
      if (!obj_order_matcher.write_gt_cam_score(gt_l_id(), gt_h_id(), gt_score_txt.str())) {
        log << " ERROR: writing output failed --> can not find ground truth leaf_id " << gt_l_id() << ", hypo_id " << gt_h_id() << '\n';
        if (do_log) {
          volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FAILED);
          volm_io::write_log(out_folder(), log.str());
        }
        std::cerr << log.str();
        return volm_io::MATCHER_EXE_FAILED;
      }
      std::cout << " ground truth score stored in " << gt_score_txt.str() << std::endl;
    }
  }
  else {
    std::cout << " object based depth/order matcher (pass 1) is avoided" << std::endl;
  }

  // start pass 2 matcher
  if (use_ps2()) {
    std::cout << " we will use pass 2, i.e. object based, ray based ORIENT/NLCD matcher\n"
             << " input: query, index, leaves, candidate list(is_candidate), depth_interval\n"
             << " NEED TO CHECK WHETHEER WE HAVE PASS 0 MATCHER RESULT, IF SO, LOAD THE REDUCED SPACE FROM PASS 0" << std::endl;
  }
  else {
    std::cout << " object based orientation/land classification matcher (pass 2) is avoided" << std::endl;
  }
#endif

#if 0
  // read the generated binary to check the value
  std::vector<volm_score_sptr> scores;
  std::stringstream out_fname_bin;
  out_fname_bin << out_folder() << "ps_1_scores_tile_" << tile_id() << ".bin";
  volm_score::read_scores(scores, out_fname_bin.str());  // this file may be too large, make sure it fits to memory!!
  std::cout << " THE READ IN BINRAY SCORE FILE" << std::endl;
  for (unsigned i = 0; i < scores.size(); i++) {
    std::cout << scores[i]->leaf_id_ << ' ' << scores[i]->hypo_id_
             << ' ' << scores[i]->max_score_ << ' ' << scores[i]->max_cam_id_ << '\n'
             << " cam_id: \t";
    std::vector<unsigned> cam_ids = scores[i]->cam_id_;
    for (unsigned jj = 0; jj < cam_ids.size(); jj++)
      std::cout << ' ' << cam_ids[jj];
    std::cout << '\n';
  }
#endif

  // finish everything successfully
  volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FINISHED, 90, "", status_xml.str());
  return volm_io::SUCCESS;
}
