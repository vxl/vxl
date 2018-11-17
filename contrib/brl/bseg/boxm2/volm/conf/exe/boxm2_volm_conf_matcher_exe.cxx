// This is contrib/brl/bseg/volm/conf/exe/boxm2_volm_conf_matcher_exe.cxx
//:
// \file
// \brief  executable to run configuration matcher
//
// \author Yi Dong
// \date September 08, 2014
// \verbatim
//  Modifications
//   Yi Dong -- Oct, 2014  Modify the candidate region
//   Yi Dong -- Feb, 2014  Update the matcher that can accept tag xml file as configuration query input
// \endverbatim
//

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vgl/vgl_intersection.h>
#include <vgl/io/vgl_io_polygon.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <bkml/bkml_parser.h>
#include <volm/volm_io.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/conf/volm_conf_query.h>
#include <volm/conf/volm_conf_buffer.h>
#include <volm/volm_candidate_list.h>
#include <boxm2/volm/conf/boxm2_volm_conf_matcher.h>
#include <volm/volm_utils.h>

int main(int argc, char** argv)
{
  vul_arg<unsigned>       tile_id("-tile", "ROI tile id", 9999);
  vul_arg<int>            leaf_id("-leaf", "geo location leaf id pass(-1) for matcher running on all leaves", -1);
  vul_arg<std::string>   query_img("-img",  "query image", "");
  vul_arg<std::string>     dms_bin("-dms",  "depth map scene file", "");
  vul_arg<std::string>     tag_xml("-tag",  "tag xml file", "");
  vul_arg<float>     floor_height("-floor-height",  "building floor height (default is 4.5m/per floor)", 4.5f);
  vul_arg<std::string>     cam_kml("-cam",  "camera calibration kml file", "");
  vul_arg<std::string>     cam_inc("-inc",  "file defining camera angle incremental values", "");
  vul_arg<std::string>  geo_folder("-geo",  "folder to read the geo hypotheses ", "");
  vul_arg<std::string>  idx_folder("-idx",  "folder to read configuration indices", "");
  vul_arg<std::string>  index_name("-idx-name", "name of the loaded index", "");
  vul_arg<std::string>  out_folder("-out",  "output folder", "");
  vul_arg<bool>           is_cand("-is-cand", "option to specify whether there is candidate region available", false);
  vul_arg<std::string> cand_folder("-cand-folder", "folder where the candidate regions for each geo location leaf stores", "");
  vul_arg<float>  buffer_capacity("-buffer", "buffer capacity for index creation (in GByte)", 2.0f);
  vul_arg<unsigned>  tol_in_pixel("-tol", "distance tolerance value in pixel unit (default is 25)", 25);
  vul_arg<bool>        use_height("-height", "option to use building height in matching", false);
  vul_arg<bool>              read("-read", "option to read the matching score from output folder", false);
  vul_arg_parse(argc, argv);

  // input check
  if (tile_id() == 9999 || geo_folder().compare("") == 0 || out_folder().compare("") == 0)
  {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::stringstream log_file;
  std::stringstream log;
  log_file << out_folder() << "/log_tile_" << tile_id() << "_leaf_" << leaf_id() << ".xml";

  if (use_height()) {
    std::cout << "Configuration Matcher will match the height of URGENT building footprints!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  std::cout << "Distance tolerance is " << tol_in_pixel() << " pixels!!!!!!!!!!!!!!!!!!!!!!" << std::flush << std::endl;

  // load the geo index locations
  std::stringstream file_name_pre;
  file_name_pre << geo_folder() << "/geo_index_tile_" << tile_id();
  if (!vul_file::exists(file_name_pre.str()+".txt")) {
    log << "ERROR: loading geo index locations fails form file: " << file_name_pre.str() << ".txt!\n";
    volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  std::vector<volm_geo_index_node_sptr> loc_leaves_all;
  loc_leaves_all.clear();
  volm_geo_index::get_leaves_with_hyps(root, loc_leaves_all);

  std::vector<volm_geo_index_node_sptr> loc_leaves;
  loc_leaves.clear();
  if (leaf_id() >= 0 && leaf_id() < (int)loc_leaves_all.size())
    loc_leaves.push_back(loc_leaves_all[leaf_id()]);
  else
    for (const auto & i : loc_leaves_all)
      loc_leaves.push_back(i);

  if (!read())        // perform configurational matcher
  {
    // input check
    if ( cam_kml().compare("") == 0 || cam_inc().compare("") == 0 )
    {
      std::cerr << "ERROR: missing input for configuration matcher!!!" << std::endl;
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    if (dms_bin().compare("") == 0 && tag_xml().compare("") == 0)
    {
      std::cerr << "ERROR: missing query tag file, either tag .xml file or .vsl binary file required" << std::endl;
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    // load the query label
    std::string world_region, query_name;
    unsigned img_ni, img_nj;
    depth_map_scene_sptr dms = new depth_map_scene;
    if (vul_file::exists(dms_bin()))  // load from .vsl file
    {
      vsl_b_ifstream dms_is(dms_bin().c_str());
      dms->b_read(dms_is);
      dms_is.close();
    }
    else // load from query tag xml file
    {
      if (!vul_file::exists(tag_xml()))
      {
        log << "ERROR: can not find tag xml file: " << tag_xml() << "!\n";
        volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
      }
      std::cout << "Configuration Matcher will load query from tag xml file: " << tag_xml() << "!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
      if (!volm_io::read_conf_query_tags(tag_xml(), floor_height(), dms, world_region, img_ni, img_nj, query_name)) {
        log << "ERROR: load query tag xml failed, check tag file for more details!!!\n";
        volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
      }
      std::cout << "  query is loaded successfully from tag xml!!!" << std::endl;
    }

    // create the camera space
    // read the camera incremental values
    if (!vul_file::exists(cam_inc())) {
      log << "ERROR: can not find camera incremental file: " << cam_inc() << "!\n";
      volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
    }
    volm_io_expt_params params;
    params.read_cam_inc_params(cam_inc());
    double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
    double tfov, tfov_dev, altitude, lat, lon;
    if (!volm_io::read_camera(cam_kml(), dms->ni(), dms->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, tfov, tfov_dev, altitude, lat, lon)) {
      log << "ERROR: parsing camera kml file failed: " << cam_kml() << "!\n";
      volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
    }
    // create camera space
    volm_camera_space_sptr csp = new volm_camera_space(tfov, tfov_dev, params.fov_inc, altitude, dms->ni(), dms->nj(),
                                                       heading, heading_dev, params.head_inc,
                                                       tilt, tilt_dev, params.tilt_inc,
                                                       roll, roll_dev, params.roll_inc);
    // enforce ground plane constraint if user specified a ground plane in the depth map scene
    if (dms->ground_plane().size() > 0)
    {
      camera_space_iterator cit = csp->begin();
      for ( ; cit != csp->end(); ++cit) {
        unsigned current = csp->cam_index();
        vpgl_perspective_camera<double> cam = csp->camera();  // camera t current stat of iterator
        bool success = true;
        for (unsigned i = 0; success && i < dms->ground_plane().size(); i++)
          success = dms->ground_plane()[i]->region_ground_2d_to_3d(cam);
        if (success) // add this camera
          csp->add_camera_index(current);
      }
    }
    else
    {
      csp->generate_full_camera_index_space();
    }
    // create the volm_conf_query
    volm_conf_query_sptr query = new volm_conf_query(csp, dms, tol_in_pixel());
    // some screen outputs for input
    std::cout << "--------- Operate configuration matcher on tile " << tile_id() << " and leaf " << leaf_id() << " ------------ " << std::endl;
    std::cout << " Input query info: " << std::endl;
    std::cout << "  query has " << query->ncam() << " cameras" << std::endl;
    std::vector<std::string> cam_string = query->camera_strings();
    std::vector<std::map<std::string, std::pair<float, float> > > conf_objs_d_tol = query->conf_objects_d_tol();
    std::vector<std::map<std::string, volm_conf_object_sptr> > conf_objs = query->conf_objects();
    std::vector<std::string> query_ref_obj = query->ref_obj_name();
    if (cam_string.size() < 20) {
      for (unsigned i = 0; i < conf_objs.size(); i++) {
        std::cout << "   camera " << i << " : " << cam_string[i] << " has " << conf_objs[i].size() << " configurational objects " << std::endl;
      }
    }
    unsigned num_locs = 0;
    std::cout << " Index type: " << index_name() << std::endl;
    std::cout << " Tile " << tile_id() << " has " << loc_leaves.size() << " leaves for matcher" << std::endl;
    std::cout << " Configuration matcher will operate on " << loc_leaves.size() << " leaves (leaf id: " << leaf_id() << ')' << std::endl;
    // create a configuration matcher
    if (idx_folder().compare("") == 0) {
      log << "ERROR: can not find index folder: " << idx_folder() << "!\n";
      volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
    }
    if (cand_folder().compare("") == 0) {
      log << "ERROR: can not find candidate region folder: " << cand_folder() << "!\n";
      volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
    }
    std::cout << " Start the configuration matcher!!!!!!!!" << std::endl;
    vul_timer t;
    t.mark();
    boxm2_volm_conf_matcher matcher(query, tile_id(), loc_leaves, idx_folder(), out_folder(), cand_folder(), buffer_capacity());
    int matched_locs = 0;
    if (is_cand())
      matched_locs = matcher.conf_match_cpp(index_name(), use_height());
    else
      matched_locs = matcher.conf_match_cpp_no_candidate(index_name(), use_height());
    if (matched_locs < 0) {
      log << "ERROR: configuration matcher failed on tile " << tile_id() << ", leaf " << leaf_id() << "!\n";
      volm_io::write_error_log(log_file.str(), log.str());
      return volm_io::EXE_MATCHER_FAILED;
    }
    std::cout << " Matching " << matched_locs << " and " << query->ncam() << " cameras per location costs " << t.all()/(1000.0*60.0) << " minutes" << std::endl;
    return volm_io::SUCCESS;
  }
  else  // read the score for given leaf
  {
    std::cout << "--------- Reading configuration matcher score on tile " << tile_id() << " ------------ " << std::endl;
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
      while (leaf->hyps_->get_next(0,1,h_pt))
      {
        if (is_cand && !volm_candidate_list::inside_candidate_region(cand_inner, cand_outer, h_pt.x(), h_pt.y()))
          continue;
        volm_conf_score score_in;
        score_idx.get_next(score_in);
        std::cout << "location: " << std::setprecision(8) << h_pt.x() << ", " << std::setprecision(8) << h_pt.y() << " (tile " << tile_id() << "): ---> ";
        score_in.print(std::cout);
      }
      score_idx.finalize();
    }
    return volm_io::SUCCESS;
  }
  return volm_io::SUCCESS;
}

#if 0
int main(int argc, char** argv)
{
  vul_arg<unsigned>      tile_id("-tile", "ROI tile id", 9999);
  vul_arg<int>           leaf_id("-leaf", "geo location leaf id pass(-1) for matcher running on all leaves", -1);
  vul_arg<std::string>  query_img("-img",  "query image", "");
  vul_arg<std::string>    dms_bin("-dms",  "depth map scene file", "");
  vul_arg<std::string>    cam_kml("-cam",  "camera calibration kml file", "");
  vul_arg<std::string>    cam_inc("-inc",  "file defining camera angle incremental values", "");
  vul_arg<std::string> geo_folder("-geo",  "folder to read the geo hypotheses ", "");
  vul_arg<std::string> idx_folder("-idx",  "folder to read configuration indices", "");
  vul_arg<std::string> index_name("-idx-name", "name of the loaded index", "");
  vul_arg<std::string> out_folder("-out",  "output folder", "");
  vul_arg<std::string>  cand_file("-cand", "candidate region kml provided by previous matcher", "");
  vul_arg<float> buffer_capacity("-buffer", "buffer capacity for index creation (in GByte)", 2.0f);
  vul_arg<unsigned> tol_in_pixel("-tol", "distance tolerance value in pixel unit (default is 25)", 25);
  vul_arg<bool>       use_height("-height", "option to use building height in matching", false);
  vul_arg<bool>             read("-read", "option to read the matching score from output folder", false);
  vul_arg_parse(argc, argv);
  std::cout << "argc: " << argc << std::endl;

  // input check
  if (tile_id() == 9999 || geo_folder().compare("") == 0 || out_folder().compare("") == 0)
  {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  std::stringstream log_file;
  std::stringstream log;
  log_file << out_folder() << "/log_tile_" << tile_id() << "_leaf_" << leaf_id() << ".xml";

  // load the geo index locations
  // create the candidate region if exists
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

  if (use_height()){
    std::cout << "Configuration Matcher will match the height of URGENT building footprints!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  std::cout << "Distance tolerance is " << tol_in_pixel() << " pixels!!!!!!!!!!!!!!!!!!!!!!" << std::flush << std::endl;

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
  if (leaf_id() >= 0 && leaf_id() < (int)loc_leaves_all.size()) {
    if (is_cand && vgl_intersection(loc_leaves_all[leaf_id()]->extent_, cand_out)) {
        std::cout << "input leaf " << leaf_id() << " is inside candidate polygon, matcher will be performed" << std::endl;
        loc_leaves.push_back(loc_leaves_all[leaf_id()]);
    }
    else
      loc_leaves.push_back(loc_leaves_all[leaf_id()]);
    if (loc_leaves.empty()) {
      std::cout << "input leaf " << leaf_id() << ": " << loc_leaves_all[leaf_id()] << " is outside candidate polygon.  Exit!!!\n";
      return volm_io::SUCCESS;
    }
  }
  else
  {
    for (unsigned i = 0; i < loc_leaves_all.size(); i++)
      if (is_cand && vgl_intersection(loc_leaves_all[i]->extent_, cand_out))
        loc_leaves.push_back(loc_leaves_all[i]);
      else
        loc_leaves.push_back(loc_leaves_all[i]);
  }

  // perform configurational matcher
  if (!read())
  {
    // input check
    if (dms_bin().compare("") == 0 || cam_kml().compare("") == 0 || cam_inc().compare("") == 0 )
    {
      std::cerr << "ERROR: missing input for configuration matcher!!!" << std::endl;
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    // load the depth map scene
    if (!vul_file::exists(dms_bin())) {
      log << "ERROR: can not find depth map scene binary: " << dms_bin() << "!\n";
      volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
    }
    depth_map_scene_sptr dms = new depth_map_scene;
    vsl_b_ifstream dms_is(dms_bin().c_str());
    dms->b_read(dms_is);
    dms_is.close();

    // create the camera space
    // read the camera incremental values
    if (!vul_file::exists(cam_inc())) {
      log << "ERROR: can not find camera incremental file: " << cam_inc() << "!\n";
      volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
    }
    volm_io_expt_params params;
    params.read_cam_inc_params(cam_inc());
    double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
    double tfov, tfov_dev, altitude, lat, lon;
    if (!volm_io::read_camera(cam_kml(), dms->ni(), dms->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, tfov, tfov_dev, altitude, lat, lon)) {
      log << "ERROR: parsing camera kml file failed: " << cam_kml() << "!\n";
      volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
    }
    // create camera space
    volm_camera_space_sptr csp = new volm_camera_space(tfov, tfov_dev, params.fov_inc, altitude, dms->ni(), dms->nj(),
                                                       heading, heading_dev, params.head_inc,
                                                       tilt, tilt_dev, params.tilt_inc,
                                                       roll, roll_dev, params.roll_inc);
    // enforce ground plane constraint if user specified a ground plane in the depth map scene
    if (dms->ground_plane().size() > 0)
    {
      camera_space_iterator cit = csp->begin();
      for ( ; cit != csp->end(); ++cit) {
        unsigned current = csp->cam_index();
        vpgl_perspective_camera<double> cam = csp->camera();  // camera t current stat of iterator
        bool success = true;
        for (unsigned i = 0; success && i < dms->ground_plane().size(); i++)
          success = dms->ground_plane()[i]->region_ground_2d_to_3d(cam);
        if (success) // add this camera
          csp->add_camera_index(current);
      }
    }
    else
    {
      csp->generate_full_camera_index_space();
    }
#if 0
    // save the camera space as binary in out_folder
    vsl_b_ofstream ofs(out_folder() + "/camera_space.bin");
    csp->b_write(ofs);
    ofs.close();
#endif
    // create the volm_conf_query
    volm_conf_query_sptr query = new volm_conf_query(csp, dms, tol_in_pixel());

    // some screen outputs for input
    std::cout << "--------- Operate configuration matcher on tile " << tile_id() << " and leaf " << leaf_id() << " ------------ " << std::endl;
    std::cout << " Input query info: " << std::endl;
    std::cout << "  query has " << query->ncam() << " cameras" << std::endl;
    std::vector<std::string> cam_string = query->camera_strings();
    std::vector<std::map<std::string, std::pair<float, float> > > conf_objs_d_tol = query->conf_objects_d_tol();
    std::vector<std::map<std::string, volm_conf_object_sptr> > conf_objs = query->conf_objects();
    if (cam_string.size() < 10000) {
      for (unsigned i = 0; i < conf_objs.size(); i++) {
        std::cout << "   camera " << i << " : " << cam_string[i] << " has " << conf_objs[i].size() << " configurational objects" << std::endl;
        //for (std::map<std::string, volm_conf_object_sptr>::iterator mit = conf_objs[i].begin();  mit != conf_objs[i].end(); ++mit)  {
        //  std::cout << " cam " << i << "  " << mit->first << "\t\t";
        //  mit->second->print(std::cout);
        //  std::cout << "      distance tolerance: " << conf_objs_d_tol[i][mit->first].first << " meter to " << conf_objs_d_tol[i][mit->first].second << " meter " << std::endl;
        //}
      }
    }
    unsigned num_locs = 0;
    std::cout << " Index type: " << index_name() << std::endl;
    std::cout << " Tile " << tile_id() << " has " << loc_leaves.size() << " leaves for matcher" << std::endl;
    for (int i = 0; i < loc_leaves.size(); i++) {
      std::cout << "  leaf " << i << " has " << loc_leaves[i]->hyps_->locs_.size() << " locations" << std::endl;
      num_locs += (unsigned)loc_leaves[i]->hyps_->locs_.size();
    }
    std::cout << "  matching will operate on " << num_locs << " locations in " << loc_leaves.size() << std::endl;
#if 0
    // visualize the configuration query
    if (cam_string.size() < 100) {
      if (vul_file::exists(query_img()))
        query->visualize_ref_objs(query_img(), out_folder());
    }
#endif
    // create a configurational matcher
    if (idx_folder().compare("") == 0) {
      log << "ERROR: can not find index folder: " << idx_folder() << "!\n";
      volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
    }
    vul_timer t;
    t.mark();
    boxm2_volm_conf_matcher matcher(query, tile_id(), loc_leaves, idx_folder(), out_folder(), cand_out, cand_in, buffer_capacity());

    std::cout << " Start the configuration matcher ..." << std::endl;
    int matched_locs = matcher.conf_match_cpp(index_name(), leaf_id(), use_height());
    if (matched_locs == 0) {
      log << "ERROR: configurational matcher failed on tile " << tile_id() << ", leaf " << leaf_id() << "!\n";
      return volm_io::EXE_MATCHER_FAILED;
    }
    std::cout << " Matching " << matched_locs << " and " << query->ncam() << " cameras per location consumes " << t.all()/(1000.0*60.0) << " minutes" << std::endl;
    return volm_io::SUCCESS;
  }
  else  // read the score for given leaf
  {
    std::cout << "--------- Reading configuration matcher score on tile " << tile_id() << " ------------ " << std::endl;
    std::stringstream score_file_pre;
    score_file_pre << out_folder() << "/conf_score_tile_" << tile_id();
    for (unsigned i = 0; i < loc_leaves.size(); i++)
    {
      volm_geo_index_node_sptr leaf = loc_leaves[i];
      std::string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name() + ".bin";
      if (!vul_file::exists(score_bin_file)) {
        continue;  // this leaf may not have any location inside candidate region and therefore no score stored
      }
      volm_conf_buffer<volm_conf_score> score_idx(buffer_capacity());
      score_idx.initialize_read(score_bin_file);
      vgl_point_3d<double> h_pt;
      while (leaf->hyps_->get_next(0,1,h_pt))
      {
#if 0
        if (is_cand && !volm_candidate_list::inside_candidate_region(cand_in, cand_out, h_pt.x(), h_pt.y()))
          continue;
#endif
        if (is_cand && !volm_candidate_list::inside_candidate_region(cand_out, h_pt.x(), h_pt.y()))
          continue;
        volm_conf_score score_in;
        score_idx.get_next(score_in);
        std::cout << "location: " << h_pt.x() << ", " << h_pt.y() << " (tile " << tile_id() << "): --> ";
        score_in.print(std::cout);
      }
      score_idx.finalize();
    }
    return volm_io::SUCCESS;
  }
}
#endif
