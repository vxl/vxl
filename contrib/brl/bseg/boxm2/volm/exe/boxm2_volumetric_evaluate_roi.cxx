//:
// \file
// \executable to read score profile from generated probability map for threshold ranging from 0.4 to 0.9
//
//
// \author Yi Dong
// \date Feb 18, 2013

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vcl_set.h>
#include <vcl_ios.h>
#include <bvrml/bvrml_write.h>

// generate gt hypos
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> gt_file("-gt_locs", "file with the gt locs of all test cases", "");
  vul_arg<vcl_string> out_root("-out_rt", "experiment output root", "");
  vul_arg<unsigned> pass_id("-pass", "from pass 0 to pass 1", 1);
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  vcl_stringstream log;
  if (out_root().compare("") == 0 ||
      gt_file().compare("") == 0 ||
      pass_id() > 2)
  {
    log << "EXE_ARGUMENT_ERROR!\n";
    vul_arg_display_usage_and_exit();
    volm_io::write_composer_log(out_root(), log.str());
    volm_io::write_log(out_root(), log.str());
    vcl_cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // read gt location, i.e., lat and lon
  if (!vul_file::exists(gt_file())) {
    log << "ERROR : can not find ground truth position file -->" << gt_file() << '\n';
    volm_io::write_composer_log(out_root(), log.str());
    volm_io::write_log(out_root(), log.str());
    vcl_cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_string, vcl_string> > > samples;
  /* unsigned int cnt = */ volm_io::read_gt_file(gt_file(), samples);

  // obtain the ground truth score from create_probmap.xml

  // the overall ROI for each test_image
  // key -- test_img_id, vector< gt_max_score, ROI for different thres, total_pixel_evaluated >
  vcl_map<unsigned, vcl_vector<float> > test_img_roi;
  vcl_map<unsigned, vcl_vector<float> > test_img_thres;
  /*vcl_vector<float> thresholds;
  for (unsigned i = 4; i < 10; ++i) {
    thresholds.push_back(0.1*i);
  }*/

  // loop over all possible test images
  for (unsigned id = 0; id < 100; ++id) {
    vcl_stringstream out_folder;
    if (id < 10)
      out_folder << out_root() << "/p1a_test1_0" << id;
    else
      out_folder << out_root() << "/p1a_test1_" << id;
    //vcl_cout << "out_folder = " << out_folder.str() << vcl_endl;
    if (!vul_file::is_directory(out_folder.str())) {
      continue;
    }
    vcl_cout << " we have test image folder --> " << out_folder.str() << vcl_endl;
    vcl_cout << " with image category --> " << samples[id].second.second << vcl_endl;

    vcl_stringstream log_test_img;
    vcl_string log_file = out_folder.str() + "/evaluate_roi_log.xml";

    // create tile based on current image category
    vcl_vector<volm_tile> tiles;
    if (samples[id].second.second == "desert")
      tiles = volm_tile::generate_p1_wr1_tiles();
    else
      tiles = volm_tile::generate_p1_wr2_tiles();

    // get the score for ground truth location and and highest score for all locations
    float gt_score = 0.0f;
    float max_score_all = 0.0f;
    for (unsigned i = 0; i < tiles.size(); i++) {
      vcl_string img_name = out_folder.str() + "/" + "ProbMap_" + tiles[i].get_string() + ".tif";
      if (!vul_file::exists(img_name)) {
        log_test_img << " WARNING: missing prob_map: " << img_name << '\n';
        volm_io::write_post_processing_log(log_file, log_test_img.str());
        vcl_cerr << log_test_img.str();
        continue;
      }
      vil_image_view<float> tile_img = vil_load(img_name.c_str());
      // search for the maximum score for all locations
      for (unsigned ii = 0; ii < tile_img.ni(); ii++)
        for (unsigned jj = 0; jj < tile_img.nj(); jj++)
          if (tile_img(ii, jj) > max_score_all && tile_img(ii,jj) > 0) max_score_all = tile_img(ii,jj);
      // search for the maximum score for ground truth location
      unsigned u, v;
      if (tiles[i].global_to_img(samples[id].first.x(), samples[id].first.y(), u, v)) {
        if (u < tile_img.ni() && v < tile_img.nj())
          gt_score = tile_img(u,v);
          log_test_img << "\t id = " << id << ", GT location: " << samples[id].first.x() << ", "
                  << samples[id].first.y() << " is at pixel: "
                  << u << ", " << v << " in tile " << i << " and has value: "
                  << gt_score << '\n';
          volm_io::write_post_processing_log(log_file, log_test_img.str());
          vcl_cerr << log_test_img.str();
      }
    }

    // create the threshold array based on ground truth score and maximum score
    vcl_vector<float> thresholds;
    float ds = 0.05f * gt_score;
    for (int i = -4; i < 0; i++)
      thresholds.push_back(gt_score + ds*i);
    thresholds.push_back(gt_score);
    thresholds.push_back(max_score_all);
    vcl_pair<unsigned, vcl_vector<float> > p_thres;
    p_thres.first = id;
    p_thres.second = thresholds;
    test_img_thres.insert(p_thres);

    vcl_cerr << " test_id = " << id << ", gt_score = " << gt_score << ", max_score = " << max_score_all << vcl_endl;
    // calculate roi for current valid out_folder
    //  cnt_map -- key is the thresholds, element --- cnt_below, total pixel count, total pixel uncount
    vcl_map<float, vcl_vector<unsigned> > cnt_map;
    for (vcl_vector<float>::iterator vit = thresholds.begin(); vit != thresholds.end(); ++vit) {
      vcl_vector<unsigned> cnt_vec(3,0);
      vcl_pair<float, vcl_vector<unsigned> > cnt_pair;
      cnt_pair.first = *vit;  cnt_pair.second = cnt_vec;
      cnt_map.insert(cnt_pair);
    }
    
    // loop over all tiles of current test images
    for (unsigned i = 0; i < tiles.size(); ++i) {
      vcl_string img_name = out_folder.str() + "/" + "ProbMap_" + tiles[i].get_string() + ".tif";
      if (!vul_file::exists(img_name)) {
        log_test_img << " WARNING: missing prob_map: " << img_name << '\n';
        volm_io::write_post_processing_log(log_file, log_test_img.str());
        vcl_cerr << log_test_img.str();
        continue;
      }
      vil_image_view<float> tile_img = vil_load(img_name.c_str());
      for (unsigned ii = 0; ii < tile_img.ni(); ++ii) {
        for (unsigned jj = 0; jj < tile_img.nj(); ++jj) {
          // loop over all threshold
          for ( vcl_map<float, vcl_vector<unsigned> >::iterator mit = cnt_map.begin(); mit != cnt_map.end(); ++mit)
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
    vcl_vector<float> score_roi;
    score_roi.push_back(gt_score);
    for (vcl_map<float, vcl_vector<unsigned> >::iterator mit = cnt_map.begin();
         mit != cnt_map.end(); ++mit)
    {
      float roi = (float)mit->second[0]/mit->second[1];
      score_roi.push_back(roi);
    }
    score_roi.push_back((float)cnt_map.begin()->second[1]);
    vcl_pair<unsigned, vcl_vector<float> > pair_roi;
    pair_roi.first = id;
    pair_roi.second = score_roi;
    test_img_roi.insert(pair_roi);

#if 1
    // create png tile images for different thresholds, only generate png tile prob_map with thres smaller than ground truth score
    for (unsigned ti = 0; ti < tiles.size(); ++ti) {
      vcl_string img_name = out_folder.str() + "/" + "ProbMap_" + tiles[ti].get_string() + ".tif";
      if (!vul_file::exists(img_name)) {
        log_test_img << " WARNING: missing prob_map: " << img_name << '\n';
        volm_io::write_post_processing_log(log_file, log_test_img.str());
        vcl_cerr << log_test_img.str();
        continue;
      }
      vil_image_view<float> tile_img = vil_load(img_name.c_str());
      for (vcl_vector<float>::iterator vit = thresholds.begin(); vit != thresholds.end(); ++vit) {
        if (*vit < gt_score) {
          vil_image_view<vxl_byte> out_png(tile_img.ni(), tile_img.nj());
          out_png.fill(volm_io::UNEVALUATED);
          // loop over current tile image to rescale the score to [0, 255]
          for (unsigned ii = 0; ii < tile_img.ni(); ++ii) {
            for (unsigned jj = 0; jj< tile_img.nj(); ++jj) {
              if (tile_img(ii, jj) > 0)
                out_png(ii, jj) = volm_io::scale_score_to_1_255(*vit, tile_img(ii,jj));
            }
          }
          // save the image
          vcl_stringstream out_png_name;
          out_png_name << out_folder.str() << "/ProbMap_" << tiles[ti].get_string() << "_thres_" << *vit << ".png";
          vil_save(out_png, (out_png_name.str()).c_str());
        }
      }
    }
#endif

#if 1
    // create color png tile images for different thresholds, only generate png tile porb_map with thres lower than ground truth score
    for (unsigned ti = 0; ti < tiles.size(); ti++) {
      vcl_string img_name = out_folder.str() + "/" + "ProbMap_" + tiles[ti].get_string() + ".tif";
      if (!vul_file::exists(img_name)) {
        log_test_img << " WARNING: missing prob_map: " << img_name << '\n';
        volm_io::write_post_processing_log(log_file, log_test_img.str());
        vcl_cerr << log_test_img.str();
        continue;
      }
      vil_image_view<float> tile_img = vil_load(img_name.c_str());
      for (vcl_vector<float>::iterator vit = thresholds.begin(); vit != thresholds.end(); ++vit) {
        if (*vit < gt_score) {
          vil_image_view<vil_rgb<vxl_byte> > out_png(tile_img.ni(), tile_img.nj());
          // initialize the image
          for (unsigned i = 0; i < out_png.ni(); i++)
            for (unsigned j = 0; j < out_png.nj(); j++) {
              out_png(i,j).r = (unsigned char)255;  out_png(i,j).g = (unsigned char)255;  out_png(i,j).b = (unsigned char)255;
            }
          // loop over current tile image to rescale the score to [0, 255]
          for (unsigned ii = 0; ii < tile_img.ni(); ii++) {
            for (unsigned jj = 0; jj< tile_img.nj(); jj++) {
              if (tile_img(ii, jj) > 0) {
                unsigned color_id = volm_io::scale_score_to_1_255(*vit, tile_img(ii,jj));
                out_png(ii,jj).r = bvrml_color::heatmap_classic[color_id][0];
                out_png(ii,jj).g = bvrml_color::heatmap_classic[color_id][1];
                out_png(ii,jj).b = bvrml_color::heatmap_classic[color_id][2];
              }
            }
          }
          // save the image
          vcl_stringstream out_png_name;
          out_png_name << out_folder.str() << "/ProbMap_" << tiles[ti].get_string() << "_thres_rgb_" << *vit << ".png";
          vil_save(out_png, (out_png_name.str()).c_str());
        }
      }
    }
#endif

  } // end of test_id loop

  vcl_string eoi_file = out_root() + "/roi_result.txt";
  vcl_ofstream fout(eoi_file.c_str());

  fout << "  test_id      gt_loc_score      total_locs                                       thresholds\n";
  fout << "----------------------------------------------------------------------------------------------------------------------------\n";
  
  //for (vcl_vector<float>::iterator vit = thresholds.begin(); vit != thresholds.end(); ++vit) {
  //  fout.setf(vcl_ios_right);
  //  fout.precision(2); fout.fill(' '); fout.width(*vit > 0.6 ? 13 : 10);
  //  fout << *vit;
  //}
  fout.setf(vcl_ios_right);
  vcl_map<unsigned, vcl_vector<float> >::iterator mit = test_img_roi.begin();
  for (; mit != test_img_roi.end(); ++mit) {
    unsigned id = mit->first;

    // print out the thresholds array
    fout << "                                              ";
    vcl_vector<float> thresholds = test_img_thres[id];
    for (vcl_vector<float>::iterator vit = thresholds.begin(); vit != thresholds.end(); ++vit) {
      fout.precision(2); fout.fill(' '); fout.width(13);
      fout << *vit;
    }
    fout << '\n';
    vcl_stringstream out_str;
    if (id < 10) out_str << "p1a_test1_0" << mit->first;
    else                 out_str << "p1a_test1_"  << mit->first;
    fout << out_str.str();
    fout.precision(4); fout.width(13); fout.fill(' ');
    fout << mit->second[0] << ' ';
    unsigned tot_loc = mit->second[(unsigned int)mit->second.size()-1];
    fout.width(13); fout.fill(' ');
    fout << tot_loc << "        ";
    for (unsigned i = 1; i < thresholds.size()+1; i++) {
      fout.setf(vcl_ios_right);
      fout.precision(5); fout.width(13); fout.fill(' ');
      fout << mit->second[i];
    }
    fout << "\n----------------------------------------------------------------------------------------------------------------------------\n";
  }
  fout.close();

#if 0
  vcl_map<unsigned, vcl_vector<float> >::iterator mit = test_img_roi.begin();
  
  for (; mit != test_img_roi.end(); ++mit) {
    vcl_stringstream out_str;
    if (mit->first < 10) out_str << "p1a_test1_0" << mit->first;
    else                 out_str << "p1a_test1_"  << mit->first;
    fout << out_str.str();
    fout.precision(5); fout.width(13); fout.fill(' ');
    fout << mit->second[0] << ' ';
    unsigned tot_loc = (unsigned)(mit->second[mit->second.size()-1]);
    fout.width(13); fout.fill(' ');
    fout << tot_loc << "        ";
    for (unsigned i = 1; i < 7; ++i) {
      fout.setf(vcl_ios_right);
      fout.precision(5); fout.width(12); fout.fill(' ');
      fout << mit->second[i];
    }
    fout << '\n';
  }
  fout.close();
#endif

  return volm_io::SUCCESS;
}
