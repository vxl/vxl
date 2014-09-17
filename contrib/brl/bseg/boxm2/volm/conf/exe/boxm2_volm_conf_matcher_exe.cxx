// This is contrib/brl/bseg/volm/conf/exe/boxm2_volm_conf_matcher_exe.cxx
//:
// \file
// \brief  executable to run configuration matcher
//
// \author Yi Dong
// \date September 08, 2014
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vgl/vgl_intersection.h>
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

int main(int argc, char** argv)
{
  vul_arg<unsigned>      tile_id("-tile", "ROI tile id", 9999);
  vul_arg<int>           leaf_id("-leaf", "geo location leaf id pass(-1) for matcher running on all leaves", -1);
  vul_arg<vcl_string>  query_img("-img",  "query image", "");
  vul_arg<vcl_string>    dms_bin("-dms",  "depth map scene file", "");
  vul_arg<vcl_string>    cam_kml("-cam",  "camera calibration kml file", "");
  vul_arg<vcl_string>    cam_inc("-inc",  "file defining camera angle incremental values", "");
  vul_arg<vcl_string> geo_folder("-geo",  "folder to read the geo hypotheses ", "");
  vul_arg<vcl_string> idx_folder("-idx",  "folder to read configuration indices", "");
  vul_arg<vcl_string> index_name("-idx-name", "name of the loaded index", "");
  vul_arg<vcl_string> out_folder("-out",  "output folder", "");
  vul_arg<vcl_string>  cand_file("-cand", "candidate region kml provided by previous matcher", "");
  vul_arg<float> buffer_capacity("-buffer", "buffer capacity for index creation (in GByte)", 2.0f);
  vul_arg<unsigned> tol_in_pixel("-tol", "distance tolerance value in pixel unit (default is 25)", 25);
  vul_arg<bool>             read("-read", "option to read the matching score from output folder", false);
  vul_arg_parse(argc, argv);
  vcl_cout << "argc: " << argc << vcl_endl;

  // input check
  if (tile_id() == 9999 || geo_folder().compare("") == 0 || out_folder().compare("") == 0)
  {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  vcl_stringstream log_file;
  vcl_stringstream log;
  log_file << out_folder() << "/log_tile_" << tile_id() << "_leaf_" << leaf_id() << ".xml";
  
  // load the geo index locations
  // create the candidate region if exists
  vgl_polygon<double> cand_poly;
  bool is_cand = false;
  cand_poly.clear();
  if (vul_file::exists(cand_file())) {
    cand_poly = bkml_parser::parse_polygon(cand_file());
    vcl_cout << "candidate regions (" << cand_poly.num_sheets() << " sheet)are loaded from file: " << cand_file() << "!!!!!!!!!!" << vcl_endl;
    is_cand = (cand_poly.num_sheets() != 0);
  }

  // load geo index locations
  vcl_stringstream file_name_pre;
  file_name_pre << geo_folder() << "/geo_index_tile_" << tile_id();
  if (!vul_file::exists(file_name_pre.str() + ".txt")) {
    log << "ERROR: loading geo index locations fails from file: " << file_name_pre.str() << ".txt!\n";
    volm_io::write_error_log(log_file.str(), log.str());  return volm_io::EXE_ARGUMENT_ERROR;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  vcl_vector<volm_geo_index_node_sptr> loc_leaves_all;
  loc_leaves_all.clear();
  volm_geo_index::get_leaves_with_hyps(root, loc_leaves_all);

  // obtain the desired leaf
  vcl_vector<volm_geo_index_node_sptr> loc_leaves;
  if (leaf_id() >= 0 && leaf_id() < (int)loc_leaves_all.size()) {
    if (is_cand && vgl_intersection(loc_leaves_all[leaf_id()]->extent_, cand_poly)) {
        vcl_cout << "input leaf " << leaf_id() << " is inside candidate polygon, matcher will be performed" << vcl_endl;
        loc_leaves.push_back(loc_leaves_all[leaf_id()]);
    }
    else
      loc_leaves.push_back(loc_leaves_all[leaf_id()]);
    if (loc_leaves.empty()) {
      vcl_cout << "input leaf " << leaf_id() << ": " << loc_leaves_all[leaf_id()] << " is outside candidate polygon.  Exit!!!\n";
      return volm_io::SUCCESS;
    }
  }
  else
  {
    for (unsigned i = 0; i < loc_leaves_all.size(); i++)
      if (is_cand && vgl_intersection(loc_leaves_all[i]->extent_, cand_poly))
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
      vcl_cerr << "ERROR: missing input for configuration matcher!!!" << vcl_endl;
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
    vcl_cout << "--------- Operate configuration matcher on tile " << tile_id() << " and leaf " << leaf_id() << " ------------ " << vcl_endl;
    vcl_cout << " Input query info: " << vcl_endl;
    vcl_cout << "  query has " << query->ncam() << " cameras" << vcl_endl;
    vcl_vector<vcl_string> cam_string = query->camera_strings();
    vcl_vector<vcl_map<vcl_string, vcl_pair<float, float> > > conf_objs_d_tol = query->conf_objects_d_tol();
    vcl_vector<vcl_map<vcl_string, volm_conf_object_sptr> > conf_objs = query->conf_objects(); 
    if (cam_string.size() < 4) {
      for (unsigned i = 0; i < conf_objs.size(); i++) {
        vcl_cout << "   camera: " << cam_string[i] << " has following configurational objects" << vcl_endl;
        for (vcl_map<vcl_string, volm_conf_object_sptr>::iterator mit = conf_objs[i].begin();  mit != conf_objs[i].end(); ++mit)  {
          vcl_cout << "    " << mit->first << "\t\t";
          mit->second->print(vcl_cout);
          vcl_cout << "      distance tolerance: " << conf_objs_d_tol[i][mit->first].first << " meter to " << conf_objs_d_tol[i][mit->first].second << " meter " << vcl_endl;
        }
      }
    }
    unsigned num_locs = 0;
    vcl_cout << " Index type: " << index_name() << vcl_endl;
    vcl_cout << " Tile " << tile_id() << " has " << loc_leaves.size() << " leaves for matcher" << vcl_endl;
    for (int i = 0; i < loc_leaves.size(); i++) {
      vcl_cout << "  leaf " << i << " has " << loc_leaves[i]->hyps_->locs_.size() << " locations" << vcl_endl;
      num_locs += (unsigned)loc_leaves[i]->hyps_->locs_.size();
    }
    vcl_cout << "  matching will operate on " << num_locs << " locations in " << loc_leaves.size() << vcl_endl;
#if 0
    // visualize the configuration query
    if (cam_string.size() < 30) {
      //query->generate_top_views(out_folder());
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
    boxm2_volm_conf_matcher matcher(query, tile_id(), loc_leaves, idx_folder(), out_folder(), cand_poly, buffer_capacity());

    vcl_cout << " Start the configuration matcher ..." << vcl_endl;
    int matched_locs = matcher.conf_match_cpp(index_name(), leaf_id());
    if (matched_locs == 0) {
      log << "ERROR: configurational matcher failed on tile " << tile_id() << ", leaf " << leaf_id() << "!\n";
      return volm_io::EXE_MATCHER_FAILED;
    }
    vcl_cout << " Matching " << matched_locs << " and " << query->ncam() << " cameras per location consumes " << t.all()/(1000.0*60.0) << " seconds" << vcl_endl;
    return volm_io::SUCCESS;
  }
  else  // read the score for given leaf
  {
    vcl_cout << "--------- Reading configuration matcher score on tile " << tile_id() << " ------------ " << vcl_endl;
    vcl_stringstream score_file_pre;
    score_file_pre << out_folder() << "/conf_score_tile_" << tile_id();
    for (unsigned i = 0; i < loc_leaves.size(); i++)
    {
      volm_geo_index_node_sptr leaf = loc_leaves[i];
      vcl_string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name() + ".bin";
      if (!vul_file::exists(score_bin_file)) {
        continue;  // this leaf may not have any location inside candidate region and therefore no score stored
      }
      volm_conf_buffer<volm_conf_score> score_idx(buffer_capacity());
      score_idx.initialize_read(score_bin_file);
      vgl_point_3d<double> h_pt;
      while (leaf->hyps_->get_next(0,1,h_pt))
      {
        if (is_cand && !volm_candidate_list::inside_candidate_region(cand_poly, h_pt.x(), h_pt.y()))
          continue;
        volm_conf_score score_in;
        score_idx.get_next(score_in);
        vcl_cout << "location: " << h_pt.x() << ", " << h_pt.y() << " (tile " << tile_id() << "): --> ";
        score_in.print(vcl_cout);
      }
      score_idx.finalize();
    }
  }
}