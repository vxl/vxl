//:
// \file
// \executable to create the index ray_based depth image for the top 10 cameras
// \author Yi Dong
// \date Feb 21, 2013

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>
#include <vpgl/vpgl_utm.h>
#include <vnl/vnl_math.h>


int main(int argc, char** argv)
{
  vul_arg<std::string> gt_file("-gt_locs", "file with the gt locs of all test cases", "");
  vul_arg<std::string> geo_hypo_folder("-hypo", "folder prefix to read the geo hypotheses", "");
  vul_arg<std::string> geo_index_folder("-index", "folder prefix to read the geo index and the hypo","");
  vul_arg<unsigned>   point_angle("-point", "point_angle use to locate geo index", 0);
  vul_arg<std::string> candidate_list("-cand", "candidate list if exist", "");
  vul_arg<std::string> cam_bin("-cam", "camera space binary", "");                                // query -- camera space binary
  vul_arg<std::string> dms_bin("-dms", "depth_map_scene binary", "");                             // query -- depth map scene
  vul_arg<std::string> img("-img", "query images, to get the image size", "");                    // query -- actual query image
  vul_arg<std::string> sph_bin("-sph", "spherical shell binary", "");                             // query -- spherical_shell binary
  vul_arg<std::string> query_bin("-query", "query binary file", "");
  vul_arg<float>      buffer_capacity("-buff", "index buffer capacity (GB)", 1.0f);
  vul_arg<std::string> out("-out", "job output folder", "");
  vul_arg<unsigned>   id("-id", "id of the test image", 9999);
  vul_arg<unsigned>   pass_id("-pass", "from pass 0 to pass 1", 1);
  vul_arg_parse(argc, argv);

  // check input
  if (out().compare("") == 0 || geo_hypo_folder().compare("") == 0 || geo_index_folder().compare("") == 0 || point_angle() == 0 ||
      gt_file().compare("") == 0 ||
      pass_id() > 2 ||
      img().compare("") == 0 ||
      query_bin().compare("") == 0 ||
      id() > 1000)
  {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::stringstream log;
  std::string log_file = out() + "/render_img_log.xml";

  // read the gt location for test image id, i.e., lat and lon
  if (!vul_file::exists(gt_file())) {
    log << "ERROR: can not find ground truth position file -->" << gt_file() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::vector<std::pair<vgl_point_3d<double>, std::pair<std::pair<std::string, int>, std::string> > > samples;
  unsigned int cnt = volm_io::read_gt_file(gt_file(), samples);
  if (id() >= cnt) {
    log << "ERROR: the file: " << gt_file() << " does not contain test id: " << id() << "!\n";
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // check whether we have candidate list for this query
  bool is_candidate = false;
  vgl_polygon<double> cand_poly;
  std::cout << " candidate list = " <<  candidate_list() << std::endl;
  if ( candidate_list().compare("") != 0) {
    if (!vul_file::exists(candidate_list())) {
      log << " ERROR: can not fine candidate list file: " << candidate_list() << '\n';
      volm_io::write_post_processing_log(log_file, log.str());
      std::cerr << log.str();
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

  // create tiles
  std::vector<volm_tile> tiles;
  if (samples[id()].second.second == "desert" )
    tiles = volm_tile::generate_p1_wr1_tiles();
  else if (samples[id()].second.second == "coast" )
    tiles = volm_tile::generate_p1_wr2_tiles();
  else if (samples[id()].second.second == "Chile" )
    tiles = volm_tile::generate_p1b_wr1_tiles();
  else if (samples[id()].second.second == "India" )
    tiles = volm_tile::generate_p1b_wr2_tiles();
  else if (samples[id()].second.second == "Jordan" )
    tiles = volm_tile::generate_p1b_wr3_tiles();
  else if (samples[id()].second.second == "Philippines" )
    tiles = volm_tile::generate_p1b_wr4_tiles();
  else if (samples[id()].second.second == "Taiwan" )
    tiles = volm_tile::generate_p1b_wr5_tiles();
  else {
    log << "ERROR: unknown world region, should be \" desert, coast, Chile, India, Jordan, Philippines, Taiwan\"\n";
    volm_io::write_post_processing_log(log_file, log.str());  std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // locate which tile contains current locations
  unsigned tile_id = tiles.size();
  for (unsigned i = 0; i < tiles.size(); i++)
    if (tiles[i].bbox_double().contains(samples[id()].first.x(), samples[id()].first.y()))
      tile_id = i;
  std::cout << "GT location " << id() << " ---> " << samples[id()].first.x() << ", " << samples[id()].first.y() << ", is in tile " << tile_id << std::endl;

  // load the geo_locations
  std::stringstream file_name_pre;
  file_name_pre << geo_hypo_folder() << "/geo_index_tile_" << tile_id;
  if (!vul_file::exists(file_name_pre.str() + ".txt")) {
    log << "GT location " << id() << " ---> " << samples[id()].first.x() << ", " << samples[id()].first.y() << " is in tile " << tile_id
        << " but no geo_locations for this tile\n";
    volm_io::write_post_processing_log(log_file, log.str());
    std::cout << log.str();
    return volm_io::POST_PROCESS_HALT;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  if (is_candidate) {
    volm_geo_index::prune_tree(root, cand_poly);
  }
  std::vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);

  // load index parameters
  boxm2_volm_wr3db_index_params params;
  std::stringstream params_file;
  params_file << geo_index_folder() << "/geo_index_tile_" << tile_id << "_index.params";
  if (!params.read_params_file(params_file.str())) {
    log << "ERROR: cannot read params file from " << params_file.str() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  unsigned layer_size = params.layer_size;

  // create volm_query
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);
  // load sph_shell
  if (!vul_file::exists(sph_bin())) {
    log << "ERROR: cannot read spherical shell binary from " << sph_bin() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vsl_b_ifstream sph_ifs(sph_bin());
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container;
  sph_shell->b_read(sph_ifs);
  sph_ifs.close();
  if (sph_shell->get_container_size() != params.layer_size) {
    log << "ERROR: the spherical shell binary (" << sph_shell->get_container_size() << ") has different parameters setting from index ("
        << params.layer_size << ")\n";
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // load camera space
  if (!vul_file::exists(cam_bin())) {
    log << "ERROR: can not find camera_space binary: " << cam_bin() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vsl_b_ifstream cam_ifs(cam_bin());
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(cam_ifs);
  cam_ifs.close();
  // check depth_map_scene binary
  if (!vul_file::exists(dms_bin())) {
    log << "ERROR: can not find depth_map_scene " << dms_bin() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // load query
  volm_query_sptr query = new volm_query(query_bin(), cam_space, dms_bin(), sph_shell, sph);

  // load associate score binary file
  std::stringstream score_file;
  score_file << out() << "ps_1_scores_tile_" << tile_id << ".bin";
  if (!vul_file::exists(score_file.str())) {
    log << "ERROR: can not find score file " << score_file.str() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::vector<volm_score_sptr> scores;
  volm_score::read_scores(scores, score_file.str());

  // get the ground truth score and camera from score binary
  vgl_point_3d<double> gt_loc;
  gt_loc = samples[id()].first;
  double gt_lon, gt_lat;
  gt_lon = gt_loc.x();  gt_lat = gt_loc.y();
  unsigned hyp_gt = 0;
  volm_geo_index_node_sptr leaf_gt = volm_geo_index::get_closest(root, gt_lat, gt_lon, hyp_gt);
  // check the distance from ground trugh location to the closest in geo_index
  vgl_point_3d<double> gt_closest = leaf_gt->hyps_->locs_[hyp_gt];

  double sec_to_meter = 21.0/0.000202;;  // 1 arcsec ~ 30 meter ~ 1/3600 degree
  double x_dist = std::abs(gt_loc.x()-gt_closest.x())*sec_to_meter;
  double y_dist = std::abs(gt_loc.y()-gt_closest.y())*sec_to_meter;
  vgl_vector_2d<double> gt_dist_vec(x_dist, y_dist);
  double gt_dist = sqrt(gt_dist_vec.sqr_length());
  std::cerr << std::setprecision(10) << " GT_location = " << gt_loc
           << ", closest location = " << gt_closest
           << ", distance = " << gt_dist << " meter\n";

  unsigned h_gt, cam_gt_best;
  float score_gt;
  for (auto & score : scores) {
    unsigned li = score->leaf_id_;
    unsigned hi = score->hypo_id_;
    if (leaf_gt->get_string() == leaves[li]->get_string() && hyp_gt == hi) {
      h_gt = hi; score_gt = score->max_score_; cam_gt_best = score->max_cam_id_;
    }
  }
  cam_angles gt_cam_ang = cam_space->camera_angles(cam_gt_best);
  if (!vul_file::exists(img())) {
    log << "ERROR: can not find the test query image: " << img() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vil_image_view<vxl_byte> query_img = vil_load(img().c_str());
  log << " For GT location, leaf is " << leaf_gt->get_string()
      << " hypo_id = " << hyp_gt << '\n'
      << " For GT location, closest hypo_loc " << gt_closest.x() << ", " << gt_closest.y()
      << " distance from closest to GT location is " << gt_dist
      << " has best score = " << score_gt << ", best camera stored in BestCamera.kml ( "
      << gt_cam_ang.get_string() << ", id = " << cam_gt_best << " ) img size " << query_img.ni() << " by " << query_img.nj() << '\n';
  std::cout << log.str();
  volm_io::write_post_processing_log(log_file, log.str());

  // render the index ray-based image
  std::stringstream out_fname_pre;
  out_fname_pre << out() << "/geo_index_tile_" << tile_id;
  // load associate indices
  std::stringstream file_index_name_pre;
  file_index_name_pre << geo_index_folder() << "/geo_index_tile_" << tile_id;
  boxm2_volm_wr3db_index_sptr ind_dst = new boxm2_volm_wr3db_index(layer_size, buffer_capacity());
  boxm2_volm_wr3db_index_sptr ind_ori = new boxm2_volm_wr3db_index(layer_size, buffer_capacity());
  boxm2_volm_wr3db_index_sptr ind_lnd = new boxm2_volm_wr3db_index(layer_size, buffer_capacity());
  ind_dst->initialize_read(leaf_gt->get_index_name(file_index_name_pre.str()));
  ind_ori->initialize_read(leaf_gt->get_label_index_name(file_index_name_pre.str(), "orientation"));
  ind_lnd->initialize_read(leaf_gt->get_label_index_name(file_index_name_pre.str(), "land"));

  unsigned h_id;
  vgl_point_3d<double> h_pt;
  while (leaf_gt->hyps_->get_next(0,1,h_pt)) {
    std::vector<unsigned char> values_dst(layer_size);
    std::vector<unsigned char> values_ori(layer_size);
    std::vector<unsigned char> values_lnd(layer_size);
    h_id = leaf_gt->hyps_->current_-1;
    ind_dst->get_next(values_dst);
    ind_ori->get_next(values_ori);
    ind_lnd->get_next(values_lnd);
    if (h_id == h_gt) {
      std::stringstream dst_img_fname;
      std::stringstream ori_img_fname;
      std::stringstream lnd_img_fname;
      dst_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_dst_best_cam.png";
      ori_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_ori_best_cam.png";
      lnd_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_lnd_best_cam.png";
      vil_image_view<vil_rgb<vxl_byte> > dst_img(query_img.ni(), query_img.nj());
      vil_image_view<vil_rgb<vxl_byte> > ori_img(query_img.ni(), query_img.nj());
      vil_image_view<vil_rgb<vxl_byte> > lnd_img(query_img.ni(), query_img.nj());
      dst_img.fill(vil_rgb<unsigned char>(120,120,120));
      ori_img.fill(vil_rgb<unsigned char>(120,120,120));
      lnd_img.fill(vil_rgb<unsigned char>(120,120,120));
      query->depth_rgb_image(values_dst, cam_gt_best, dst_img, "depth");
      query->depth_rgb_image(values_ori, cam_gt_best, ori_img, "orientation");
      query->depth_rgb_image(values_lnd, cam_gt_best, lnd_img, "land");
      vil_save(dst_img, (dst_img_fname.str()).c_str());
      vil_save(ori_img, (ori_img_fname.str()).c_str());
      vil_save(lnd_img, (lnd_img_fname.str()).c_str());

      if (id() == 46 || id() == 61 || id() == 56) {
        std::stringstream dst_gt_img_fname;
        std::stringstream ori_gt_img_fname;
        std::stringstream lnd_gt_img_fname;
        dst_gt_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_dst_gt_cam.png";
        ori_gt_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_ori_gt_cam.png";
        lnd_gt_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_lnd_gt_cam.png";
        vil_image_view<vil_rgb<vxl_byte> > dst_gt_img(query_img.ni(), query_img.nj());
        vil_image_view<vil_rgb<vxl_byte> > ori_gt_img(query_img.ni(), query_img.nj());
        vil_image_view<vil_rgb<vxl_byte> > lnd_gt_img(query_img.ni(), query_img.nj());
        // initialize the image
        dst_gt_img.fill(vil_rgb<unsigned char>(120,120,120));
        ori_gt_img.fill(vil_rgb<unsigned char>(120,120,120));
        lnd_gt_img.fill(vil_rgb<unsigned char>(120,120,120));
        cam_angles* gt_cam_ang_ptr;
        if (id() == 46)
          gt_cam_ang_ptr = new cam_angles(0, 8.61, 9.075913235, 89.141542);
        else if (id() == 61)
          gt_cam_ang_ptr = new cam_angles(0, 11.1, 11.11, 84.095883);
        else if (id() == 56)
          gt_cam_ang_ptr = new cam_angles(0, 12.6, 10.6377, 97.660);
        std::cout << " ground truth camera is: ";
        gt_cam_ang_ptr->print();
        std::cout << std::endl;
        std::pair<unsigned, cam_angles> gt_cam_pair = cam_space->cam_index_nearest_in_valid_array(*gt_cam_ang_ptr);
        //unsigned gt_cam_id = gt_cam_pair.first;
        unsigned gt_cam_id = (cam_space->valid_indices()[gt_cam_pair.first]);
        query->depth_rgb_image(values_dst, gt_cam_id, dst_gt_img, "depth");
        query->depth_rgb_image(values_ori, gt_cam_id, ori_gt_img, "orientation");
        query->depth_rgb_image(values_lnd, gt_cam_id, lnd_gt_img, "land");
        vil_save(dst_gt_img, (dst_gt_img_fname.str()).c_str());
        vil_save(ori_gt_img, (ori_gt_img_fname.str()).c_str());
        vil_save(lnd_gt_img, (lnd_gt_img_fname.str()).c_str());
      }
    }
  }

  // write out the camera kml
  std::string cam_kml = out() + "/BestCamera.kml";
  std::ofstream ofs_kml(cam_kml.c_str());
  bkml_write::open_document(ofs_kml);

  // write camera
  std::string kml_name = "BestCamera";
  double head = (gt_cam_ang.heading_ < 0) ? gt_cam_ang.heading_ + 360.0 : gt_cam_ang.heading_;
  double tilt = (gt_cam_ang.tilt_ < 0) ? gt_cam_ang.tilt_ + 360 : gt_cam_ang.tilt_;
  double roll;
  if (gt_cam_ang.roll_ * gt_cam_ang.roll_ < 1E-10) roll = 0;
  else                                             roll = gt_cam_ang.roll_;

  double tfov = gt_cam_ang.top_fov_;
  double tv_rad = tfov / vnl_math::deg_per_rad;
  double ttr = std::tan(tv_rad);
  double rfov = std::atan( query_img.ni() * ttr / query_img.nj() ) * vnl_math::deg_per_rad;
  bkml_write::write_photo_overlay(ofs_kml, kml_name, gt_closest.x(), gt_closest.y(), cam_space->altitude(),
                                  head, tilt, roll, tfov, rfov);
  // write location
  bkml_write::write_location(ofs_kml, gt_closest.y(), gt_closest.x(), cam_space->altitude(), "gt_closest");
  bkml_write::close_document(ofs_kml);
  return volm_io::SUCCESS;
}


#if 0
// generate gt hypos
int main(int argc,  char** argv)
{
  vul_arg<std::string> gt_file("-gt_locs", "file with the gt locs of all test cases", "");
  vul_arg<std::string> geo_hypo_folder_pre("-hypo", "folder prefix to read the geo hypotheses", "");
  vul_arg<std::string> geo_index_folder_pre("-geo_pre", "folder prefix to read the geo index and the hypo","");
  vul_arg<unsigned> point_angle("-point", "point_angle use to locate geo index", 0);
  vul_arg<std::string> candidate_list("-cand", "candidate list if exist", "");
  vul_arg<std::string> cam_bin("-cam", "camera space binary", "");                                // query -- camera space binary
  vul_arg<std::string> dms_bin("-dms", "depth_map_scene binary", "");                             // query -- depth map scene
  vul_arg<std::string> sph_bin("-sph", "spherical shell binary", "");                             // query -- spherical_shell binary
  vul_arg<std::string> query_bin("-query", "query binary file", "");
  vul_arg<float>      buffer_capacity("-buff", "index buffer capacity (GB)", 1.0f);
  vul_arg<std::string> out("-out", "job output folder", "");
  vul_arg<unsigned>   id("-id", "id of the test image", 6);
  vul_arg<float>      inc("-inc", "increments in arcseconds, e.g. 0.1 for ~3m increments", 0.35f);
  vul_arg<std::string> img("-img", "query images, to get the image size", "");
  vul_arg<unsigned>   pass_id("-pass", "from pass 0 to pass 1", 1);
  vul_arg_parse(argc, argv);

  std::stringstream log;
  std::string log_file = out() + "/render_img_log.xml";
  if (out().compare("") == 0 ||
      geo_hypo_folder_pre().compare("") == 0 ||
      geo_index_folder_pre().compare("") == 0 ||
      point_angle() == 0 ||
      gt_file().compare("") == 0 ||
      pass_id() > 2 ||
      img().compare("") == 0 ||
      query_bin().compare("") == 0 ||
      id() > 100)
  {
    log << "EXE_ARGUMENT_ERROR!\n";
    vul_arg_display_usage_and_exit();
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

#if 0 // used in commented-out section
  double hypo_interval = 3*(inc()/0.1);
#endif

  // read the gt location for test image id, i.e., lat and lon
  if (!vul_file::exists(gt_file())) {
    log << "ERROR: can not find ground truth position file -->" << gt_file() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::vector<std::pair<vgl_point_3d<double>, std::pair<std::pair<std::string, int>, std::string> > > samples;
  unsigned int cnt = volm_io::read_gt_file(gt_file(), samples);
  if (id() >= cnt) {
    log << "ERROR: the file: " << gt_file() << " does not contain test id: " << id() << "!\n";
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // check whether we have candidate list for this query
  bool is_candidate = false;
  vgl_polygon<double> cand_poly;
  if ( candidate_list().compare("") != 0) {
#ifdef DEBUG
    std::cout << " candidate list = " <<  candidate_list() << std::endl;
#endif
    if ( vul_file::extension(candidate_list()).compare(".txt") == 0) {
      is_candidate = true;
      volm_io::read_polygons(candidate_list(), cand_poly);
    }
    else {
      log << "ERROR: candidate list exist but with wrong format, only txt allowed" << candidate_list() << '\n';
      volm_io::write_post_processing_log(log_file, log.str());
      std::cerr << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
  }

  // create tiles
  std::vector<volm_tile> tiles;
  if (samples[id()].second.second == "desert")
    tiles = volm_tile::generate_p1_wr1_tiles();
  else
    tiles = volm_tile::generate_p1_wr2_tiles();

  // locate which tile contains current locations
  unsigned tile_id = tiles.size();
  for (unsigned i = 0; i < tiles.size(); i++) {
    unsigned u, v;
    if (tiles[i].global_to_img(samples[id()].first.x(), samples[id()].first.y(), u, v) )
      if (u < tiles[i].ni() && v < tiles[i].nj())
        tile_id = i;
  }

  // calculate the utm zone id from [lat,lon]
  int zone_id;
  /*zone_id = 17;*/
  vpgl_utm utm;
  double x, y;
  utm.transform(samples[id()].first.y(), samples[id()].first.x(), x, y, zone_id);
  std::cerr << " for GT location " << id() << " ----> "
           << samples[id()].first.y() << ", "
           << samples[id()].first.x() << ", the zone is "
           << zone_id << " and the utm coord = " << x << ", " << y << '\n';

  std::stringstream geo_hypo_ss;
  geo_hypo_ss << geo_hypo_folder_pre() << zone_id << "_inc_0.99_nh_200/";
  std::string geo_hypo_folder = geo_hypo_ss.str();

  std::stringstream geo_index_ss;
  geo_index_ss << geo_index_folder_pre() << zone_id << "_inc_0.99_nh_200_pa_" << point_angle() << '/';
  std::string geo_index_folder = geo_index_ss.str();

  std::cerr << " geo_hypo_folder = " << geo_hypo_folder << '\n'
           << " geo_index_folder = " << geo_index_folder << '\n';

  // check the existance of index for current tile
  std::stringstream file_name_pre;
  file_name_pre << geo_hypo_folder << "geo_index_tile_" << tile_id;
  if (!vul_file::exists(file_name_pre.str() + ".txt")) {
    log << "for GT location " << id() << " ---> "
        << samples[id()].first.x() << ", "
        << samples[id()].first.y() << " is in the tile "
        << tile_id << " but no geo_index for this tile, stop the post_processing\n";
    volm_io::write_post_processing_log(log_file, log.str());
    std::cout << log.str();
    return volm_io::POST_PROCESS_HALT;
  }
  else {
    log << "for GT location: " << id() << " ---> "
        << samples[id()].first.x() << ", "
        << samples[id()].first.y() << " is in the tile "
        << tile_id << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cout << log.str();
  }

  // load associate index given tile_id, if index exists
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  if (is_candidate) {
    volm_geo_index::prune_tree(root, cand_poly);
  }
  std::vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);

  // create volm_query
  // read in the parameter to create spherical container for query
  boxm2_volm_wr3db_index_params params;
  std::stringstream params_file;
  params_file << geo_index_folder << "geo_index_tile_" << tile_id << "_index.params";
  if (!params.read_params_file(params_file.str())) {
    log << "ERROR: cannot read params file from " << params_file.str() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);
  // load sph_shell
  if (!vul_file::exists(sph_bin())) {
    log << "ERROR: cannot read spherical shell binary from " << sph_bin() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vsl_b_ifstream sph_ifs(sph_bin());
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container;
  sph_shell->b_read(sph_ifs);
  sph_ifs.close();
  if (sph_shell->get_container_size() != params.layer_size) {
    log << "ERROR: the spherical shell binary (" << sph_shell->get_container_size() << ") has different parameters setting from index ("
        << params.layer_size << ")\n";
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  unsigned layer_size = (unsigned)sph_shell->get_container_size();
  // load camera space
  if (!vul_file::exists(cam_bin())) {
    log << "ERROR: can not find camera_space binary: " << cam_bin() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vsl_b_ifstream cam_ifs(cam_bin());
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(cam_ifs);
  cam_ifs.close();
  // check depth_map_scene binary
  if (!vul_file::exists(dms_bin())) {
    log << "ERROR: can not find depth_map_scene " << dms_bin() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // load the query
  //if (!vul_file::exists(query_bin())) {
  //  log << "ERROR: can not find query_binar " << query_bin() << '\n';
  //  volm_io::write_post_processing_log(log_file, log.str());
  //  std::cerr << log.str();
  //  return volm_io::EXE_ARGUMENT_ERROR;
  //}
  volm_query_sptr query = new volm_query(query_bin(), cam_space, dms_bin(), sph_shell, sph);


  // load associate score binary file
  std::stringstream score_file;
  score_file << out() << "ps_1_scores_tile_" << tile_id << ".bin";
  if (!vul_file::exists(score_file.str())) {
    log << "ERROR: can not find score file " << score_file.str() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::vector<volm_score_sptr> scores;
  volm_score::read_scores(scores, score_file.str());

#ifdef DEBUG
    std::cout << " THE READ IN BINRAY SCORE FILE\n"
             << " file name = " << score_file.str() << std::endl;
    for (unsigned i = 0; i < scores.size(); i++) {
      std::cout << scores[i]->leaf_id_ << ' ' << scores[i]->hypo_id_
               << ' ' << scores[i]->max_score_ << ' ' << scores[i]->max_cam_id_ << std::endl;
    }
#endif

  // get the ground truth score and camera from score binary
  vgl_point_3d<double> gt_loc;
  gt_loc = samples[id()].first;
  double gt_lon, gt_lat;
  gt_lon = gt_loc.x();  gt_lat = gt_loc.y();
  unsigned hyp_gt = 0;
  volm_geo_index_node_sptr leaf_gt = volm_geo_index::get_closest(root, gt_lat, gt_lon, hyp_gt);
  // check the distance from ground trugh location to the closest in geo_index
  vgl_point_3d<double> gt_closest = leaf_gt->hyps_->locs_[hyp_gt];

  double sec_to_meter = 21.0/0.000202;;  // 1 arcsec ~ 30 meter ~ 1/3600 degree
  double x_dist = abs(gt_loc.x()-gt_closest.x())*sec_to_meter;
  double y_dist = abs(gt_loc.y()-gt_closest.y())*sec_to_meter;
  vgl_vector_2d<double> gt_dist_vec(x_dist, y_dist);
  double gt_dist = sqrt(gt_dist_vec.sqr_length());
  std::cerr << std::setprecision(10) << " GT_location = " << gt_loc
           << ", closest location = " << gt_closest
           << ", distance = " << gt_dist << " meter\n";
#if 0
  double gt_dist = gt_dist_vec.sqr_length();
  if (gt_dist > min_size) {
    log << "WARNING: the GT location [" << gt_loc.x() << ", " << gt_loc.y() << "] to the closest location ["
        << gt_closest.x() << ", " << gt_closest.y() << "] in geo_index is "
        << gt_dist << ", larger than hypotheses interval "
        << hypo_interval << " meters in geo_index\n";
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
  }
#endif

#if 1

  unsigned h_gt, cam_gt_best;
  float score_gt;
  for (unsigned i = 0; i < scores.size(); i++) {
    unsigned li = scores[i]->leaf_id_;
    unsigned hi = scores[i]->hypo_id_;
    if (leaf_gt->get_string() == leaves[li]->get_string() && hyp_gt == hi) {
      h_gt = hi; score_gt = scores[i]->max_score_; cam_gt_best = scores[i]->max_cam_id_;
    }
  }
  cam_angles gt_cam_ang = cam_space->camera_angles(cam_gt_best);
  if (!vul_file::exists(img())) {
    log << "ERROR: can not find the test query image: " << img() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vil_image_view<vxl_byte> query_img = vil_load(img().c_str());
  log << " For GT location, leaf is " << leaf_gt->get_string()
      << " hypo_id = " << hyp_gt << '\n'
      << " For GT location, closest hypo_loc " << gt_closest.x() << ", " << gt_closest.y()
      << " distance from closest to GT location is " << gt_dist
      << " has best score = " << score_gt << ", best camera stored in BestCamera.kml ( "
      << gt_cam_ang.get_string() << ", id = " << cam_gt_best << " ) img size " << query_img.ni() << " by " << query_img.nj() << '\n';
  std::cout << log.str();
  volm_io::write_post_processing_log(log_file, log.str());


  // render the index_depth image
  std::stringstream out_fname_pre;
  out_fname_pre << out() << "/geo_index_tile_" << tile_id;

  std::stringstream file_index_name_pre;
  file_index_name_pre << geo_index_folder << "geo_index_tile_" << tile_id;
  boxm2_volm_wr3db_index_sptr ind_dst = new boxm2_volm_wr3db_index(layer_size, buffer_capacity());
  boxm2_volm_wr3db_index_sptr ind_combine = new boxm2_volm_wr3db_index(layer_size, buffer_capacity());
  ind_dst->initialize_read(leaf_gt->get_index_name(file_index_name_pre.str()));
  ind_combine->initialize_read(leaf_gt->get_label_index_name(file_index_name_pre.str(),"combined"));

  unsigned h_id;
  vgl_point_3d<double> h_pt;
  while (leaf_gt->hyps_->get_next(0,1,h_pt)) {
    std::vector<unsigned char> values_dst(layer_size);
    std::vector<unsigned char> values_combine(layer_size);
    h_id = leaf_gt->hyps_->current_-1;
    ind_dst->get_next(values_dst);
    ind_combine->get_next(values_combine);
    if (h_id == h_gt) {

      // extract the values from combined index to orient and hypo
      std::vector<unsigned char> values_ori;
      std::vector<unsigned char> values_lnd;
      std::vector<unsigned char> test_ori;
      std::vector<unsigned char> test_lnd;
      for (unsigned ii = 0; ii < values_combine.size(); ii++) {
        unsigned char ind_ori, ind_lnd;
        if (values_combine[ii] != 254)
          volm_io_extract_values(values_combine[ii], ind_ori, ind_lnd);
        else {
          ind_ori = 254;
          ind_lnd = 254;
        }
        values_ori.push_back(ind_ori);
        values_lnd.push_back(ind_lnd);
      }

#if 0
      if (id() == 40) {
        std::cout << "id = " << id() << ", h_id = " << h_id << ", h_gt = " << h_gt << ", point = " << h_pt << std::endl;
        for (unsigned ii = 0; ii < values_combine.size(); ii++) {
          std::cout << " ii = " << ii << ", combine = " << (int)values_combine[ii]
                   << " ---> orient = " << (int)values_ori[ii] << " test_ori = " << (int)test_ori[ii]
                   << ", land_id = " << (int)values_lnd[ii] << " test_lnd = " << (int)test_lnd[ii]
                   << std::endl;
        }
      }
#endif

      std::stringstream dst_img_fname;
      std::stringstream ori_img_fname;
      std::stringstream lnd_img_fname;
      dst_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_depth_best_cam.png";
      ori_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_orient_best_cam.png";
      lnd_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_land_best_cam.png";

      vil_image_view<vil_rgb<vxl_byte> > dst_img(query_img.ni(), query_img.nj());
      vil_image_view<vil_rgb<vxl_byte> > ori_img(query_img.ni(), query_img.nj());
      vil_image_view<vil_rgb<vxl_byte> > lnd_img(query_img.ni(), query_img.nj());

      // initialize the image
      for (unsigned i = 0; i < query_img.ni(); i++)
        for (unsigned j = 0; j < query_img.nj(); j++) {
          dst_img(i,j).r = (unsigned char)120;  dst_img(i,j).g = (unsigned char)120;  dst_img(i,j).b = (unsigned char)120;
          ori_img(i,j).r = (unsigned char)120;  ori_img(i,j).g = (unsigned char)120;  ori_img(i,j).b = (unsigned char)120;
          lnd_img(i,j).r = (unsigned char)120;  lnd_img(i,j).g = (unsigned char)120;  lnd_img(i,j).b = (unsigned char)120;
        }
#ifdef DEBUG
      std::cout << " cam_id = " << cam_gt_best << std::endl;
#endif
      query->depth_rgb_image(values_dst, cam_gt_best, dst_img, "depth");
      query->depth_rgb_image(values_ori, cam_gt_best, ori_img, "orientation");
      query->depth_rgb_image(values_lnd, cam_gt_best, lnd_img, "land");
      vil_save(dst_img, (dst_img_fname.str()).c_str());
      vil_save(ori_img, (ori_img_fname.str()).c_str());
      vil_save(lnd_img, (lnd_img_fname.str()).c_str());

      // hack here to render the gt camera for test_id_36
      //std::pair<unsigned, cam_angles> gt_cam_pair;
      unsigned gt_cam_id;
      if (id() == 36) {
        cam_angles gt_cam_ang(0.31, 29.03, 68.0, 77.13);
        std::pair<unsigned, cam_angles> gt_cam_pair = cam_space->cam_index_nearest_in_valid_array(gt_cam_ang);
        gt_cam_id = gt_cam_pair.first;
        log << " For GT location , closest hypo_loc " << gt_closest.x() << ", " << gt_closest.y()
            << " GT camera " << gt_cam_pair.first << " --> " << gt_cam_pair.second.get_string() << '\n';
        volm_io::write_post_processing_log(log_file, log.str());
        std::cout << log.str();
      }
      else if (id() == 40) {
        cam_angles gt_cam_ang(-0.74, 5.30, 67.0, 87.70);
        std::pair<unsigned, cam_angles> gt_cam_pair = cam_space->cam_index_nearest_in_valid_array(gt_cam_ang);
        gt_cam_id = gt_cam_pair.first;
        log << " For GT location , closest hypo_loc " << gt_closest.x() << ", " << gt_closest.y()
            << " GT camera " << gt_cam_pair.first << " --> " << gt_cam_pair.second.get_string() << '\n';
        volm_io::write_post_processing_log(log_file, log.str());
        std::cout << log.str();
      }
      if (id() == 36 || id() == 40) {
        std::stringstream dst_gt_img_fname;
        std::stringstream ori_gt_img_fname;
        std::stringstream lnd_gt_img_fname;
        dst_gt_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_depth_gt_cam.png";
        ori_gt_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_orient_gt_cam.png";
        lnd_gt_img_fname << out_fname_pre.str() << "_ps_" << pass_id() << "_ind_lnd_gt_cam.png";
        vil_image_view<vil_rgb<vxl_byte> > dst_gt_img(query_img.ni(), query_img.nj());
        vil_image_view<vil_rgb<vxl_byte> > ori_gt_img(query_img.ni(), query_img.nj());
        vil_image_view<vil_rgb<vxl_byte> > lnd_gt_img(query_img.ni(), query_img.nj());
        // initialize the image
        for (unsigned i = 0; i < query_img.ni(); i++)
          for (unsigned j = 0; j < query_img.nj(); j++) {
            dst_gt_img(i,j).r = (unsigned char)120;  dst_gt_img(i,j).g = (unsigned char)120;  dst_gt_img(i,j).b = (unsigned char)120;
            ori_gt_img(i,j).r = (unsigned char)120;  ori_gt_img(i,j).g = (unsigned char)120;  ori_gt_img(i,j).b = (unsigned char)120;
            lnd_gt_img(i,j).r = (unsigned char)120;  lnd_gt_img(i,j).g = (unsigned char)120;  lnd_gt_img(i,j).b = (unsigned char)120;
          }
        query->depth_rgb_image(values_dst, gt_cam_id, dst_gt_img, "depth");
        query->depth_rgb_image(values_ori, gt_cam_id, ori_gt_img, "orientation");
        query->depth_rgb_image(values_lnd, gt_cam_id, lnd_gt_img, "land");
        vil_save(dst_gt_img, (dst_gt_img_fname.str()).c_str());
        vil_save(ori_gt_img, (ori_gt_img_fname.str()).c_str());
        vil_save(lnd_gt_img, (lnd_gt_img_fname.str()).c_str());
      }
    }
  }

  // write out the camera.kml
  std::string cam_kml = out() + "/BestCamera.kml";
  std::ofstream ofs_kml(cam_kml.c_str());

  std::stringstream kml_name;
  kml_name << "p1a_test1_" << id();
  bkml_write::open_document(ofs_kml);

  double head = (gt_cam_ang.heading_ < 0) ? gt_cam_ang.heading_ + 360.0 : gt_cam_ang.heading_;
  double tilt = (gt_cam_ang.tilt_ < 0) ? gt_cam_ang.tilt_ + 360 : gt_cam_ang.tilt_;
  double roll;
  if (gt_cam_ang.roll_ * gt_cam_ang.roll_ < 1E-10) roll = 0;
  else                                             roll = gt_cam_ang.roll_;

  double tfov = gt_cam_ang.top_fov_;
  double tv_rad = tfov / vnl_math::deg_per_rad;
  double ttr = std::tan(tv_rad);
  double rfov = std::atan( query_img.ni() * ttr / query_img.nj() ) * vnl_math::deg_per_rad;

  bkml_write::write_photo_overlay(ofs_kml, kml_name.str(), gt_closest.x(), gt_closest.y(), cam_space->altitude(),
                                  head, tilt, roll, tfov, rfov);
  bkml_write::close_document(ofs_kml);
  ofs_kml.close();

#if 0
  bkml_parser* parser = new bkml_parser();
  std::FILE* xmlFile = std::fopen(cam_kml.c_str(), "r");
  if (!xmlFile) {
    std::cerr << cam_kml.c_str() << " error on opening\n";
    delete parser;
    return false;
  }
  if (!parser->parseFile(xmlFile)) {
    std::cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';

    delete parser;
    return false;
  }
  std::cout << "test reading Bestcamera.kml -------\n"
           << "\t\t lon = " << parser->longitude_ << ", lat = " << parser->latitude_ << ", alt = " << parser->altitude_
           << ", head = " << parser->heading_ << ", tilt = " << parser->tilt_ << ", roll = " << parser->roll_
           << ", tfov = " << parser->top_fov_ << ", rfov = " << parser->right_fov_ << std::endl;
#endif

#endif
  return volm_io::SUCCESS;
}
#endif
