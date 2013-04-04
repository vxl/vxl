//:
// \file
// \executable to generate depth_map_scene, camera_space and volm_query for volm_matcher
// \author Yi Dong
// \date Apr 03, 2013

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vcl_set.h>
#include <vcl_ios.h>


inline float minimum(const float& v1, const float& v2)
{
  return (v1 > v2) ? v2 : v1;
}


int main(int argc, char** argv)
{
  vul_arg<vcl_string> cam_bin("-cam", "camera space binary", "");
  vul_arg<vcl_string> label_xml("-label", "labelme kml file", "");
  vul_arg<vcl_string> category_file("-cat", "category file for transferring labled type to land id", "");
  vul_arg<vcl_string> geo_hypo_folder_a("-hypoa", "hypo folder for utm zone 17", "");
  vul_arg<vcl_string> geo_hypo_folder_b("-hypob", "hypo folder for utm zone 18", "");
  vul_arg<vcl_string> out("-out", "job output folder", "");
  vul_arg<float> kl ("-kl", "parameter for nonlinear score scaling", 200.0f);
  vul_arg<float> ku ("-ku", "parameter for nonlinear score scaling", 10.0f);
  vul_arg<unsigned> job_id ("-job", "query image job_id", 0);
  vul_arg<bool> logger("-logger", "option to write status.xml", false);
  vul_arg_parse(argc, argv);

  // check input parameters
  vcl_stringstream log;
  if (out().compare("") == 0 || cam_bin().compare("") == 0 ||
      label_xml().compare("") == 0 || category_file().compare("") == 0 ||
      geo_hypo_folder_a().compare("") == 0 ||
      geo_hypo_folder_b().compare("") == 0) {
      log << "EXE_ARGUMENT_ERROR!\n";
      vcl_cerr << log.str();
      volm_io::write_composer_log(out(), log.str());
      volm_io::write_status(out(), volm_io::EXE_ARGUMENT_ERROR, 100);
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
  }

  // load depth map scene
  if (!vul_file::exists(label_xml()) || vul_file::exists(category_file())) {
    log << "EXE_ARGUMENT_ERROR!\n";  vcl_cerr << log.str();
    volm_io::write_composer_log(out(), log.str());
    volm_io::write_status(out(), volm_io::EXE_ARGUMENT_ERROR, 100);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_xml(), category_file(), dm, img_category)) {
    log << "EXE_ARGUMENT_ERROR!\n";  vcl_cerr << log.str();
    volm_io::write_composer_log(out(), log.str());
    volm_io::write_status(out(), volm_io::LABELME_FILE_IO_ERROR, 100);
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  if (img_category == "desert") {
    log << "ERROR: current matcher works only on coast\n";  vcl_cerr << log.str();
    volm_io::write_composer_log(out(), log.str());
    volm_io::write_status(out(), volm_io::LABELME_FILE_IO_ERROR, 100);
    return volm_io::LABELME_FILE_IO_ERROR;
  }

  // create prob map
  vcl_vector<volm_tile> tiles;
  tiles = volm_tile::generate_p1_wr2_tiles();

  // initialize the prob_map
  vcl_vector<vil_image_view<float> > tile_imgs;
  for (unsigned i = 0; i < tiles.size(); i++) {
    vil_image_view<float> img(3601,3601);
    img.fill(-1.0f);
    tile_imgs.push_back(img);
  }


  // read the score and create top 30 list
  for (unsigned tile_id = 0; tile_id < tiles.size(); tile_id++) {
    volm_tile tile = tiles[tile_id];
    unsigned zone_id;
    vcl_string geo_hypo_folder;
    if (tile_id < 8 && tile_id != 5) {
      zone_id = 17;  geo_hypo_folder = geo_hypo_folder_a();
    } else {
      zone_id = 18;  geo_hypo_folder = geo_hypo_folder_b();
    }
    // load associate geo_hypo
    vcl_stringstream file_name_pre;
    file_name_pre << geo_hypo_folder << "/geo_index_tile_" << tile_id;
    float min_size;
    volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
    volm_geo_index::read_hyps(root, file_name_pre.str());
    vcl_vector<volm_geo_index_node_sptr> leaves;
    volm_geo_index::get_leaves_with_hyps(root, leaves);

    // load the score binary
    vcl_stringstream score_file;
    score_file << out() << "ps_1_scores_zone_" << zone_id << "_tile_" << tile_id << ".bin";
    if (!vul_file::exists(score_file.str()))
      continue;
    vcl_vector<volm_score_sptr> scores;
    volm_score::read_scores(scores, score_file.str());
    // fill the image
    unsigned total_ind = scores.size();
    for (unsigned ii = 0; ii < total_ind; ii++) {
      vgl_point_3d<double> h_pt = leaves[scores[ii]->leaf_id_]->hyps_->locs_[scores[ii]->hypo_id_];
      unsigned u, v;
      if (tile.global_to_img(h_pt.x(), h_pt.y(), u, v)) {
        if (u < tile.ni() && v < tile.nj()) {
          if (scores[ii]->max_score_ > tile_imgs[tile_id](u,v))
            tile_imgs[tile_id](u,v) = scores[ii]->max_score_;
        }
      }
    }
  } // end of the loop over all tiles

  // save the probmap
  for (unsigned i = 0; i < tiles.size(); i++) {
    vcl_string img_name = out() + "/ProbMap_" + tiles[i].get_string() + ".tif";
    vil_save(tile_imgs[i], img_name.c_str());
  }

  // create top 30 list
  vcl_map<unsigned, vcl_vector<float> > max_scores;
  vcl_map<unsigned, vcl_vector<vgl_point_3d<double> > > max_locs;
  vcl_map<unsigned, vcl_vector<unsigned> > max_cameras;
  // search for the top 30 scores
  for (unsigned i = 0; i < tiles.size(); i++) {
    vil_image_view<float> tile_img = tile_imgs[i];
    // use stl set temporary
    vcl_set<float> score_set;
    for (unsigned u = 0; u < tile_img.ni(); u++) {
      for (unsigned v = 0; v < tile_img.nj(); v++) {
        if (tile_img(u,v) > 0)
          score_set.insert(tile_img(u,v));
      }
    }
    vcl_vector<float> max_score;
    vcl_set<float>::iterator sit = score_set.end();
    --sit;  max_score.push_back(*sit);
    --sit;  max_score.push_back(*sit);
    --sit;  max_score.push_back(*sit);
    vcl_pair<unsigned, vcl_vector<float> > pair;
    pair.first = i;
    pair.second = max_score;
    max_scores.insert(pair);
  }
  // locate top 30 locations and cameras
  for (unsigned tile_id = 0; tile_id < tiles.size(); tile_id++) {
    vcl_vector<vgl_point_3d<double> > loc_vec;
    vcl_vector<unsigned> cam_vec;
    loc_vec.resize(max_scores[tile_id].size());
    cam_vec.resize(max_scores[tile_id].size());
    unsigned zone_id;
    vcl_string geo_hypo_folder;
    if (tile_id < 8 && tile_id != 5) {
      zone_id = 17;  geo_hypo_folder = geo_hypo_folder_a();
    } else {
      zone_id = 18;  geo_hypo_folder = geo_hypo_folder_b();
    }
    // load associate geo_hypo
    vcl_stringstream file_name_pre;
    file_name_pre << geo_hypo_folder << "/geo_index_tile_" << tile_id;
    float min_size;
    volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
    volm_geo_index::read_hyps(root, file_name_pre.str());
    vcl_vector<volm_geo_index_node_sptr> leaves;
    volm_geo_index::get_leaves_with_hyps(root, leaves);
    // load the score binary
    vcl_stringstream score_file;
    score_file << out() << "ps_1_scores_zone_" << zone_id << "_tile_" << tile_id << ".bin";
    if (!vul_file::exists(score_file.str()))
      continue;
    vcl_vector<volm_score_sptr> scores;
    volm_score::read_scores(scores, score_file.str());
    unsigned total_ind = scores.size();
    for (unsigned ii = 0; ii < total_ind; ii++) {
      float score = scores[ii]->max_score_;
      if (score == max_scores[tile_id][0]) {
        loc_vec[0] = leaves[scores[ii]->leaf_id_]->hyps_->locs_[scores[ii]->hypo_id_];
        cam_vec[0] = scores[ii]->max_cam_id_;
      }
      else if (score == max_scores[tile_id][1]) {
        loc_vec[1] = leaves[scores[ii]->leaf_id_]->hyps_->locs_[scores[ii]->hypo_id_];
        cam_vec[1] = scores[ii]->max_cam_id_;
      }
      else if (score == max_scores[tile_id][2]) {
        loc_vec[2] = leaves[scores[ii]->leaf_id_]->hyps_->locs_[scores[ii]->hypo_id_];
        cam_vec[2] = scores[ii]->max_cam_id_;
      }
    }
    vcl_pair<unsigned, vcl_vector<vgl_point_3d<double> > > loc_pair;
    vcl_pair<unsigned, vcl_vector<unsigned> > cam_pair;
    loc_pair.first = tile_id;
    loc_pair.second = loc_vec;
    cam_pair.first = tile_id;
    cam_pair.second = cam_vec;
    max_locs.insert(loc_pair);
    max_cameras.insert(cam_pair);
  }
#if 0
  for (unsigned i = 0; i < tiles.size(); i++) {
    vcl_vector<vgl_point_2d<unsigned> > loc_pixel;
    loc_pixel.resize(3);
    volm_tile tile = tiles[i];
    vil_image_view<float> tile_img = tile_imgs[i];
    // find the pixel that gives maximum scores
    for (unsigned u = 0; u < tile_img.ni(); u++) {
      for (unsigned v = 0; v < tile_img.nj(); v++) {
        if (tile_img(u,v) == max_scores[i][0])
          loc_pixel[0] = vgl_point_2d<unsigned>(u,v);
        else if (tile_img(u,v) == max_scores[i][1])
          loc_pixel[1] = vgl_point_2d<unsigned>(u,v);
        else if (tile_img(u,v) == max_scores[i][2])
          loc_pixel[2] = vgl_point_2d<unsigned>(u,v);
      }
    }
    // transfer img pixel to lon and lat
    vcl_vector<vgl_point_2d<double> > locs_vec;
    for (unsigned id = 0; id < loc_pixel.size(); id++) {
      double lon, lat;
      tile.img_to_global(loc_pixel[id].x(), loc_pixel[id].y(), lon, lat);
      locs_vec.push_back(vgl_point_2d<double>(lon, lat));
    }
    vcl_pair<unsigned, vcl_vector<vgl_point_2d<double> > > pair;
    pair.first = i;
    pair.second = locs_vec;
    max_locs.insert(pair);
  }
#endif

  // calculate roi based on maximum score
  float max_score_all = 0;
  vcl_map<unsigned, vcl_vector<float> >::iterator mit = max_scores.begin();
  for(; mit != max_scores.end(); ++mit) {
    for (vcl_vector<float>::iterator vit = mit->second.begin(); vit != mit->second.end(); ++vit)
      if (max_score_all < *vit) max_score_all = *vit;
  }
  vcl_vector<double> thresholds;
  float ds = 0.1f * max_score_all;
  for (int i = -3; i < 0; i++)
    thresholds.push_back(max_score_all + ds*i);
  thresholds.push_back(0.95*max_score_all);
  thresholds.push_back(max_score_all);
  //  cnt_map -- key is the thresholds, element --- cnt_below, total pixel count, total pixel uncount
  vcl_map<float, vcl_vector<unsigned> > cnt_map;
  for (vcl_vector<double>::iterator vit = thresholds.begin(); vit != thresholds.end(); ++vit) {
      vcl_vector<unsigned> cnt_vec(3,0);
      vcl_pair<float, vcl_vector<unsigned> > cnt_pair;
      cnt_pair.first = *vit;  cnt_pair.second = cnt_vec;
      cnt_map.insert(cnt_pair);
  }
  for (unsigned i = 0; i < tiles.size(); ++i) {
    vil_image_view<float> tile_img = tile_imgs[i];
    for (unsigned u = 0; u < tile_img.ni(); u++) {
      for (unsigned v = 0; v < tile_img.nj(); v++) {
        // loop over all threshold
        for ( vcl_map<float, vcl_vector<unsigned> >::iterator mit = cnt_map.begin(); mit != cnt_map.end(); ++mit)
        {
          if (tile_img(u, v) < 0)
            mit->second[2]++;
          else {
            mit->second[1]++;
            if (tile_img(u, v) < mit->first)
              mit->second[0]++;
          }
        }
      }
    }
  }

  vcl_vector<double> score_roi;
  score_roi.push_back(0.0f);
  for (vcl_map<float, vcl_vector<unsigned> >::iterator mit = cnt_map.begin();
         mit != cnt_map.end(); ++mit)
  {
    double roi = 1.0 - (double)mit->second[0]/mit->second[1];
    score_roi.push_back(roi);
  }
  score_roi.push_back((float)cnt_map.begin()->second[1]);

  // create folder for different thresholds,
  vcl_vector<vcl_string> prob_thres_folders;
  for (vcl_vector<double>::iterator vit = thresholds.begin(); vit != thresholds.end(); ++vit) {
    vcl_stringstream folder_name;
    float pre = *vit/max_score_all;
    folder_name << out() << "/ProbMap_scaled_" << *vit;
    vul_file::make_directory(folder_name.str());
    prob_thres_folders.push_back(folder_name.str());
  }
  // create png tile images for different thresholds, only generate png tile prob_map with thres smaller than ground truth score
  for (unsigned ti = 0; ti < tiles.size(); ++ti) {
    vil_image_view<float> tile_img = tile_imgs[ti];
    unsigned cnt = 0;
    for (vcl_vector<double>::iterator vit = thresholds.begin(); vit != thresholds.end(); ++vit) {
      vil_image_view<vxl_byte> out_png(tile_img.ni(), tile_img.nj());
      out_png.fill(volm_io::UNKNOWN);
      // loop over current tile image to rescale the score to [0, 255]
      for (unsigned ii = 0; ii < tile_img.ni(); ++ii) {
        for (unsigned jj = 0; jj< tile_img.nj(); ++jj) {
          if (tile_img(ii, jj) > 0)
            out_png(ii, jj) = volm_io::scale_score_to_1_255_sig(kl(), ku(), *vit, tile_img(ii,jj));
            //out_png(ii, jj) = volm_io::scale_score_to_1_255(*vit, tile_img(ii,jj));
        }
      }
      // save the image
      vcl_string out_png_name = prob_thres_folders[cnt++] + "/" + "ProbMap_" + tiles[ti].get_string() + ".tif";
      vil_save(out_png, out_png_name.c_str());
    }
  }
  vcl_string eoi_file = out() + "/roi_result_max_score.txt";
  vcl_ofstream fout(eoi_file.c_str());
  fout << "  test_id      gt_loc_score      total_locs                                       thresholds\n"
       << "----------------------------------------------------------------------------------------------------------------------------\n";
  fout.setf(vcl_ios_right);
  for (vcl_vector<double>::iterator vit = thresholds.begin(); vit != thresholds.end(); ++vit) {
    fout.precision(6); fout.width(13); fout.fill(' '); 
    fout << *vit;
  }
  fout << '\n';
  vcl_stringstream out_str;
  out_str << "job_" << job_id();
  fout << out_str.str();
  fout.precision(4); fout.width(13); fout.fill(' ');
  fout << score_roi[0] << ' ';
  unsigned tot_loc = (unsigned)score_roi[score_roi.size()-1];
  fout.width(13); fout.fill(' ');
  fout << tot_loc << "        ";
  for (unsigned i = 1; i < thresholds.size()+1; i++) {
    fout.setf(vcl_ios_right);
    fout.precision(6); fout.width(13); fout.fill(' ');
    fout << score_roi[i];
  }
  fout << "\n----------------------------------------------------------------------------------------------------------------------------\n";
  fout.close();

  // generate top 30 BestCamera.kml
  
  // screen output here ...
  mit = max_scores.begin();
  vcl_map<unsigned, vcl_vector<vgl_point_3d<double> > >::iterator mit_locs = max_locs.begin();
  vcl_map<unsigned, vcl_vector<unsigned> >::iterator mit_cams = max_cameras.begin();
  for (; mit != max_scores.end(); ++mit) {
    vcl_cout << " for tile " << mit->first << '(' << mit_locs->first << ")\n";
    vcl_cout << "\t 1st max = " << mit->second[0] << " --- cam = " << mit_cams->second[0] << " --- locs = " << mit_locs->second[0] << '\n';
    vcl_cout << "\t 2nd max = " << mit->second[1] << " --- cam = " << mit_cams->second[1] << " --- locs = " << mit_locs->second[1] << '\n';
    vcl_cout << "\t 3rd max = " << mit->second[2] << " --- cam = " << mit_cams->second[2] << " --- locs = " << mit_locs->second[2] << vcl_endl;
    ++mit_locs;
    ++mit_cams;
  }

  volm_io::write_status(out(), volm_io::SUCCESS,100);
  return volm_io::SUCCESS;

}

#if 0
int main(int argc, char** argv)
{
  vul_arg<vcl_string> cam_kml("-cam", "camera space binary", "");
  vul_arg<vcl_string> params_file("-params", "camera incremental param and parameters for depth interval", "");
  vul_arg<vcl_string> label_xml("-label", "labelme kml file", "");
  vul_arg<vcl_string> category_file("-cat", "category file for transferring labled type to land id", "");
  vul_arg<vcl_string> sph_bin("-sph", "spherical shell binary file", "");
  vul_arg<vcl_string> out_folder("-out", "output folder where the generate query binary, camspace binary and weight parameter stored", "");
  vul_arg<unsigned> id("-id", "job id", 100);
  vul_arg_parse(argc, argv);

  if (cam_kml().compare("") == 0 || params_file().compare("") == 0 ||
      label_xml().compare("") == 0 || category_file().compare("") == 0 ||
      sph_bin().compare("") == 0 || out_folder().compare("") == 0)
  {
    vcl_cerr << " ERROR: input file/folders can not be empty!\n";
    volm_io::write_status(out_folder(), volm_io::PRE_PROCESS_FAILED, 0);
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  
  // create depth_map_scene from label me file
  if (!vul_file::exists(label_xml()) || !vul_file::exists(category_file())) {
    vcl_cerr << "problem opening labelme xml file or category file --> " << label_xml() << vcl_endl;
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR, 0);
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_xml(), category_file(), dm, img_category) ) {
    vcl_cerr << "problem parsing labelme xml file --> " << label_xml() << vcl_endl;
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR, 0);
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // save depth_map_scene as a binary
  vcl_string dms_bin_file = out_folder() + "/depth_map_scene.bin";
  vsl_b_ofstream ofs_dms(dms_bin_file);
  dm->b_write(ofs_dms);
  ofs_dms.close();

  // read the params
  if (!vul_file::exists(params_file())) {
    vcl_cerr << "problem opening camera incremental file --> " << params_file() << '\n';
    volm_io::write_status(out_folder(), volm_io::PRE_PROCESS_FAILED, 0);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_io_expt_params params;
  params.read_params(params_file());

  // create camera space and save it as binary for matcher
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double tfov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_kml(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, tfov, top_fov_dev, altitude, lat, lon)) {
    vcl_cerr << "problem parsing camera kml file --> " << cam_kml() << '\n';
    volm_io::write_status(out_folder(), volm_io::CAM_FILE_IO_ERROR, 0);
    return volm_io::CAM_FILE_IO_ERROR;
  }
  vcl_cout << " create camera space from " << cam_kml() << vcl_endl;
  if ( vcl_abs(heading-0) < 1E-10) heading = 180.0;
  vcl_cout << "cam params:"
           << "\n head: " << heading << " dev: " << heading_dev
           << "\n tilt: " << tilt << " dev: " << tilt_dev << " inc: " << params.head_inc
           << "\n roll: " << roll << " dev: " << roll_dev << " inc: " << params.roll_inc
           << "\n  fov: " << tfov << " dev: " << top_fov_dev << " inc: " << params.fov_inc
           << "\n  alt: " << altitude << vcl_endl;

  // construct camera space
  volm_camera_space_sptr cam_space = new volm_camera_space(tfov, top_fov_dev, params.fov_inc, altitude, dm->ni(), dm->nj(),
                                                           heading, heading_dev, params.head_inc,
                                                           tilt, tilt_dev, params.tilt_inc,
                                                           roll, roll_dev, params.roll_inc);

  if (dm->ground_plane().size() > 0)  // enforce ground plane constraint if user specified a ground plane
  {
    camera_space_iterator cit = cam_space->begin();
    for ( ; cit != cam_space->end(); ++cit) {
      unsigned current = cam_space->cam_index();
      vpgl_perspective_camera<double> cam = cam_space->camera(); // camera at current state of iterator
      bool success = true;
      for (unsigned i = 0; success && i < dm->ground_plane().size(); i++)
        success = dm->ground_plane()[i]->region_ground_2d_to_3d(cam);
      if (success) // add this camera
        cam_space->add_camera_index(current);
    }
  }
  else
    cam_space->generate_full_camera_index_space();

  //cam_space.print_valid_cams();
  vcl_string cam_bin_file = out_folder() + "/camera_space.bin";
  vsl_b_ofstream ofs_cam(cam_bin_file);
  cam_space->b_write(ofs_cam);
  ofs_cam.close();

  // create depth interval
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle,params.vmin,params.dmax);

  // load the spherical shell container
  if (!vul_file::exists(sph_bin())) {
    vcl_cerr << " ERROR: can not find spherical shell binary --> " << sph_bin() << vcl_endl;
    volm_io::write_status(out_folder(), volm_io::PRE_PROCESS_FAILED);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container();
  vsl_b_ifstream ifs_sph(sph_bin());
  sph_shell->b_read(ifs_sph);
  ifs_sph.close();

  // create volm_query
  volm_query_sptr query = new volm_query(cam_space, dms_bin_file, sph_shell, sph);

  // save the volm_query 
  vcl_string query_bin_file = out_folder() + "/volm_query.bin";
  vsl_b_ofstream ofs(query_bin_file);
  query->write_data(ofs);
  ofs.close();

  // screen output
  // sky
  depth_map_scene_sptr dmq = query->depth_scene();
  vcl_cout << " The " << dmq->ni() << " x " << dmq->nj() << " query image has following defined depth region" << vcl_endl;
  if (!dmq->sky().empty()) {
    vcl_cout << " -------------- SKYs --------------" << vcl_endl;
    for (unsigned i = 0; i < dmq->sky().size(); i++) {
      vcl_cout << "\t name = " << (dmq->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << (int)query->sky_orient()
               << ", land_id = " << dmq->sky()[i]->land_id()
               << ", land_name = " << volm_label_table::land_string(dmq->sky()[i]->land_id())
               << ", land_fallback_category = ";
      volm_fallback_label::print_id(dmq->sky()[i]->land_id());
      vcl_cout << ", land_fallback_weight = " ;
      volm_fallback_label::print_wgt(dmq->sky()[i]->land_id());
      vcl_cout << vcl_endl;
    }
  }

  // ground
  if (!dmq->ground_plane().empty()) {
    vcl_cout << " -------------- GROUND PLANE --------------" << vcl_endl;
    for (unsigned i = 0; i < dmq->ground_plane().size(); i++) {
      vcl_cout << "\t name = " << dmq->ground_plane()[i]->name()
                << ", depth = " << dmq->ground_plane()[i]->min_depth()
                << ", orient = " << dmq->ground_plane()[i]->orient_type()
                << ", land_id = " << dmq->ground_plane()[i]->land_id()
                << ", land_name = " << volm_label_table::land_string(dmq->ground_plane()[i]->land_id())
                << ", land_fallback = ";
      volm_fallback_label::print_id(dmq->ground_plane()[i]->land_id());
      vcl_cout << ", land_fallback_wgt = ";
      volm_fallback_label::print_wgt(dmq->ground_plane()[i]->land_id());
      vcl_cout << vcl_endl;
    }
  }

  vcl_vector<depth_map_region_sptr> drs = query->depth_regions();
  vcl_vector<vcl_vector<unsigned char> >& obj_land = query->obj_land_id();
  vcl_vector<vcl_vector<float> >& obj_land_wgt = query->obj_land_wgt();
  if (!drs.empty()) {
    vcl_cout << " -------------- NON GROUND/SKY OBJECTS --------------" << vcl_endl;
    for (unsigned i = 0; i < drs.size(); i++) {
      vcl_cout << "\t " <<  drs[i]->name()
               << " region,\t min_depth = " << drs[i]->min_depth()
               << ",\t max_depth = " << drs[i]->max_depth()
               << ",\t order = " << drs[i]->order()
               << ",\t orient = " << drs[i]->orient_type()
               << ",\t land_id = " << drs[i]->land_id()
               << ",\t land_name = " << volm_label_table::land_string( drs[i]->land_id() )
               << ",\t fallback_category = ";
      volm_fallback_label::print_id(drs[i]->land_id());
      vcl_cout << " (";
      for (unsigned jj =0; jj < obj_land[i].size(); jj++)
        vcl_cout << volm_label_table::land_string(obj_land[i][jj]) << ", ";
      vcl_cout << " ),\t fallback_wgt = ";
      volm_fallback_label::print_wgt(drs[i]->land_id());
      vcl_cout << " (";
      for (unsigned jj = 0; jj < obj_land_wgt[i].size(); jj++)
        vcl_cout << obj_land_wgt[i][jj] << ' ';
      vcl_cout << ')' << vcl_endl;
    }
  }
  
  volm_io::write_status(out_folder(), volm_io::PRE_PROCESS_FINISHED, 30);
  return volm_io::PRE_PROCESS_FINISHED;
}
#endif
