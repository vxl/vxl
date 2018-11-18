//:
// \file
// \executable to read score profile from generated probability map for threshold ranging from 0.4 to 0.9
//
//
// \author Yi Dong
// \date Feb 18, 2013

#include <set>
#include <iostream>
#include <ios>
#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bvrml/bvrml_write.h>

int main(int argc, char** argv)
{
  vul_arg<unsigned>    world_id("-world",     "ROI world id", 9999);
  vul_arg<std::string>       out("-out", "experiment output root", "");
  vul_arg<float>             kl("-kl", "parameter for nonlinear score scaling", 200.0f);
  vul_arg<float>             ku("-ku", "parameter for nonlinear score scaling", 10.0f);
  vul_arg_parse(argc, argv);

  if (out().compare("") == 0 || world_id() == 9999) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::stringstream log;
  std::string log_file = out() + "/evaluate_roi_max_log.xml";

  // create tile
  // start to create the probability map
  std::vector<volm_tile> tiles;
  if (!volm_tile::generate_tiles(world_id(), tiles)) {
    log << "ERROR: unknown world id " << world_id() << "!\n";
    volm_io::write_error_log(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // start to calculate roi
  std::vector<double> test_img_roi;
  std::vector<double> test_img_thres;

  // get the score for ground truth location and and highest score for all locations
  float gt_score = 0.0f;
  float max_score_all = 0.0f;
  for (auto & tile : tiles) {
    std::string img_name = out() + "/" + "ProbMap_float_" + tile.get_string() + ".tif";
    if (!vul_file::exists(img_name)) {
      log << " WARNING: missing prob_map: " << img_name << '\n';
      volm_io::write_post_processing_log(log_file, log.str());
      std::cerr << log.str();
      continue;
    }
    vil_image_view<float> tile_img = vil_load(img_name.c_str());
    // search for the maximum score for all locations
    for (unsigned ii = 0; ii < tile_img.ni(); ii++)
      for (unsigned jj = 0; jj < tile_img.nj(); jj++)
        if (tile_img(ii, jj) > max_score_all && tile_img(ii,jj) > 0) max_score_all = tile_img(ii,jj);
  }

  float ds = 0.1f * max_score_all;
  for (int i = -3; i < 0; i++)
    test_img_thres.push_back(max_score_all + ds*i);
  test_img_thres.push_back(0.95*max_score_all);
  test_img_thres.push_back(max_score_all);

  std::cerr << " Among all locations " << gt_score << ", max_score = " << max_score_all << '\n';
  // calculate roi for current valid out_folder
  // cnt_map -- key is the thresholds, element --- cnt_below, total pixel count, total pixel uncount
  std::map<float, std::vector<unsigned> > cnt_map;
  for (double & test_img_thre : test_img_thres) {
    std::vector<unsigned> cnt_vec(3,0);
    std::pair<float, std::vector<unsigned> > cnt_pair;
    cnt_pair.first = test_img_thre;  cnt_pair.second = cnt_vec;
    cnt_map.insert(cnt_pair);
  }

  // loop over all tiles of current test images
  for (auto & tile : tiles) {
    std::string img_name = out() + "/" + "ProbMap_float_" + tile.get_string() + ".tif";
    if (!vul_file::exists(img_name)) {
      log << " WARNING: missing prob_map: " << img_name << '\n';
      volm_io::write_post_processing_log(log_file, log.str());
      std::cerr << log.str();
      continue;
    }
    vil_image_view<float> tile_img = vil_load(img_name.c_str());
    for (unsigned ii = 0; ii < tile_img.ni(); ++ii) {
      for (unsigned jj = 0; jj < tile_img.nj(); ++jj) {
        // loop over all threshold
        for (auto & mit : cnt_map)
        {
          if (tile_img(ii, jj) < 0)
            mit.second[2]++;
          else {
            mit.second[1]++;
            if (tile_img(ii, jj) < mit.first)
              mit.second[0]++;
          }
        }
      }
    }
  } // end of tile loop

  // calculate ROI, save the gt_score and ROI for current test_id
  test_img_roi.push_back(gt_score);
  for (auto & mit : cnt_map)
  {
    float roi = 1.0 - (double)mit.second[0]/mit.second[1];
    test_img_roi.push_back(roi);
  }
  test_img_roi.push_back((float)cnt_map.begin()->second[1]);

  // create folder for different thresholds,
  std::vector<std::string> prob_thres_folders;
  for (double & test_img_thre : test_img_thres) {
    std::stringstream folder_name;
    folder_name << out() << "/ProbMap_scaled_" << test_img_thre;
    vul_file::make_directory(folder_name.str());
    prob_thres_folders.push_back(folder_name.str());
  }

  // create tile images for different thresholds, only generate png tile prob_map with thres smaller than ground truth score
  for (auto & tile : tiles) {
    std::string img_name = out() + "/" + "ProbMap_float_" + tile.get_string() + ".tif";
    if (!vul_file::exists(img_name)) {
      log << " WARNING: missing prob_map: " << img_name << '\n';
      volm_io::write_post_processing_log(log_file, log.str());
      std::cerr << log.str();
      continue;
    }
    vil_image_view<float> tile_img = vil_load(img_name.c_str());
    unsigned cnt = 0;
    for (double & test_img_thre : test_img_thres) {
      vil_image_view<vxl_byte> out_png(tile_img.ni(), tile_img.nj());
      out_png.fill(volm_io::UNKNOWN);
      // loop over current tile image to rescale the score to [0, 255]
      for (unsigned ii = 0; ii < tile_img.ni(); ++ii) {
        for (unsigned jj = 0; jj< tile_img.nj(); ++jj) {
          if (tile_img(ii, jj) > 0)
            out_png(ii, jj) = volm_io::scale_score_to_1_255_sig(kl(), ku(), test_img_thre, tile_img(ii,jj));
            //out_png(ii, jj) = volm_io::scale_score_to_1_255(*vit, tile_img(ii,jj));
        }
      }
      // save the image
      std::string out_png_name = prob_thres_folders[cnt++] + "/" + "ProbMap_" + tile.get_string() + ".tif";
      vil_save(out_png, out_png_name.c_str());
    }
  }

  // write down the ROI values
  std::string eoi_file = out() + "/roi_result_max_score.txt";
  std::ofstream fout(eoi_file.c_str());

  fout << "  test_id      gt_loc_score      total_locs                                       thresholds\n"
       << "----------------------------------------------------------------------------------------------------------------------------\n";
  fout.setf(std::ios::right);
  std::vector<double> thresholds = test_img_thres;
  fout << "                                              ";
  for (double & threshold : thresholds) {
    fout.precision(6); fout.fill(' '); fout.width(13);
    fout << threshold;
  }
  fout << '\n';
  std::stringstream out_str;
  out_str << "query_image";
  fout << out_str.str();
  fout.precision(4); fout.width(13); fout.fill(' ');
  fout << test_img_roi[0] << ' ';
  auto tot_loc = (unsigned int)test_img_roi[test_img_roi.size()-1];
  fout.width(13); fout.fill(' ');
  fout << tot_loc << "        ";
  for (unsigned i = 1; i < thresholds.size()+1; i++) {
    fout.setf(std::ios::right);
    fout.precision(6); fout.width(13); fout.fill(' ');
    fout << test_img_roi[i];
  }
  fout << "\n----------------------------------------------------------------------------------------------------------------------------\n";
  fout.close();

  return volm_io::SUCCESS;
}

#if 0
int main(int argc,  char** argv)
{
  vul_arg<std::string> gt_file("-gt_locs", "file with the gt locs of all test cases", "");
  vul_arg<std::string> out("-out", "experiment output root", "");
  vul_arg<unsigned> pass_id("-pass", "from pass 0 to pass 1", 1);
  vul_arg<unsigned> test_id("-test", "test1 or 2 or 3", 1);
  vul_arg<unsigned> id("-id", "test image id",1000);
  vul_arg<float> kl ("-kl", "parameter for nonlinear score scaling", 200.0f);
  vul_arg<float> ku ("-ku", "parameter for nonlinear score scaling", 10.0f);
  vul_arg_parse(argc, argv);
  std::cout << "argc: " << argc << std::endl;
  std::stringstream log;
  if (out().compare("") == 0 ||
      gt_file().compare("") == 0 ||
      pass_id() > 2 ||
      test_id() > 3 ||
      test_id() < 1 || id() >= 1000)
  {
    log << "EXE_ARGUMENT_ERROR!\n";
    vul_arg_display_usage_and_exit();
    volm_io::write_composer_log(out(), log.str());
    volm_io::write_log(out(), log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // read gt location, i.e., lat and lon
  if (!vul_file::exists(gt_file())) {
    log << "ERROR : can not find ground truth position file -->" << gt_file() << '\n';
    volm_io::write_composer_log(out(), log.str());
    volm_io::write_log(out(), log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::vector<std::pair<vgl_point_3d<double>, std::pair<std::pair<std::string, int>, std::string> > > samples;
  volm_io::read_gt_file(gt_file(), samples);
  // create tiles based on image id and associate ROI
  std::vector<volm_tile> tiles;
  if (samples[id()].second.second == "desert")
    tiles = volm_tile::generate_p1_wr1_tiles();
  else if (samples[id()].second.second == "coast")
    tiles = volm_tile::generate_p1_wr2_tiles();
  else if (samples[id()].second.second == "Chile")
    tiles = volm_tile::generate_p1b_wr1_tiles();
  else if (samples[id()].second.second == "India")
    tiles = volm_tile::generate_p1b_wr2_tiles();
  else if (samples[id()].second.second == "Jordan")
    tiles = volm_tile::generate_p1b_wr3_tiles();
  else if (samples[id()].second.second == "Philippines")
    tiles = volm_tile::generate_p1b_wr4_tiles();
  else if (samples[id()].second.second == "Taiwan")
    tiles = volm_tile::generate_p1b_wr5_tiles();
  else {
    log << "ERROR: cannot find ROI for image id " << id() << "\n";
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // start to calculate roi
  std::vector<double> test_img_roi;
  std::vector<double> test_img_thres;

  std::stringstream log_test_img;
  std::string log_file = out() + "/evaluate_roi_max_log.xml";

  // get the score for ground truth location and and highest score for all locations
  float gt_score = 0.0f;
  float max_score_all = 0.0f;
  for (unsigned i = 0; i < tiles.size(); i++) {
    std::string img_name = out() + "/" + "ProbMap_float_" + tiles[i].get_string() + ".tif";
    if (!vul_file::exists(img_name)) {
      log_test_img << " WARNING: missing prob_map: " << img_name << '\n';
      volm_io::write_post_processing_log(log_file, log_test_img.str());
      std::cerr << log_test_img.str();
      continue;
    }
    vil_image_view<float> tile_img = vil_load(img_name.c_str());
    // search for the maximum score for all locations
    for (unsigned ii = 0; ii < tile_img.ni(); ii++)
      for (unsigned jj = 0; jj < tile_img.nj(); jj++)
        if (tile_img(ii, jj) > max_score_all && tile_img(ii,jj) > 0) max_score_all = tile_img(ii,jj);
    // search for the maximum score for ground truth location
    unsigned u, v;
    if (tiles[i].global_to_img(samples[id()].first.x(), samples[id()].first.y(), u, v)) {
      if (u < tile_img.ni() && v < tile_img.nj())
        gt_score = tile_img(u,v);
        log_test_img << "\t id = " << id() << ", GT location: " << samples[id()].first.x() << ", "
                      << samples[id()].first.y() << " is at pixel: "
                      << u << ", " << v << " in tile " << i << " and has value: "
                      << gt_score << '\n';
        volm_io::write_post_processing_log(log_file, log_test_img.str());
        std::cerr << log_test_img.str();
    }
  }

  float ds = 0.1f * max_score_all;
  for (int i = -3; i < 0; i++)
    test_img_thres.push_back(max_score_all + ds*i);
  test_img_thres.push_back(0.95*max_score_all);
  test_img_thres.push_back(max_score_all);

  std::cerr << " test_id = " << id() << ", gt_score = " << gt_score << ", max_score = " << max_score_all << '\n';
  // calculate roi for current valid out_folder
  // cnt_map -- key is the thresholds, element --- cnt_below, total pixel count, total pixel uncount
  std::map<float, std::vector<unsigned> > cnt_map;
  for (std::vector<double>::iterator vit = test_img_thres.begin(); vit != test_img_thres.end(); ++vit) {
    std::vector<unsigned> cnt_vec(3,0);
    std::pair<float, std::vector<unsigned> > cnt_pair;
    cnt_pair.first = *vit;  cnt_pair.second = cnt_vec;
    cnt_map.insert(cnt_pair);
  }
  // loop over all tiles of current test images
  for (unsigned i = 0; i < tiles.size(); ++i) {
    std::string img_name = out() + "/" + "ProbMap_float_" + tiles[i].get_string() + ".tif";
    if (!vul_file::exists(img_name)) {
      log_test_img << " WARNING: missing prob_map: " << img_name << '\n';
      volm_io::write_post_processing_log(log_file, log_test_img.str());
      std::cerr << log_test_img.str();
      continue;
    }
    vil_image_view<float> tile_img = vil_load(img_name.c_str());
    for (unsigned ii = 0; ii < tile_img.ni(); ++ii) {
      for (unsigned jj = 0; jj < tile_img.nj(); ++jj) {
        // loop over all threshold
        for ( std::map<float, std::vector<unsigned> >::iterator mit = cnt_map.begin(); mit != cnt_map.end(); ++mit)
        {
          if (tile_img(ii, jj) < 0)
            mit->second[2]++;
          else {
            mit->second[1]++;
            if (tile_img(ii, jj) < mit->first)
              mit->second[0]++;
          }
        }
      }
    }
  } // end of tile loop

  // calculate ROI, save the gt_score and ROI for current test_id
  test_img_roi.push_back(gt_score);
  for (std::map<float, std::vector<unsigned> >::iterator mit = cnt_map.begin();
        mit != cnt_map.end(); ++mit)
  {
    float roi = 1.0 - (double)mit->second[0]/mit->second[1];
    test_img_roi.push_back(roi);
  }
  test_img_roi.push_back((float)cnt_map.begin()->second[1]);


  // create folder for different thresholds,
  std::vector<std::string> prob_thres_folders;
  for (std::vector<double>::iterator vit = test_img_thres.begin(); vit != test_img_thres.end(); ++vit) {
    std::stringstream folder_name;
    float pre = *vit/max_score_all;
    folder_name << out() << "/ProbMap_scaled_" << *vit;
    vul_file::make_directory(folder_name.str());
    prob_thres_folders.push_back(folder_name.str());
  }
  // create png tile images for different thresholds, only generate png tile prob_map with thres smaller than ground truth score
  for (unsigned ti = 0; ti < tiles.size(); ++ti) {
    std::string img_name = out() + "/" + "ProbMap_float_" + tiles[ti].get_string() + ".tif";
    if (!vul_file::exists(img_name)) {
      log_test_img << " WARNING: missing prob_map: " << img_name << '\n';
      volm_io::write_post_processing_log(log_file, log_test_img.str());
      std::cerr << log_test_img.str();
      continue;
    }
    vil_image_view<float> tile_img = vil_load(img_name.c_str());
    unsigned cnt = 0;
    for (std::vector<double>::iterator vit = test_img_thres.begin(); vit != test_img_thres.end(); ++vit) {
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
      std::string out_png_name = prob_thres_folders[cnt++] + "/" + "ProbMap_" + tiles[ti].get_string() + ".tif";
      vil_save(out_png, out_png_name.c_str());
    }
  }

  // write down the ROI values
  std::string eoi_file = out() + "/roi_result_max_score.txt";
  std::ofstream fout(eoi_file.c_str());

  fout << "  test_id      gt_loc_score      total_locs                                       thresholds\n"
       << "----------------------------------------------------------------------------------------------------------------------------\n";

  //for (std::vector<double>::iterator vit = thresholds.begin(); vit != thresholds.end(); ++vit) {
  //  fout.setf(std::ios::right);
  //  fout.precision(2); fout.fill(' '); fout.width(*vit > 0.6 ? 13 : 10);
  //  fout << *vit;
  //}
  fout.setf(std::ios::right);
  std::vector<double> thresholds = test_img_thres;
  fout << "                                              ";
  for (std::vector<double>::iterator vit = thresholds.begin(); vit != thresholds.end(); ++vit) {
    fout.precision(6); fout.fill(' '); fout.width(13);
    fout << *vit;
  }
  fout << '\n';
  std::stringstream out_str;
  if (id() < 10) out_str << "p1a_test1_0" << id();
  else                 out_str << "p1a_test1_"  << id();
  fout << out_str.str();
  fout.precision(4); fout.width(13); fout.fill(' ');
  fout << test_img_roi[0] << ' ';
  unsigned tot_loc = (unsigned int)test_img_roi[test_img_roi.size()-1];
  fout.width(13); fout.fill(' ');
  fout << tot_loc << "        ";
  for (unsigned i = 1; i < thresholds.size()+1; i++) {
    fout.setf(std::ios::right);
    fout.precision(6); fout.width(13); fout.fill(' ');
    fout << test_img_roi[i];
  }
  fout << "\n----------------------------------------------------------------------------------------------------------------------------\n";
  fout.close();

  return volm_io::SUCCESS;


}
#endif
