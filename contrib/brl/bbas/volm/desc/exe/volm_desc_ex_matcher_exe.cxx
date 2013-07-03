//:
// \file
// \brief executable to match queries based on the existance of distinguishable objects
// \author Yi Dong
// \date June 04, 2013

#include <volm/desc/volm_desc_ex_matcher.h>
#include <volm/desc/volm_desc_ex_land_only_matcher.h>
#include <volm/desc/volm_desc_ex_2d_matcher.h>
#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_buffered_index.h>
#include <vul/vul_arg.h>

void error_report(vcl_string error_file, vcl_string error_msg)
{
  vcl_cerr << error_msg;
  volm_io::write_post_processing_log(error_file, error_msg);
}

int main(int argc, char** argv)
{
  // query input
  vul_arg<vcl_string> depth_scene("-dms", "depth map scene file", "");
  // index and geolocation
  vul_arg<unsigned> tile_id("-tile", "tile id to specify which tile are being matched", 100);
  vul_arg<vcl_string> geo_hypo_folder("-geo", "folder where the geolocation for this tile is stored", "");
  vul_arg<vcl_string> desc_index_folder("-index", "directory that contains the created wr3db indices", "");
  vul_arg<vcl_string> out_folder("-out", "output folder for the query image", "");
  vul_arg<float> buffer_capacity("-buff", "buffer size used for loading indices", 2.0f);
  vul_arg<bool> is_land_only("-land", "option to execute matcher using ex_land_only descriptor", false);
  vul_arg<bool> is_ex_2d("-ex2d", "option to execute 2D existance matcher", false);
  // post processing related
  vul_arg<vcl_string> gt_file("-gt", "ground truth files", "");
  vul_arg<float> kl("-kl", "parameter for nonlinear score scaling", 200.0f);
  vul_arg<float> ku("-ku", "parameter for nonlinear score scaling", 10.0f);
  vul_arg<float> threshold("-thres", "threshold ratio for generating prob map", 0.3);
  vul_arg<unsigned> test_id("-testid", "phase 1 test id", 0);
  vul_arg<unsigned> id("-imgid", "query image id", 101);
  vul_arg<unsigned> thresc("-thresc", "threshold that used to create candidate list", 0);
  vul_arg<unsigned> top_size("-top", "desired top list for each candidate list", 1);
  // options
  vul_arg<bool> is_matcher("-match", "option to execute the matcher", false);
  vul_arg<bool> is_cand("-cand", "option to execute the candidate list generation", false);

  vul_arg_parse(argc, argv);

  vcl_stringstream image_name;
  if (id() < 10)
    image_name << "p1a_test" << test_id() << "_0" << id();
  else
    image_name << "p1a_test" << test_id() << "_" << id();
  // run the existance matcher and generate probability map
  if (is_matcher()) {
    // check the argument 
    if (depth_scene().compare("") == 0 || geo_hypo_folder().compare("") == 0 ||
        desc_index_folder().compare("") == 0 || out_folder().compare("") == 0 ||
        gt_file().compare("") == 0 || id() > 100 || test_id() == 0 || tile_id() == 100)
    {
      vcl_cerr << " ERROR: arguemnts error, check the input " << vcl_endl;
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    // error log
    vcl_stringstream err_log;
    vcl_stringstream err_log_file;
    err_log_file << out_folder() << "/err_log_tile_" << tile_id() << ".xml" << vcl_endl;
    
    
    // load the depth map scene
    if (!vul_file::exists(depth_scene())) {
      err_log << " ERROR: can not find depth map scene file" << depth_scene() << '\n';
      vcl_cerr << err_log;
      volm_io::write_post_processing_log(err_log_file.str(), err_log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    depth_map_scene_sptr dms = new depth_map_scene;
    vsl_b_ifstream dis(depth_scene().c_str());
    dms->b_read(dis);
    dis.close();

    // fetch the ground truth location
    if (!vul_file::exists(gt_file())) {
      err_log << " ERROR: can not find ground truth file" << gt_file() << '\n';
      error_report(err_log_file.str(), err_log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_pair<vcl_string, int>, vcl_string> > > samples;
    unsigned int cnt = volm_io::read_gt_file(gt_file(), samples);

    int img_info_id = -1;
    for (unsigned kk = 0; kk < samples.size(); kk++) {
      if (samples[kk].second.first.second == id()) {
        img_info_id = kk;
        break;
      }
    }
    //if (query_img_info.size() <= img_id()) {
    if (img_info_id < 0) {
      vcl_cerr << "query image id: " << id() << " cannot be found in the gt loc file: " << gt_file() << "!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    vgl_point_3d<double> gt_loc;
    gt_loc = samples[img_info_id].first;

    // create coast tiles
    vcl_vector<volm_tile> tiles = volm_tile::generate_p1_wr2_tiles();

    // create ex_matcher
    vcl_string params_file_pre = desc_index_folder() + "/desc_index_tile_0";
    volm_buffered_index_params params;
    if (!params.read_ex_param_file(params_file_pre)) {
      err_log << " ERROR: fetching parameter failed from file: " << params_file_pre << ".params\n";
      error_report(err_log_file.str(), err_log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    volm_desc_matcher_sptr ex_matcher;
    if (is_land_only()) {
      ex_matcher = new volm_desc_ex_land_only_matcher(dms, params.radius, params.nlands, 0);
    }
    else if (is_ex_2d()) {
      ex_matcher = new volm_desc_ex_2d_matcher(dms, params.radius, params.nlands, 0);
    }
    else {
      ex_matcher = new volm_desc_ex_matcher(dms, params.radius, params.norients, params.nlands, 0);
    }
    
    vcl_cout << " ex_matcher is " << ex_matcher->get_index_type_str() << vcl_endl;

    // create query
    volm_desc_sptr query = ex_matcher->create_query_desc();
    vcl_string query_file = out_folder() + "/" + image_name.str() + "_query.svg";
    query->visualize(query_file, 2);

    vcl_cout << " query = ";
    query->print();
    vcl_cout << vcl_endl;

    // start the matcher
    vcl_cout << " =========== Start to execute existance matcher on tile " << tile_id() << " for image: " << image_name.str() << " ===============" << vcl_endl;
    vcl_cout << " \t Descriptor type : " << ex_matcher->get_index_type_str() << vcl_endl;
    if (tile_id() != 10) {
      if (!ex_matcher->matcher(query, geo_hypo_folder(), desc_index_folder(), buffer_capacity(), tile_id())) {
        err_log << " ERROR: matcher for tile " << tile_id() << " failed\n";
        error_report(err_log_file.str(), err_log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }
      // save the score binary
      if (!ex_matcher->write_out(out_folder(), tile_id())) {
        err_log << " ERROR: save the score binary file for tile " << tile_id() << " failed\n";
        error_report(err_log_file.str(), err_log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }
    }
    // create probability map
    float gt_score = 0.0f;
    if (!ex_matcher->create_prob_map(geo_hypo_folder(), out_folder(), tile_id(), tiles[tile_id()], gt_loc, gt_score)) {
      err_log << " ERROR: creating probability map for tile " << tile_id() << " failed\n";
      error_report(err_log_file.str(), err_log.str());
      return volm_io::POST_PROCESS_FAILED;
    }
    if (gt_score != 0) {
      // record the ground score
      vcl_stringstream gt_log;
      vcl_string gt_log_file = out_folder() + "/gt_score.xml";
      gt_log << " closest geolocation relative to ground truth [" << gt_loc.x() << ", " << gt_loc.y()
             << "] is in tile " << tile_id() << ", having score = " << gt_score << '\n';
      volm_io::write_post_processing_log(gt_log_file, gt_log.str());
    }
    vcl_cout << "\t geo hypothesis folder: " << geo_hypo_folder() << vcl_endl;
    vcl_cout << "\t existance index folder: " << desc_index_folder() << vcl_endl;
    vcl_cout << "\t query image has " << query->get_area() << " bins" << vcl_endl;
    
    if (gt_score != 0)
      vcl_cout << "\t ground truth location is in tile " << tile_id() << " with score = " << gt_score << vcl_endl;

    // create scaled probability map
    float thres_value = threshold();
    ex_matcher->check_threshold(query, thres_value);
    if (thres_value != threshold())
      vcl_cout << "\t NOTE: the threshold given has changed from " << threshold() << " to " << thres_value << vcl_endl;
    vcl_cout << "\t threshold used for scaling probability maps: " << thres_value << vcl_endl;
    if (!volm_desc_matcher::create_scaled_prob_map(out_folder(), tiles[tile_id()], tile_id(), ku(), kl(), thres_value)) {
      err_log << " ERROR: creating scaled probability map for tile " << tile_id() << " failed\n";
      error_report(err_log_file.str(), err_log.str());
      return volm_io::POST_PROCESS_FAILED;
    }
    vcl_cout << " ========================================== Finish ===============================================" << vcl_endl;
  }
  // generate candidate list once we have the scaled_probability_map
  if (is_cand()) {
    if (out_folder().compare("") == 0) {
      vcl_cerr << " ERROR: output folder and probability map folder can not be empty when creating candidate list.  Check input" << vcl_endl;
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    vcl_stringstream log;
    vcl_stringstream log_file;
    log_file << out_folder() << "/error_log_candidate_list.xml";

    vcl_cout << " =========== Start to create candidate list for image: " << image_name.str() << " ===============" << vcl_endl;
    vcl_stringstream prob_map_folder;
    prob_map_folder << out_folder() << "/ProbMap_scaled_" << threshold();

    if (!vul_file::exists(prob_map_folder.str())) {
      log << " can not find the directory for probability map: " << prob_map_folder.str() << '\n';
      error_report(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    vcl_stringstream cand_folder;
    cand_folder << out_folder() << "/T_" << thresc() ;
    if( !vul_file::is_directory(cand_folder.str()))
    vul_file::make_directory(cand_folder.str());

    vcl_cout << "\t threshold: " << thresc() << vcl_endl;
    vcl_cout << "\t threshold used for the probability maps: " << threshold() << vcl_endl;
    vcl_cout << "\t probability maps are stored in: " << prob_map_folder.str() << vcl_endl;
    vcl_cout << "\t result stored in: " << cand_folder.str() << vcl_endl;
    if (!volm_desc_matcher::create_candidate_list(prob_map_folder.str(), cand_folder.str(), thresc(), top_size(), ku(), kl(), threshold(), test_id(), id())) {
      log << " creating candidate list failed for image " << id() << " at threshold " << thresc() << '\n';
      error_report(log_file.str(), log.str());
      vcl_cout << " ========================================== Failed ========================================" << vcl_endl;
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    vcl_cout << " ========================================== Finish ========================================" << vcl_endl;
    return volm_io::SUCCESS;
  }

  return volm_io::SUCCESS;
}
