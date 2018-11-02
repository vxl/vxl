//:
// \file
// \executable to read score profile from generated probability map for threshold ranging from 0.4 to 0.9
//  Note that the ground truth is require to define the threshold appropriately
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

bool is_same(double const& x, double const& y)
{
  return (std::fabs(x-y) < 1E-6);
}

int main(int argc, char** argv)
{
  vul_arg<unsigned>    world_id("-world",     "ROI world id", 9999);
  vul_arg<std::string>       out("-out", "experiment output root", "");
  vul_arg<float>             kl("-kl", "parameter for nonlinear score scaling", 200.0f);
  vul_arg<float>             ku("-ku", "parameter for nonlinear score scaling", 10.0f);
  vul_arg<double>        gt_lon("-lon",      "location longitude", 0.0);
  vul_arg<double>        gt_lat("-lat",      "location latitude", 0.0);
  vul_arg_parse(argc, argv);

  // input check
  if (out().compare("") == 0 || world_id() == 9999 || is_same(gt_lon(),0.0) || is_same(gt_lat(),0.0) ) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  std::stringstream log;
  std::string log_file = out() + "/evaluate_roi_log.xml";

  // create tile
  std::vector<volm_tile> tiles;
  if (!volm_tile::generate_tiles(world_id(), tiles)) {
    log << "ERROR: unknown world id " << world_id() << "!\n";
    volm_io::write_error_log(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // start to calculate roi
  std::vector<double> test_img_roi;
  std::vector<double> test_img_thres;

  // get the score for ground truth location and and maximum score for all locations
  float gt_score = 0.0f;
  float max_score_all = 0.0f;
  for (unsigned i = 0; i < tiles.size(); i++) {
    std::string img_name = out() + "/" + "ProbMap_float_" + tiles[i].get_string() + ".tif";
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
    // search for the maximum score for ground truth location
    unsigned u, v;
    if (tiles[i].global_to_img(gt_lon(), gt_lat(), u, v)) {
      if (u < tile_img.ni() && v < tile_img.nj())
        gt_score = tile_img(u,v);
        log << "\t query location: " << gt_lon() << "x" << gt_lat() << " is at pixel: "
                      << u << ", " << v << " in tile " << i << " and has value: "
                      << gt_score << '\n';
        volm_io::write_post_processing_log(log_file, log.str());
        std::cerr << log.str();
    }
  }

  // create the threshold array based on ground truth score and maximum score
  float ds = 0.05f * gt_score;
  for (int i = -4; i < 0; i++)
    test_img_thres.push_back(gt_score + ds*i);
  test_img_thres.push_back(0.99*gt_score);
  test_img_thres.push_back(gt_score);
  test_img_thres.push_back(max_score_all);

  std::cout << " for query location: " << gt_lon() << "x" << gt_lat() << ", gt_score = " << gt_score << ", max_score = " << max_score_all << '\n';
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
      if (test_img_thre < gt_score) {
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
        //std::stringstream out_png_name;
        std::string out_png_name = prob_thres_folders[cnt++] + "/" + "ProbMap_" + tile.get_string() + ".tif";
        vil_save(out_png, out_png_name.c_str());
      }
    }
  }

  // write down the ROI values
  std::string eoi_file = out() + "/roi_result.txt";
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
