//:
// \file
// \brief executable to match a given volumetric query and a camera estimate to an indexed reference volume
// \author Ozge C. Ozcanli
// \date Oct 8, 2012

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <boxm2/volm/boxm2_volm_matcher_p1.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bbas/bocl/bocl_manager.h>
#include <bbas/bocl/bocl_device.h>
#include <vcl_set.h>
#include <vul/vul_timer.h>
#include <vpl/vpl.h>

#if 0
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<vcl_string> input_file("-input", "text file with a list of hyp and index files to process", "");
  vul_arg<vcl_string> score_folder("-score", "score folder", "");  // composer will read the score files from this folder for this job
  vul_arg<vcl_string> rat_folder("-rat", "rationale folder", "");  // composer will write top 30 to this folder
  vul_arg<vcl_string> out_folder("-out", "job output folder", "");
  vul_arg<float> thres("-t", "threshold to scale the score values to [1,254], piecewise linear s.t. [0,t) -> [1,127), [t,1] -> [127,254]", 0.5);
  vul_arg<bool> save_images("-save", "save out images or not", false);
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  vcl_stringstream log;
  //: check input parameters
  if (input_file().compare("") == 0 || out_folder().compare("") == 0) {
    vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
    vul_arg_display_usage_and_exit();
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_file(), dm, img_category)) {
    vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR, 100);
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  float threshold = thres();
  vcl_cout << "composer using threshold: " << threshold << " to scale the value to [1,127]\n";
  // generate tile and output imges
  vcl_vector<volm_tile> tiles;
  vcl_vector<vil_image_view<vxl_byte> > out_imgs;
  if (img_category == "desert") {
    tiles = volm_tile::generate_p1_wr1_tiles();
  }
  else if (img_category == "coast") {
    tiles = volm_tile::generate_p1_wr2_tiles();
  }
  else{
    vcl_string error_msg = " ERROR: unknown image category (should be desert or coast), generating tile for output failed";
    vcl_cerr << error_msg << '\n';
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR, 100, error_msg);
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  for (unsigned i = 0; i < tiles.size(); ++i) {
    vil_image_view<vxl_byte> out(3601, 3601);
    out.fill(volm_io::UNEVALUATED);
    out_imgs.push_back(out);
  }


  // read the input hyp and index file pairs, only from the image category
  vcl_ifstream input_fs(input_file().c_str());
  vcl_vector<vcl_string> hyp_files;
  vcl_vector<vcl_string> index_files;
  vcl_vector<vcl_string> score_files;
  vcl_vector<vcl_string> cam_files;
  while (!input_fs.eof()) {
    vcl_string region_type, hyp_file, ind_file; unsigned gpu_id;
    input_fs >> region_type;  // desert or coast
    input_fs >> hyp_file;
    input_fs >> ind_file;
    input_fs >> gpu_id;
    if (region_type.compare("") == 0 || hyp_file.compare("") == 0 || ind_file.compare("") == 0)
      break;
    if (region_type.compare(img_category) != 0)
      continue;

    hyp_files.push_back(hyp_file);
    index_files.push_back(ind_file);
    ind_file = vul_file::strip_directory(ind_file);
    ind_file = vul_file::strip_extension(ind_file);
    score_files.push_back(score_folder() + "/" + ind_file + "_score.bin");
    cam_files.push_back(score_folder() + "/" + ind_file + "_camera.bin");
  }

  if (!hyp_files.size()) {
    log << "cannot read any hyp index file pairs from: " << input_file() << " for category: " << img_category << "!\n";
    volm_io::write_composer_log(out_folder(), log.str());

    // save images
    if (save_images()) {
      for (unsigned i = 0; i < out_imgs.size(); i++) {
        vcl_string out_name = out_folder() + "/ProbMap_" + tiles[i].get_string() + ".tif";
        vil_save(out_imgs[i], out_name.c_str());
      }
    }
    volm_io::write_status(out_folder(), volm_io::SUCCESS, 100);
    return volm_io::SUCCESS;
  }

  // read the params of index 0, assume the container params are the same for all these indices
  boxm2_volm_wr3db_index_params q_params;
  if (!q_params.read_params_file(index_files[0])) {
    vcl_cerr << " cannot read params file for " << index_files[0] << '\n';
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100);
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // create query array
  // create containers
  volm_spherical_container_sptr sph = new volm_spherical_container(q_params.solid_angle,q_params.vmin,q_params.dmax);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, q_params.cap_angle, q_params.point_angle, q_params.top_angle, q_params.bottom_angle);
  volm_query_sptr query = new volm_query(cam_file(), label_file(), sph, sph_shell, false);

  query->draw_query_regions(rat_folder() + "/query_regions.png");

  if (query->get_cam_num() == 0) {
    log << "query has 0 cameras! Exiting!\n";
    volm_io::write_composer_log(out_folder(), log.str());
    if (save_images()) {
    for (unsigned i = 0; i < out_imgs.size(); i++) {
      vcl_string out_name = out_folder() + "/ProbMap_" + tiles[i].get_string() + ".tif";
      vil_save(out_imgs[i], out_name.c_str());
      }
    }
    volm_io::write_status(out_folder(), volm_io::SUCCESS, 100);
    return volm_io::SUCCESS;
  }
  vul_timer t;

  // halt till all camera.bin files are available
  volm_io::write_status(out_folder(), volm_io::COMPOSE_HALT);

  bool all_available = false;
  while (!all_available) {
    all_available = true;
    for (unsigned i = 0; i < cam_files.size(); i++) {
      if (!vul_file::exists(cam_files[i]) && !(vul_file::size(cam_files[i]) > 0)) {
        log << " halting for: " << cam_files[i] << "!\n";
        vcl_cout << " halting for: " << cam_files[i] << "!\n";
        volm_io::write_status(out_folder(), volm_io::SCORE_FILE_MISSING, 100);
        all_available = false;
        vpl_sleep(30);
        break;
      }
    }
  }
  volm_io::write_status(out_folder(), volm_io::COMPOSE_STARTED);
  // composer to generate final output
  vcl_cout << "\n Start to generate output images using tiles" << vcl_endl;


  // keep a map of scores to cam ids for top 30
  vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > > top_matches;
  vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > >::iterator top_matches_iter;


  vbl_array_2d<bool> mask;
  vbl_array_2d<float> kernel;
  volm_tile::get_uncertainty_region((float)10, (float)10, (float)0.01, mask, kernel);
  for (unsigned i = 0; i < score_files.size(); i++)
  {
    boxm2_volm_wr3db_index_params params;
    params.read_params_file(index_files[i]);
    unsigned long ind_size;
    boxm2_volm_wr3db_index_params::read_size_file(index_files[i], ind_size);

    // read and scale the scores from binary
    vsl_b_ifstream is(score_files[i].c_str());
    vsl_b_ifstream isc(cam_files[i].c_str());
    vcl_vector<float> scores;
    vcl_vector<unsigned> cam_ids;
    float score;
    unsigned cam_id;
    for (unsigned jj = 0; jj < ind_size; jj++) {
      vsl_b_read(is, score);
      vsl_b_read(isc,cam_id);
      scores.push_back(score);
      cam_ids.push_back(cam_id);
#if 1
      vcl_cout << " for index " << jj << ", score = " << score
               << ",\t camera id = " << cam_id
               << "\t with valid rays = " << query->get_valid_ray_num(cam_id)
               << "\t camera = " << query->get_cam_string(cam_id)
               << vcl_endl;
#endif
    }

    // read in the hypotheses
    volm_loc_hyp hyp(hyp_files[i]);
    vcl_cout << hyp.size() << " hypotheses read from: " << hyp_files[i] << '\n'
             << scores.size() << " scores read from: " << score_files[i] << " params start: " << params.start << " skip: " << params.skip << vcl_endl;

    vgl_point_3d<double> h_pt;
    unsigned ind_idx = 0;
    while (hyp.get_next(params.start, params.skip, h_pt))
    {
      //unsigned ind_idx = (unsigned)vcl_floor((float)(hyp.current_-params.skip)/params.skip) + params.start;
#if 0
      vcl_cout << "Processing hypothesis x: " << h_pt.x() << " y: " << h_pt.y() << " z: " << h_pt.z()
               << ", index id: " << ind_idx << " score = " << scores[ind_idx] << ", best cam_id = " << cam_ids[ind_idx]
               << vcl_endl;
#endif
      // locate the tile/img pixel
      unsigned u, v;
      for (unsigned k = 0; k < tiles.size(); k++) {
        if (tiles[k].global_to_img(h_pt.x() , h_pt.y(), u, v))
        {
          // check if this is the best value for this pixel
          float current_score = 0;
          if ((int)out_imgs[k](u,v) > 0)
            //current_score = (((float)out_imgs[k](u,v))-1.0f)/volm_io::SCALE_VALUE;
            current_score = volm_io::scale_score_to_0_1(out_imgs[k](u,v), threshold);
          if (scores[ind_idx] > current_score) {
            if (cam_ids[ind_idx] < query->get_cam_num()) {
              volm_rationale r;
              r.cam_id = cam_ids[ind_idx];
              r.index_id = ind_idx;
              r.lat = h_pt.y();
              r.lon = h_pt.x();
              r.elev = h_pt.z();
              r.index_file = index_files[i];
              r.score_file = score_files[i];
              top_matches.insert(vcl_pair<float, volm_rationale>(scores[ind_idx], r));
              //vcl_cout << "inserting " << scores[ind_idx] << " cam id: " << cam_ids[ind_idx] << vcl_endl;
              if (top_matches.size() > 30) {
                top_matches_iter = top_matches.end();
                top_matches_iter--;
                top_matches.erase(top_matches_iter);
              }
            }
            else {
              vcl_cerr << "cam id: " << cam_ids[ind_idx] << " is invalid, query object has: "
                       << query->get_cam_num() << " cams. In tile " << tiles[k].get_string()
                       << " loc: (" << u << ", " << v << ") skipping rationale..\n"
                       << "score file is: " << score_files[i] << " id in the file: " << ind_idx
                       << " hypo id: " << hyp.current_ << '\n';
            }
            out_imgs[k](u,v) = volm_io::scale_score_to_1_255(threshold, scores[ind_idx]);
#if 0
            out_imgs[k](u,v) = (vxl_byte)(scores[ind_idx]*volm_io::SCALE_VALUE + 1);
            vcl_cout << "writing score: " << scores[ind_idx] << " scaled to " << (int)out_imgs[k](u,v) << " to loc: " << u << ", " << v << vcl_endl;
            volm_tile::mark_uncertainty_region(u, v, scores[ind_idx], mask, kernel, out_imgs[k]);
#endif
          }
        }
      }
      ind_idx++;
    }
  }

  // save images
  if (save_images()) {
    for (unsigned i = 0; i < out_imgs.size(); i++) {
      vcl_string out_name = out_folder() + "/ProbMap_" + tiles[i].get_string() + ".tif";
      vil_save(out_imgs[i], out_name.c_str());
    }
  }
  // save top 30 into rationale folder as a text file
  vcl_string top_matches_filename = rat_folder() + "/top_matches.txt";
  if (!volm_rationale::write_top_matches(top_matches, top_matches_filename))
    vcl_cerr << "cannot write to " << top_matches_filename << '\n';

  unsigned cnt = 0;
  vcl_cout << "creating rationale for top: " << top_matches.size() << vcl_endl;
  for (top_matches_iter = top_matches.begin(); top_matches_iter != top_matches.end(); top_matches_iter++) {
    volm_rationale r = top_matches_iter->second;
    vcl_string cam_postfix = query->get_cam_string(r.cam_id);
    vcl_stringstream str;
    str << rat_folder() + "/" << "query_top_" << cnt++ << cam_postfix << ".png";
    vcl_cout << "writing rat to: " << str.str() << vcl_endl; vcl_cout.flush();
    query->draw_query_image(r.cam_id, str.str());
  }

  volm_io::write_composer_log(out_folder(), log.str());
  volm_io::write_status(out_folder(), volm_io::SUCCESS, 100);
  return volm_io::SUCCESS;
}

#endif

int main(int argc,  char** argv)
{
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<vcl_string> geo_index_folder("-geo", "folder to read the geo index and the hypo", ""); // folder to read the geo_index and hypos for each leaf
  vul_arg<vcl_string> candidate_list("-cand", "candidate list for given query (txt file)", "");  // candidate list file containing polygons
  vul_arg<float>      buffer_capacity("-buff", "index buffer capacity (GB)", 1.0f);
  vul_arg<unsigned>   tile_id("-tile", "ID of the tile that current matcher consider",3);
  vul_arg<vcl_string> out_folder("-out", "job output folder", "");
  vul_arg<float>      thres("-t", "threshold to scale the score values to [1,254], piecewise linear s.t. [0,t) -> [1,127), [t,1] -> [127,254]", 0.5);
  vul_arg<bool>       save_images("-save", "save out images or not", false);
  vul_arg<bool>       use_ps0("-ps0", "choose to use pass 0 regional matcher", false);
  vul_arg<bool>       use_ps1("-ps1", "choose to use pass 1 obj_based order matcher", false);
  vul_arg<bool>       use_ps2("-ps2", "choose to use pass 2 obj_based orient matcher", false);
  vul_arg<bool>       logger("-logger", "designate one of the exes as logger", false); // if -logger exists then this one is logger exe is to do logging and generate the status.xml file
  vul_arg<bool>       gen_query("-query", "choose to create query or not", false);

  vul_arg_parse(argc, argv);
  vcl_cout << "argc: " << argc << vcl_endl;
  vcl_stringstream log;
  bool do_log = false;
  if (logger())
    do_log = true;
  //: check input parameters
  if (geo_index_folder().compare("") == 0 || out_folder().compare("") == 0) {
    vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
    vul_arg_display_usage_and_exit();
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  float threshold = thres();
  vcl_cout << " threshold = " << threshold << " --> corresponding to 127 (unknow)" << vcl_endl;

  // read depth map scene
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_file(), dm, img_category)) {
    vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR, 100);
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  // read camera space in
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double top_fov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, top_fov, top_fov_dev, altitude, lat, lon)) {
    log << "problem parsing: " << cam_file() << '\n';
    if (do_log) { volm_io::write_log(out_folder(), log.str()); }
    volm_io::write_status(out_folder(), volm_io::CAM_FILE_IO_ERROR);
    vcl_cerr << log.str();
    return volm_io::CAM_FILE_IO_ERROR;
  }
  vcl_cout << "cam params\nheading: " << heading << " dev: " << heading_dev
           << "\ntilt: " << tilt << " dev: " << tilt_dev
           << "\nroll: " << roll << " dev: " << roll_dev
           << "\ntop_fov: " << top_fov << " dev: " << top_fov_dev
           << " alt: " << altitude << vcl_endl;
  // generate tile and output imges
  vcl_vector<volm_tile> tiles;
  if (img_category == "desert") {
    tiles = volm_tile::generate_p1_wr1_tiles();
  }
  else if (img_category == "coast") {
    tiles = volm_tile::generate_p1_wr2_tiles();
  }
  else{
    vcl_string error_msg = " ERROR: unknown image category (should be desert or coast), generating tile for output failed";
    vcl_cerr << error_msg << '\n';
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR, 100, error_msg);
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  volm_tile tile = tiles[tile_id()];
  vil_image_view<vxl_byte> out_img(3601, 3601);
  out_img.fill(volm_io::UNEVALUATED);
  // read the geo_index, create geo_index tree, read in parameters
  vcl_stringstream file_name_pre;
  file_name_pre << geo_index_folder() << "geo_index_tile_" << tile_id();
  vcl_cout << " geo_index_hyps_file = " << file_name_pre.str() + ".txt" << vcl_endl;

  if (!vul_file::exists(file_name_pre.str() + ".txt")) {
    log << " ERROR: gen_index_folder is wrong (missing last slash/ ?), no geo_index_files found in " << geo_index_folder() << '\n';
    if (do_log) { volm_io::write_status(out_folder(), volm_io::GEO_INDEX_FILE_MISSING); volm_io::write_composer_log(out_folder(), log.str()); }
    vcl_cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());

  // check whether we have candidate list for this query
  bool is_candidate = false;
  vgl_polygon<double> cand_poly;


  if ( candidate_list().compare("") != 0) {
    vcl_cout << " candidate list = " <<  candidate_list() << vcl_endl;
    if ( vul_file::extension(candidate_list()).compare(".txt") == 0) {
      is_candidate = true;
      volm_io::read_polygons(candidate_list(), cand_poly);
    }
    else {
      log << " ERROR: candidate list exist but with wrong format, only txt allowed" << candidate_list() << '\n';
      if (do_log) volm_io::write_composer_log(out_folder(), log.str());
      vcl_cerr << log.str();
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
      return volm_io::EXE_ARGUMENT_ERROR;
    }
  }
  else {
    vcl_cout << " NO candidate list for this query , search over full index space" << vcl_endl;
  }
  // prune the tree, only leaves with non-zero hypos are left
  if (is_candidate) {
    volm_geo_index::prune_tree(root, cand_poly);
  }
  vcl_vector<volm_geo_index_node_sptr> all_leaves;
  volm_geo_index::get_leaves(root, all_leaves);

  // prune the tree to get rid of leaves without any hypothesis
  vcl_vector<volm_geo_index_node_sptr> leaves;
  for (unsigned li = 0; li < all_leaves.size(); li++)
    if (all_leaves[li]->hyps_)
      leaves.push_back(all_leaves[li]);

  // check the candidate list by hypotheses
  //if (is_candidate) {
  //  for (unsigned li = 0; li < leaves.size(); li++) {
  //    unsigned h_size = leaves[li]->hyps_->size();
  //    vcl_vector<vgl_point_3d<double> > locs = leaves[li]->hyps_->locs_;
  //    bool has_hypo = false;
  //    for (unsigned hi = 0; (hi < h_size); hi++)
  //      if (cand_poly.contains(locs[hi].x(), locs[hi].y()))
  //        has_hypo = true;
  //    if (!has_hypo)
  //      leaves.erase(leaves.begin()+li);
  //  }
  //}
  // create query (mainly for the camera space)
  // read in the parameter, create depth_interval table and spherical shell container
  boxm2_volm_wr3db_index_params params;
  vcl_string index_file = leaves[0]->get_index_name(file_name_pre.str());

  if (!params.read_params_file(index_file)) {
    log << " ERROR: cannot read params file from " << index_file << '\n';
    if (do_log) {
       volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
       volm_io::write_log(out_folder(), log.str());
    }
    vcl_cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // create query
  volm_query_sptr query;
  unsigned layer_size = 0;
  if (gen_query()) {
    volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);
    volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, params.cap_angle, params.point_angle, params.top_angle, params.bottom_angle);
    layer_size = (unsigned)sph_shell->get_container_size();
    query = new volm_query(cam_file(), label_file(), sph, sph_shell, false);
  }


  if (use_ps0()) {
    vcl_cerr << " TO BE IMPLEMENTED: composer for pass 0 regional matcher\n";
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  if (use_ps1()) {
    vcl_stringstream out_fname_pre;
    out_fname_pre << out_folder() << "geo_index_tile_" << tile_id();
    unsigned n_ind_above_thres = 0;
    unsigned total_ind = 0;
    for (unsigned li = 0 ; li < leaves.size(); li++) {
      // locate the score file for current leaf
      vcl_string score_file = leaves[li]->get_score_txt_name(out_fname_pre.str(), 1);
      if (!vul_file::exists(score_file)) {
        vcl_cout << "WARNING: " << score_file << " is not FOUND" << vcl_endl;
        continue;
      }
#if 0
      if (!vul_file::exists(score_file)) {
        log << " ERROR: can not find output score file\n" << score_file << '\n';
        if (do_log) volm_io::write_composer_log(out_folder(), log.str());
        vcl_cerr << log.str();
        volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
        return volm_io::EXE_ARGUMENT_ERROR;
      }
#endif
      // read score and data for current leaf
      vcl_ifstream ifs(score_file.c_str());
      vcl_vector<boxm2_volm_score_out_max> score_all;
      while ( !ifs.eof() ) {
        boxm2_volm_score_out_max score;
        ifs >> score.hypo_id_; ifs >> score.max_score_;  ifs >> score.max_cam_id_;
        score_all.push_back(score);
        total_ind++;
        if (score.max_score_ > threshold)
          n_ind_above_thres++;
      }

      // create the probability map
      vcl_cout << " filename = " << score_file << vcl_endl;
      for (unsigned i = 0; i < score_all.size(); i++) {
        vgl_point_3d<double> h_pt = leaves[li]->hyps_->locs_[score_all[i].hypo_id_];
        unsigned u, v;
        if (tile.global_to_img(h_pt.x(), h_pt.y(), u, v)) {
          // check if this is the best values for this pixel
          float current_score = 0.0f;
          if ( (int)out_img(u,v) > 0)
            current_score = volm_io::scale_score_to_0_1(out_img(u,v), threshold);
          if (score_all[i].max_score_ > current_score)
            out_img(u,v) = volm_io::scale_score_to_1_255(threshold, score_all[i].max_score_);
        }
      }
    }
    // save images for tile[tile_id]
    vcl_stringstream prob_str;
    prob_str << out_fname_pre.str() << "_t_" << threshold << "_ps1_prob_map_.tif";
    vcl_string prob_img = prob_str.str();
    vil_save(out_img, prob_img.c_str());

    // get the ground truth score and camera
    unsigned hyp_gt = 0;
    volm_geo_index_node_sptr leaf_gt = volm_geo_index::get_closest(root, lat, lon, hyp_gt);
    vcl_string score_file = leaf_gt->get_score_txt_name(out_fname_pre.str(), 1);
    if (!vul_file::exists(score_file)) {
      log << " ERROR: can not find output score file\n" << score_file << '\n';
      if (do_log) volm_io::write_composer_log(out_folder(), log.str());
      vcl_cerr << log.str();
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    vcl_ifstream ifs(score_file.c_str());
    float score_gt = 0.0f;
    unsigned cam_gt = 0, h_gt = 0;;
    while ( !ifs.eof() ) {
      unsigned h_id, cam_id;
      float score;
      ifs >> h_id;    ifs >> score;     ifs >> cam_id;
      if (h_id == hyp_gt) {
        h_gt = h_id; score_gt = score;  cam_gt = cam_id; break;
      }
    }

    if (gen_query()) {
      vcl_cout << " ground truth [lon, lat] = " << lon << ", " << lat << "] is in leaf\n\t"
               << leaf_gt->get_score_txt_name(out_fname_pre.str(), 1)
               << "\n\t\t closest id = " << h_gt << " ---> [lon, lat] = " <<  leaf_gt->hyps_->locs_[h_gt]
               << "\n\t\t score = " << score_gt
               << "\n\t\t camera = " << cam_gt << " ---> " << query->get_cam_string(cam_gt)
               << vcl_endl;
      vcl_cerr << " ground truth [lon, lat] = " << lon << ", " << lat << "] is in leaf\n\t"
               << leaf_gt->get_score_txt_name(out_fname_pre.str(), 1)
               << "\n\t\t closest id = " << h_gt << " ---> [lon, lat] = " <<  leaf_gt->hyps_->locs_[h_gt]
               << "\n\t\t score = " << score_gt
               << "\n\t\t camera = " << cam_gt << " ---> " << query->get_cam_string(cam_gt)
               << '\n';
    }
    else {
      vcl_cout << " ground truth [lon, lat] = " << lon << ", " << lat << "] is in leaf\n\t"
               << leaf_gt->get_score_txt_name(out_fname_pre.str(), 1)
               << "\n\t\t closest id = " << h_gt << " ---> [lon, lat] = " <<  leaf_gt->hyps_->locs_[h_gt]
               << "\n\t\t score = " << score_gt
               << "\n\t\t camera = " << cam_gt
               << vcl_endl;
      vcl_cerr << " ground truth [lon, lat] = " << lon << ", " << lat << "] is in leaf\n\t"
               << leaf_gt->get_score_txt_name(out_fname_pre.str(), 1)
               << "\n\t\t closest id = " << h_gt << " ---> [lon, lat] = " <<  leaf_gt->hyps_->locs_[h_gt]
               << "\n\t\t score = " << score_gt
               << "\n\t\t camera = " << cam_gt
               << '\n';
    }

    vcl_cout << total_ind << " are evaluated and " << n_ind_above_thres << " locs has score higher than " << threshold << '\n'
             << " --> ROI = " << (float)(total_ind - n_ind_above_thres)/total_ind << vcl_endl;

    vcl_cerr << total_ind << " are evaluated and " << n_ind_above_thres << " locs has score higher than " << threshold << '\n'
             << " --> ROI = " << (float)(total_ind - n_ind_above_thres)/total_ind << '\n';

    if (gen_query()) {
      vcl_cout << " ray_based index image being created with layer_size = " << layer_size << " buffer_cap = " << buffer_capacity() << vcl_endl;
      // create ray_based index image given best camera
      boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(layer_size, buffer_capacity());
      ind->initialize_read(leaf_gt->get_index_name(file_name_pre.str()));
      vcl_cout << " the leaf containing ground truth is " << leaf_gt->get_index_name(file_name_pre.str());
      vgl_point_3d<double> h_pt;
      unsigned h_id;
      while (leaf_gt->hyps_->get_next(0, 1, h_pt) ) {
        vcl_vector<unsigned char> values(layer_size);
        h_id  = leaf_gt->hyps_->current_-1;
        ind->get_next(values);

        if (h_id == hyp_gt) {
          vcl_cout << " hypo " << h_id << " ---> " << h_pt << vcl_endl;
          vcl_string ind_img_fname = out_fname_pre.str() + "_ps1_ind_best_cam_" + query->get_cam_string(cam_gt) + ".png" ;
          vil_image_view<vil_rgb<vxl_byte> > img(dm->ni(), dm->nj());
          // initialize the image
          for (unsigned i = 0; i < dm->ni(); ++i)
            for (unsigned j = 0; j < dm->nj(); ++j) {
              img(i,j).r = (unsigned char)120;
              img(i,j).g = (unsigned char)120;
              img(i,j).b = (unsigned char)120;
            }
          query->depth_rgb_image(values, cam_gt, img);
          // save the images
          vil_save(img,ind_img_fname.c_str());
          // ground truth image
          vcl_string ind_gt_fname = out_fname_pre.str() + "_ps1_ind_gt_cam_" + query->get_cam_string(0) + ".png" ;
          vil_image_view<vil_rgb<vxl_byte> > img_gt(dm->ni(), dm->nj());
          // initialize the image
          for (unsigned i = 0; i < dm->ni(); ++i)
            for (unsigned j = 0; j < dm->nj(); ++j) {
              img_gt(i,j).r = (unsigned char)120;
              img_gt(i,j).g = (unsigned char)120;
              img_gt(i,j).b = (unsigned char)120;
            }
          query->depth_rgb_image(values, 0, img_gt);
          vil_save(img_gt,ind_gt_fname.c_str());
        }
      }
    }
  }
  if (use_ps2()) {
    vcl_cerr << " TO BE IMPLEMENTED: composer for pass 0 regional matcher\n";
    return volm_io::EXE_ARGUMENT_ERROR;
  }


  return volm_io::SUCCESS;
}
