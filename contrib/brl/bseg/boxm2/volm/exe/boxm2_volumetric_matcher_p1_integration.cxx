//:
// \file
// \executable to use object/ray based matcher for query image using obj_min_dist, obj_order, ground_depth, sky_region and orientation
//  note the tiles and utm zones are distributed based on given gpu_id on the cluster
// \author Yi Dong
// \date Feb 05, 2013


#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h>
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
  vul_arg<std::string> sph_bin("-sph", "spherical shell binary", "");                                          // query -- spherical shell container binary
  vul_arg<std::string> param_file("-param", "parameter file to read the parameters for depth interval", "");   // query -- parameter file for depth interval
  vul_arg<std::string> weight_file("-wgt", "weight parameters for query", "");                                 // query -- weight parameter file
  vul_arg<std::string> geo_hypo_folder("-hypo", "folder to read the geo hypotheses", "");                      // index -- folder to read the hypos for each leaf
  vul_arg<std::string> geo_index_folder("-geo", "folder to read the geo index", "");                           // index -- folder to read the index for each location
  vul_arg<std::string> candidate_list("-cand", "candidate list in kml format","");                             // query -- candidate list polygon
  vul_arg<unsigned>   tile_id("-tile", "tile id", 0);                                                         // index -- tile_id
  vul_arg<unsigned>   zone_id("-zone", "zone id", 17);                                                        // index -- zone_id
  vul_arg<float>      buffer_capacity("-buff", "index buffer capacity (GB)", 1.0f);                           // index -- buffer capacity
  vul_arg<unsigned>   dev_id("-gpuid", "device used for current matcher", 0);                                 // matcher -- device id
  vul_arg<float>      threshold("-thres", "threshold for choosing valid cameras (0~1)", 0.4f);                // matcher -- threshold for choosing cameras
  vul_arg<unsigned>   max_cam_per_loc("-max_cam", "maximum number of cameras to be saved", 200);              // matcher -- output related
  vul_arg<std::string> out_folder("-out", "output folder where score binary is stored", "");                   // matcher -- output folder
  vul_arg<bool>       logger("-logger", "designate one of the exes as logger", false);                        // matcher -- log file generation
  vul_arg_parse(argc, argv);

  std::stringstream log;
  bool do_log = false;
  if (logger())
    do_log = true;

  // check the input parameters
  if ( sph_bin().compare("") == 0 ||
       param_file().compare("") == 0 ||
       geo_hypo_folder().compare("") == 0 ||
       geo_index_folder().compare("") == 0 ||
       out_folder().compare("") == 0 )
  {
    log << " ERROR: input file/folders can not be empty\n";
    if (do_log) {
      volm_io::write_log(out_folder(), log.str());
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    }
    std::cerr << log.str();
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  std::string cam_bin = out_folder() + "/camera_space.bin";
  std::string dms_bin = out_folder() + "/depth_map_scene.bin";
  std::string query_bin = out_folder() + "/volm_query.bin";

  // check(waiting) for all available query file
  bool all_available = false;
  while (!all_available) {
    all_available = true;
    if (!vul_file::exists(cam_bin) && !(vul_file::size(cam_bin) > 0)) {
      all_available = false;  vpl_sleep(10);
    }
    if (vul_file::exists(dms_bin) && !(vul_file::size(dms_bin) > 0)) {
      all_available = false;  vpl_sleep(10);
    }
    if (vul_file::exists(query_bin) && !(vul_file::size(query_bin) > 0)) {
      all_available = false;  vpl_sleep(10);
    }
  }


  // load spherical shell
  if ( !vul_file::exists(sph_bin()) ) {
    log << " ERROR: can not find spherical shell binary: " << sph_bin() << '\n';
    if (do_log) {
      volm_io::write_log(out_folder(), log.str());
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    }
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

  // create depth interval
  if (!vul_file::exists(param_file())) {
    log << " ERROR: can not find parameter file: " << param_file() << '\n';
    if (do_log) {
      volm_io::write_log(out_folder(), log.str());
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    }
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_io_expt_params params;
  params.read_params(param_file());

  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);
  // construct depth_interval table for pass 1 matcher
  std::map<double, unsigned char>& depth_interval_map = sph->get_depth_interval_map();
  std::vector<float> depth_interval;
  auto iter = depth_interval_map.begin();
  for (; iter != depth_interval_map.end(); ++iter)
    depth_interval.push_back((float)iter->first);

  // load camera space
  vsl_b_ifstream ifs_cam(cam_bin);
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(ifs_cam);
  ifs_cam.close();

  // load the volm_query
  std::cout << " sph_shell , point angle = " << sph_shell->point_angle() << " top_angle = " << sph_shell->top_angle() << std::endl;

  volm_query_sptr query = new volm_query(query_bin, cam_space, dms_bin, sph_shell, sph);
  // create volm_weight if weight_file is given or create equal weight
  std::vector<volm_weight> weights;
  depth_map_scene_sptr dm = query->depth_scene();
  if (vul_file::exists(weight_file())) {
    // read the weight parameter from pre-loaded
    volm_weight::read_weight(weights, weight_file());
    // check whether the loaded weight parameters satisfy the requirement, if not, create default equal weight parameters
    if (!volm_weight::check_weight(weights)) {
      weights.clear();
      volm_weight::equal_weight(weights, dm);
    }
  }
  else {
    // create equal weight
    volm_weight::equal_weight(weights, dm);
  }

  // screen output
  unsigned total_size = query->obj_based_query_size_byte();
  std::cout << "\n==================================================================================================\n"
           << "\t\t  1. Create query from given camera space and Depth map scene\n"
           << "\t\t  " << dms_bin << '\n'
           << "\t\t  generate query has " << query->get_cam_num() << " cameras "
           << " and " << (float)total_size/1024 << " Kbyte in total\n"
           << "==================================================================================================\n\n"

           << " The spherical shell for current query has parameters: point_angle = " << query->sph_shell()->point_angle()
           << ", top_angle = "    << query->sph_shell()->top_angle()
           << ", bottom_angle = " << query->sph_shell()->bottom_angle()
           << ", size = " << query->get_query_size() << '\n'
           << " The depth interval used for current query has size " << depth_interval.size()
           << ", max depth = " << depth_interval[depth_interval.size()-1] << std::endl;

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
               << ",\t land_name = " << volm_label_table::land_string(dm->ground_plane()[i]->land_id())
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
               << ",\t\t land_name = " << volm_label_table::land_string(i->land_id())
               << std::endl;
    }
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

  // define device that will be used
  bocl_manager_child &mgr = bocl_manager_child::instance();
  if (dev_id() >= (unsigned)mgr.numGPUs()) {
    log << " ERROR: GPU is " << dev_id() << " is invalid, only " << mgr.numGPUs() << " are available\n";
    if (do_log) {
      volm_io::write_log(out_folder(), log.str());
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    }
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::cout << "\n==================================================================================================\n"
           << "\t\t  3. Following device is used for volm_matcher\n"
           << "\t\t  " << mgr.gpus_[dev_id()]->info() << '\n'
           << "\n==================================================================================================" << std::endl;

  // check the geo_hypo_folder and geo_index_folder

  std::stringstream file_name_pre_hypo;
  file_name_pre_hypo << geo_hypo_folder() << "/geo_index_tile_" << tile_id() <<".txt";
  std::stringstream file_name_pre_indx;
  file_name_pre_indx << geo_index_folder() << "/geo_index_tile_" << tile_id() << "_index.params";
  if (!vul_file::exists(file_name_pre_hypo.str()) || !vul_file::exists(file_name_pre_indx.str())) {
    log << " ERROR: geo_index is missing for tile " << tile_id() << '\n';
    if (do_log) {
      volm_io::write_log(out_folder(), log.str());
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    }
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // load associate geo_hypo
  std::stringstream file_name_pre;
  file_name_pre << geo_hypo_folder() << "/geo_index_tile_" << tile_id();
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());

  vgl_polygon<double> cand_poly;
  bool is_candidate = false;

  // parse the candidate list if it exists
  if ( candidate_list().compare("") != 0) {
    if (!vul_file::exists(candidate_list())) {
      log << " ERROR: can not fine candidate list file: " << candidate_list() << '\n';
      if (do_log)  volm_io::write_composer_log(out_folder(), log.str());
      std::cerr << log.str();
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
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
  if (is_candidate)
    volm_geo_index::prune_tree(root, cand_poly);
  std::vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);

  std::cout << "\n==================================================================================================\n"
             << "\t\t  4. Start volumetric matcher for tile " << tile_id() << '\n'
             << "==================================================================================================\n" << std::endl;

  // start the volm_matcher
  bool is_last_pass = false;
  boxm2_volm_matcher_p1 obj_ps1_matcher(cam_space, query, leaves, buffer_capacity(), geo_index_folder(), tile_id(),
                                        depth_interval, cand_poly, mgr.gpus_[dev_id()], is_candidate, is_last_pass, out_folder(),
                                        threshold(), max_cam_per_loc(), weights);

  if (!obj_ps1_matcher.volm_matcher_p1()) {
    log << " ERROR: pass 1 volm_matcher failed for zone " << zone_id() << " and tile " << tile_id() << '\n';
    if (do_log) {
      volm_io::write_log(out_folder(), log.str());
      volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FAILED);
    }
    std::cerr << log.str();
    return volm_io::MATCHER_EXE_FAILED;
  }

  // write the score output binary
  std::cout << "\n==================================================================================================\n"
          << "\t\t  5. Generate output of pass 1 matcher for zone " << zone_id() << " and tile " << tile_id() << ", store it in\n"
          << "\t\t     " << out_folder() << '\n'
          << "==================================================================================================\n" << std::endl;
  std::stringstream out_fname_bin;
  out_fname_bin << out_folder() << "/ps_1_scores_tile_" << tile_id() << ".bin";
  if (!obj_ps1_matcher.write_matcher_result(out_fname_bin.str())) {
    log << " ERROR: write output failed for pass 1 matcher for zone " << zone_id() << ", tile " << tile_id() << '\n';
    if (do_log) {
      volm_io::write_log(out_folder(), log.str());
      volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FAILED);
    }
    std::cerr << log.str();
    return volm_io::MATCHER_EXE_FAILED;
  }
  if (do_log)
    volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FINISHED);
  return volm_io::MATCHER_EXE_FINISHED;
}
