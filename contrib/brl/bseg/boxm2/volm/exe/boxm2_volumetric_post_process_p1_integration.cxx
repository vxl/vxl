//:
// \file
// \executable to generate depth_map_scene, camera_space and volm_query for volm_matcher
// \author Yi Dong
// \date Apr 03, 2013

#include <set>
#include <iostream>
#include <ios>
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
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bkml/bkml_write.h>


int main(int argc, char** argv)
{
  vul_arg<std::string> label_xml("-label", "labelme kml file", "");
  vul_arg<std::string> category_file("-cat", "category file for transferring labeled type to land id", "");
  vul_arg<std::string> geo_hypo_folder_a("-hypoa", "hypo folder for utm zone 17", "");
  vul_arg<std::string> geo_hypo_folder_b("-hypob", "hypo folder for utm zone 18", "");
  vul_arg<std::string> out("-out", "job output folder", "");
  vul_arg<float> kl ("-kl", "parameter for nonlinear score scaling", 200.0f);
  vul_arg<float> ku ("-ku", "parameter for nonlinear score scaling", 10.0f);
  vul_arg<unsigned> job_id ("-job", "query image job_id", 0);
  vul_arg<bool> logger("-logger", "option to write status.xml", false);
  vul_arg_parse(argc, argv);

  // check input parameters
  std::stringstream log;
  if (out().compare("") == 0 || label_xml().compare("") == 0 || category_file().compare("") == 0 ||
      geo_hypo_folder_a().compare("") == 0 ||
      geo_hypo_folder_b().compare("") == 0) {
      log << "EXE_ARGUMENT_ERROR!\n";
      std::cerr << log.str();
      volm_io::write_composer_log(out(), log.str());
      volm_io::write_status(out(), volm_io::EXE_ARGUMENT_ERROR, 95);
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
  }
  // load depth map scene
  if (!vul_file::exists(label_xml()) || !vul_file::exists(category_file())) {
    log << "EXE_ARGUMENT_ERROR! can not find labelme xml / category file\n";  std::cerr << log.str();
    volm_io::write_composer_log(out(), log.str());
    volm_io::write_status(out(), volm_io::EXE_ARGUMENT_ERROR, 95);
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  depth_map_scene_sptr dm = new depth_map_scene;
  std::string img_category;
  if (!volm_io::read_labelme(label_xml(), category_file(), dm, img_category)) {
    log << "EXE_ARGUMENT_ERROR! parsing label me file failed\n";  std::cerr << log.str();
    volm_io::write_composer_log(out(), log.str());
    volm_io::write_status(out(), volm_io::LABELME_FILE_IO_ERROR, 95);
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  if (img_category == "desert") {
    log << "ERROR: current matcher works only on coast\n";  std::cerr << log.str();
    volm_io::write_composer_log(out(), log.str());
    volm_io::write_status(out(), volm_io::LABELME_FILE_IO_ERROR, 95);
    return volm_io::LABELME_FILE_IO_ERROR;
  }

  // create prob map
  std::vector<volm_tile> tiles;
  tiles = volm_tile::generate_p1_wr2_tiles();

  // initialize the prob_map
  std::vector<vil_image_view<float> > tile_imgs;
  for (unsigned i = 0; i < tiles.size(); i++) {
    vil_image_view<float> img(3601,3601);
    img.fill(-1.0f);
    tile_imgs.push_back(img);
  }

  for (unsigned tile_id = 0; tile_id < tiles.size(); tile_id++) {
    volm_tile tile = tiles[tile_id];
    unsigned zone_id;
    std::string geo_hypo_folder;
    if (tile_id < 8 && tile_id != 5) {
      geo_hypo_folder = geo_hypo_folder_a();
    }
    else {
      geo_hypo_folder = geo_hypo_folder_b();
    }
    // load associate geo_hypo
    std::stringstream file_name_pre;
    file_name_pre << geo_hypo_folder << "/geo_index_tile_" << tile_id;
    float min_size;
    volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
    volm_geo_index::read_hyps(root, file_name_pre.str());
    std::vector<volm_geo_index_node_sptr> leaves;
    volm_geo_index::get_leaves_with_hyps(root, leaves);

    // load the score binary
    std::stringstream score_file;
    score_file << out() << "/ps_1_scores_tile_" << tile_id << ".bin";
    if (!vul_file::exists(score_file.str())) {
      std::cout << " WARNING: score file " << score_file.str() << " does not exist" << std::endl;
      continue;
    }
    std::vector<volm_score_sptr> scores;
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
    std::string img_name = out() + "/ProbMap_float_" + tiles[i].get_string() + ".tif";
    vil_save(tile_imgs[i], img_name.c_str());
  }

  // create top 30 list
  std::map<unsigned, std::vector<float> > max_scores;
  std::map<unsigned, std::vector<vgl_point_3d<double> > > max_locs;
  std::map<unsigned, std::vector<unsigned> > max_cameras;
  // search for the top 30 scores
  for (unsigned i = 0; i < tiles.size(); i++) {
    unsigned zone_id;
    std::stringstream score_file;
    score_file << out() << "/ps_1_scores_tile_" << i << ".bin";
    if (!vul_file::exists(score_file.str()))
      continue;
    vil_image_view<float> tile_img = tile_imgs[i];
    // use stl set temporary
    std::set<float> score_set;
    for (unsigned u = 0; u < tile_img.ni(); u++) {
      for (unsigned v = 0; v < tile_img.nj(); v++) {
        if (tile_img(u,v) > 0)
          score_set.insert(tile_img(u,v));
      }
    }
    std::vector<float> max_score;
    auto sit = score_set.end();
    --sit;  max_score.push_back(*sit);
    --sit;  max_score.push_back(*sit);
    --sit;  max_score.push_back(*sit);
    std::pair<unsigned, std::vector<float> > pair;
    pair.first = i;
    pair.second = max_score;
    max_scores.insert(pair);
  }

  // locate top 30 locations and cameras
  for (unsigned tile_id = 0; tile_id < tiles.size(); tile_id++) {
    unsigned zone_id;
    std::string geo_hypo_folder;
    if (tile_id < 8 && tile_id != 5) {
      geo_hypo_folder = geo_hypo_folder_a();
    }
    else {
      geo_hypo_folder = geo_hypo_folder_b();
    }
    // load associate geo_hypo
    std::stringstream file_name_pre;
    file_name_pre << geo_hypo_folder << "/geo_index_tile_" << tile_id;
    float min_size;
    volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
    volm_geo_index::read_hyps(root, file_name_pre.str());
    std::vector<volm_geo_index_node_sptr> leaves;
    volm_geo_index::get_leaves_with_hyps(root, leaves);
    // load the score binary
    std::stringstream score_file;
    score_file << out() << "/ps_1_scores_tile_" << tile_id << ".bin";
    if (!vul_file::exists(score_file.str()))
      continue;
    std::vector<vgl_point_3d<double> > loc_vec;
    std::vector<unsigned> cam_vec;
    loc_vec.resize(max_scores[tile_id].size());
    cam_vec.resize(max_scores[tile_id].size());
    std::vector<volm_score_sptr> scores;
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
    std::pair<unsigned, std::vector<vgl_point_3d<double> > > loc_pair;
    std::pair<unsigned, std::vector<unsigned> > cam_pair;
    loc_pair.first = tile_id;
    loc_pair.second = loc_vec;
    cam_pair.first = tile_id;
    cam_pair.second = cam_vec;
    max_locs.insert(loc_pair);
    max_cameras.insert(cam_pair);
  }

  // calculate roi based on maximum score
  float max_score_all = 0;
  for (auto & max_score : max_scores) {
    for (auto vit = max_score.second.begin(); vit != max_score.second.end(); ++vit)
      if (max_score_all < *vit) max_score_all = *vit;
  }
  std::vector<double> thresholds;
  float ds = 0.1f * max_score_all;
  thresholds.push_back(max_score_all - ds*3);
  //for (int i = -3; i < 0; i++)
  //  thresholds.push_back(max_score_all + ds*i);
  //thresholds.push_back(0.95*max_score_all);
  //thresholds.push_back(max_score_all);
  //  cnt_map -- key is the thresholds, element --- cnt_below, total pixel count, total pixel uncount
  std::map<float, std::vector<unsigned> > cnt_map;
  for (double & threshold : thresholds) {
      std::vector<unsigned> cnt_vec(3,0);
      std::pair<float, std::vector<unsigned> > cnt_pair;
      cnt_pair.first = threshold;  cnt_pair.second = cnt_vec;
      cnt_map.insert(cnt_pair);
  }
  for (unsigned i = 0; i < tiles.size(); ++i) {
    vil_image_view<float> tile_img = tile_imgs[i];
    for (unsigned u = 0; u < tile_img.ni(); u++) {
      for (unsigned v = 0; v < tile_img.nj(); v++) {
        // loop over all threshold
        for (auto & mit : cnt_map)
        {
          if (tile_img(u, v) < 0)
            mit.second[2]++;
          else {
            mit.second[1]++;
            if (tile_img(u, v) < mit.first)
              mit.second[0]++;
          }
        }
      }
    }
  }

  std::vector<double> score_roi;
  score_roi.push_back(0.0f);
  for (auto & mit : cnt_map)
  {
    double roi = 1.0 - (double)mit.second[0]/mit.second[1];
    score_roi.push_back(roi);
  }
  score_roi.push_back((float)cnt_map.begin()->second[1]);

#if 1
  // create png tile images for different thresholds, only generate png tile prob_map with thres smaller than ground truth score
  for (unsigned ti = 0; ti < tiles.size(); ++ti) {
    vil_image_view<float> tile_img = tile_imgs[ti];
    for (double & threshold : thresholds) {
      vil_image_view<vxl_byte> out_png(tile_img.ni(), tile_img.nj());
      out_png.fill(volm_io::UNKNOWN);
      // loop over current tile image to rescale the score to [0, 255]
      for (unsigned ii = 0; ii < tile_img.ni(); ++ii) {
        for (unsigned jj = 0; jj< tile_img.nj(); ++jj) {
          if (tile_img(ii, jj) > 0)
            out_png(ii, jj) = volm_io::scale_score_to_1_255_sig(kl(), ku(), threshold, tile_img(ii,jj));
            //out_png(ii, jj) = volm_io::scale_score_to_1_255(*vit, tile_img(ii,jj));
        }
      }
      // save the image
      std::string out_png_name = out() + "/" + "ProbMap_" + tiles[ti].get_string() + ".tif";
      vil_save(out_png, out_png_name.c_str());
    }
  }
#endif

  std::string eoi_file = out() + "/roi_result_max_score.txt";
  std::ofstream fout(eoi_file.c_str());
  fout << "  test_id      gt_loc_score      total_locs                                       thresholds\n"
       << "----------------------------------------------------------------------------------------------------------------------------\n"
       << "                                        ";
  fout.setf(std::ios::right);
  for (double & threshold : thresholds) {
    fout.precision(6); fout.width(13); fout.fill(' ');
    fout << threshold;
  }
  fout << '\n';
  std::stringstream out_str;
  out_str << "job_" << job_id();
  fout << out_str.str();
  fout.precision(4); fout.width(13); fout.fill(' ');
  fout << score_roi[0] << ' ';
  auto tot_loc = (unsigned)score_roi[score_roi.size()-1];
  fout.width(13); fout.fill(' ');
  fout << tot_loc << "        ";
  for (unsigned i = 1; i < thresholds.size()+1; i++) {
    fout.setf(std::ios::right);
    fout.precision(6); fout.width(13); fout.fill(' ');
    fout << score_roi[i];
  }
  fout << "\n----------------------------------------------------------------------------------------------------------------------------\n";
  fout.close();

  // generate top 30 BestCamera.kml
  // load camera space
  std::string cam_bin = out() + "/camera_space.bin";
  if (!vul_file::exists(cam_bin)) {
    log << "ERROR: can not find camera_space binary: " << cam_bin << '\n';
    volm_io::write_composer_log(out(), log.str());
    volm_io::write_status(out(), volm_io::CAM_FILE_IO_ERROR, 95);
    return volm_io::CAM_FILE_IO_ERROR;
  }
  vsl_b_ifstream cam_ifs(cam_bin);
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(cam_ifs);
  cam_ifs.close();

  std::stringstream rational_folder;
  rational_folder << out() << "/rationale";
  vul_file::make_directory(rational_folder.str());

  // combine everything
  // key-score -- pair.first--cam_id, pair.second -- locs
  std::map<float, std::pair<unsigned, vgl_point_3d<double> > > score_map_all;
  for (auto & max_score : max_scores) {
    for (unsigned ii = 0; ii < max_score.second.size(); ii++) {
      std::pair<float, std::pair<unsigned, vgl_point_3d<double> > > pair_out;
      std::pair<unsigned, vgl_point_3d<double> > pair_in;
      pair_in.first = max_cameras[max_score.first][ii];  pair_in.second = max_locs[max_score.first][ii];
      pair_out.first = max_score.second[ii];             pair_out.second = pair_in;
      score_map_all.insert(pair_out);
    }
  }
  // generate top 30 best location kml
  std::string log_fname = rational_folder.str() + "/matchimg.log";
  std::ofstream ofs_log(log_fname.c_str());
  unsigned cnt = 30;
  auto mit_all = score_map_all.end();
  while (cnt) {
    --mit_all; --cnt;

    cam_angles cam_ang = cam_space->camera_angles(mit_all->second.first);
    double head = (cam_ang.heading_ < 0) ? cam_ang.heading_ + 360.0 : cam_ang.heading_;
    double tilt = (cam_ang.tilt_ < 0) ? cam_ang.tilt_ + 360 : cam_ang.tilt_;
    double roll;
    if (cam_ang.roll_ * cam_ang.roll_ < 1E-10) roll = 0;
    else                                       roll = cam_ang.roll_;

    double tfov = cam_ang.top_fov_;
    double tv_rad = tfov / vnl_math::deg_per_rad;
    double ttr = std::tan(tv_rad);
    double rfov = std::atan( dm->ni() * ttr / dm->nj() ) * vnl_math::deg_per_rad;

    std::stringstream cam_kml;
    cam_kml << rational_folder.str() << "/matchimg_" << (29-cnt) << ".kml";
    std::stringstream kml_name;
    kml_name << "matchimg_" << (29-cnt) << ".jpg";
    std::ofstream ofs_kml(cam_kml.str().c_str());
    bkml_write::open_document(ofs_kml);
    bkml_write::write_photo_overlay(ofs_kml,kml_name.str(),
                                    mit_all->second.second.x(), mit_all->second.second.y(), mit_all->second.second.z(),
                                    head, tilt, roll, tfov, rfov);
    bkml_write::close_document(ofs_kml);
    ofs_kml.close();
    //write the log file
    fout.fill(0);
    ofs_log << "MATCH " << (29-cnt) << ' ' << std::setprecision(8) << mit_all->second.second.y()
                                    << ' ' << std::setprecision(8) << mit_all->second.second.x()
                                    << ' ' << std::setprecision(8) << head << "    0\n";

    std::cout << " top " << (29-cnt) << '(' << cnt << ") " << " score = " << mit_all->first << ", cam = " << mit_all->second.first << ", loc = " << mit_all->second.second << std::endl;
  }
  ofs_log.close();

  volm_io::write_status(out(), volm_io::SUCCESS, 95);
  return volm_io::SUCCESS;
}
