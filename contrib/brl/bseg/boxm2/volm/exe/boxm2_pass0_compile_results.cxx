//:
// \file
// \executable to create probability map and evaluate ROI from volm_matcher scores
// Given a result (score) for test query image, generate paorbability map for given tiles
// and report the score for the ground truth loc, associated with its best camera
// \author Yi Dong
// \date Feb 18, 2013

#include <set>
#include <iostream>
#include <iomanip>
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
#include <bkml_write.h>

// generate gt hypos
int main(int argc,  char** argv)
{
  vul_arg<std::string> gt_file("-gt_locs", "file with the gt locs of all test cases", "");
  vul_arg<std::string> geo_hypo_folder("-hypo", "folder to read the geo hypotheses","");
  vul_arg<unsigned> zone_id("-zone", "zone_id", 0);
  vul_arg<std::string> candidate_list("-cand", "candidate list if exist", "");
  vul_arg<std::string> out("-out", "job output folder", "");
  vul_arg<unsigned> id("-id", "id of the test image", 6);
  vul_arg<unsigned> pass_id("-pass", "from pass 0 to pass 1", 1);
  vul_arg<std::string> cam_kml("-camkml",  "camera KML", "");
  vul_arg<std::string> cam_params("-camparams", "camera params", "");                                // query -- camera space binary
  vul_arg<std::string> best_cam_kml("-bestcamkml",  "camera KML", "");                                // query -- camera space binary
  vul_arg<std::string> dms_bin("-dms", "depth_map_scene binary", "");                             // query -- depth map scene

  vul_arg_parse(argc, argv);

  std::cout << "argc: " << argc << std::endl;
  std::stringstream log;
  std::string log_file = out() + "/create_prob_map_log.xml";

  if (out().compare("") == 0 ||
      geo_hypo_folder().compare("") == 0 ||
      gt_file().compare("") == 0 ||
      //img().compare("") == 0 ||
      pass_id() > 2 ||
      zone_id() == 0 ||
      id() > 100)
  {
    log << "EXE_ARGUMENT_ERROR!\n";
    vul_arg_display_usage_and_exit();
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // read gt location, i.e., lat and lon
  if (!vul_file::exists(gt_file())) {
    log << "ERROR : can not find ground truth position file -->" << gt_file() << '\n';
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::vector<std::pair<vgl_point_3d<double>, std::pair<std::pair<std::string, int>, std::string> > > samples;
  unsigned int cnt = volm_io::read_gt_file(gt_file(), samples);
  if (id() >= cnt) {
    log << "ERROR: gt_file " << gt_file() << " does not contain test id: " << id() << "!\n";
    volm_io::write_post_processing_log(log_file, log.str());
    std::cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // check whether we have candidate list for this query
  bool is_candidate = false;
  vgl_polygon<double> cand_poly;
  if ( candidate_list().compare("") != 0) {
    std::cout << " candidate list = " <<  candidate_list() << std::endl;
    if ( vul_file::extension(candidate_list()).compare(".txt") == 0) {
      volm_io::read_polygons(candidate_list(), cand_poly);
    }
    else {
      log << " ERROR: candidate list exist but with wrong format, only txt allowed" << candidate_list() << '\n';
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

  // initialize the Prob_map image if the prob_map doesn't exist
  // if the image exists, load the image instead
  std::vector<vil_image_view<float> > tile_imgs;
  for (auto & tile : tiles) {
    std::string img_name = out() + "/" + "ProbMap_" + tile.get_string() + ".tif";
    if (vul_file::exists(img_name)) {
      // load the image
      vil_image_view<float> out_img = vil_load(img_name.c_str());
      tile_imgs.push_back(out_img);
    }
    else {
      // create the image
      vil_image_view<float> out_img(3601, 3601);
      out_img.fill(-1.0f);
      tile_imgs.push_back(out_img);
    }
  }

  // look for the location and camera which provides max_score
  vgl_point_3d<double> max_score_loc;
  unsigned max_score_cam_id;
  float max_score = 0.0f;

  std::string geo_folders[1]={geo_hypo_folder()};

  for (const auto & geo_folder : geo_folders)
  {
    for (unsigned i = 0; i < tiles.size(); i++) {
      volm_tile tile = tiles[i];
      // read in the volm_geo_index for tile i
      std::stringstream file_name_pre;
      file_name_pre << geo_folder << "geo_index_tile_" << i;
      std::cout<<"Geo file "<<file_name_pre.str()<<std::endl;
      // no index for tile i exists, continue
      if (!vul_file::exists(file_name_pre.str() + ".txt")) {
        continue;
      }
      // check the zone and tile_id
      //if (zone_id() == 17 && i > 8)
      //    continue;
      //else if (zone_id() == 18 && i < 8 && i != 5)
      //    continue;
      //else if (zone_id()  == 11 && i > 4)
      //    continue;

      float min_size;
      volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
      volm_geo_index::read_hyps(root, file_name_pre.str());
      std::vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(root, leaves);

      // load score binary from output folder if exists
      std::stringstream score_file;
      score_file << out() << "ps_0_scores_" << "tile_" << i << ".bin";
      std::cout<<"Score file "<<score_file.str()<<std::endl;
      // continue if no score binary exists for this tile
      if (!vul_file::exists(score_file.str()))
      {
        std::cout<<"Score file does not exist"<<std::endl;
        continue;
      }
      std::vector<volm_score_sptr> scores;
      volm_score::read_scores(scores, score_file.str());
      // refill the image
      unsigned total_ind = scores.size();
      std::cout<<"total "<<total_ind<<std::endl;
      for (unsigned ii = 0; ii < total_ind; ii++) {
        // std::cout<<scores[ii]->max_score_<<' ';//<<std::endl;
        vgl_point_3d<double> h_pt = leaves[scores[ii]->leaf_id_]->hyps_->locs_[scores[ii]->hypo_id_];
        // look for location and camera giving maximum score
        if (scores[ii]->max_score_ > max_score) {
          max_score = scores[ii]->max_score_;
          max_score_loc = h_pt;
        }
        unsigned u, v;
        if (tile.global_to_img(h_pt.x(), h_pt.y(), u, v)) {
          if (u < tile.ni() && v < tile.nj()) {
            // check if this is the highest values for this pixel
            if (scores[ii]->max_score_ > tile_imgs[i](u,v))
              tile_imgs[i](u,v) = scores[ii]->max_score_;
            }
        }
      }
    } // end of tiles
  } // end of zone

  float gt_score = 0.0 ;

  // save the ProbMap image and output the grount truth score
  for (unsigned i = 0; i < tiles.size(); i++) {
    std::string img_name = out() + "/" + "ProbMap_" + tiles[i].get_string() + ".tif";
    vil_save(tile_imgs[i],img_name.c_str());
    unsigned u, v;
    if (tiles[i].global_to_img(samples[id()].first.x(), samples[id()].first.y(), u, v) ) {
      if (u < tiles[i].ni() && v < tiles[i].nj()) {
        log << "\t GT location: " << id() << ", "
            << samples[id()].first.x() << ", "
            << samples[id()].first.y() << " is at pixel: "
            << u << ", " << v << " in tile " << i << " and has value: "
            << tile_imgs[i](u, v)
            << " max score for this test_img = " << max_score
          //<< " given by camera " << max_cam_ang.get_string()
            << " at location " << max_score_loc.x() << ", " << max_score_loc.y()
            << '\n';
        gt_score = tile_imgs[i](u, v);
        //volm_io::write_post_processing_log(log_file, log.str());
        //std::cout << log.str();
      }
    }
  }
  //: getting the camera id from Yi's Result.

  volm_io_expt_params camera_params;
  camera_params.read_params(cam_params());
  depth_map_scene_sptr dms = new depth_map_scene;
  vsl_b_ifstream dis(dms_bin().c_str());
  if (!dis)
    return volm_io::EXE_ARGUMENT_ERROR;
  dms->b_read(dis);
  dis.close();
  // check camera input file
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double tfov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_kml(), dms->ni(), dms->nj(), heading, heading_dev,
      tilt, tilt_dev, roll, roll_dev, tfov, top_fov_dev, altitude, lat, lon)) {
    std::cout << "problem parsing camera kml file: " << cam_kml()<< '\n';
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_camera_space_sptr  cam_space
      = new volm_camera_space(tfov, top_fov_dev, camera_params.fov_inc, altitude, dms->ni(), dms->nj(),
                              heading, heading_dev, camera_params.head_inc,
                              tilt, tilt_dev, camera_params.tilt_inc,
                              roll, roll_dev, camera_params.roll_inc);

  if (!volm_io::read_camera(best_cam_kml(), dms->ni(), dms->nj(), heading, heading_dev,
      tilt, tilt_dev, roll, roll_dev, tfov, top_fov_dev, altitude, lat, lon)) {
    std::cout << "problem parsing camera kml file: " << cam_kml()<< '\n';
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  cam_angles ca(roll, tfov, heading, tilt);
  unsigned int camindex = cam_space->closest_index(ca);

  std::cout<<"Ground truth Score is "<<gt_score<<std::endl;
  int gt_count = 0;
  int tot_count = 0;
  for (const auto & geo_folder : geo_folders)
  {
    for (unsigned i = 0; i < tiles.size(); i++) {
      volm_tile tile = tiles[i];
      // read in the volm_geo_index for tile i
      std::stringstream file_name_pre;
      file_name_pre << geo_folder << "geo_index_tile_" << i;
      // no index for tile i exists, continue
      if (!vul_file::exists(file_name_pre.str() + ".txt")) {
        continue;
      }
      float min_size;
      volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
      volm_geo_index::read_hyps(root, file_name_pre.str());
      std::vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(root, leaves);
      // load score binary from output folder if exists
      std::stringstream score_file;
      score_file << out() << "ps_0_scores_" << "tile_" << i << ".bin";

      // continue if no score binary exists for this tile
      if (!vul_file::exists(score_file.str()))
      {
        std::cout<<"Score file does not exist"<<std::endl;
        continue;
      }
      std::vector<volm_score_sptr> scores;
      volm_score::read_scores(scores, score_file.str());
      // refill the image
      unsigned total_ind = scores.size();
       for (unsigned ii = 0; ii < total_ind; ii++) {
        tot_count++;
        vgl_point_3d<double> h_pt = leaves[scores[ii]->leaf_id_]->hyps_->locs_[scores[ii]->hypo_id_];
        // look for location and camera giving maximum score
        if (scores[ii]->max_score_ > gt_score)
          gt_count ++;
        if (scores[ii]->max_score_ == gt_score)
        {
          bool flag = false;
          float min_diff = 1e10;
          int min_diff_id = -1;
          int mink = -1;
          for (unsigned k = 0 ; k < scores[ii]->cam_id_.size(); k++)
          {
            cam_angles camk = cam_space->camera_angles(scores[ii]->cam_id_[k]);
            float diff = camk.dif (ca);

            if ( diff < min_diff)
            {
              min_diff = diff;
              min_diff_id = scores[ii]->cam_id_[k];
              mink = k;
            }

            if (scores[ii]->cam_id_[k] == camindex )
            {
              std::cout<<"The rank of Pass 1's best camera is  "<<k<<std::endl;
              flag = true ;
            }
          }
          if (!flag)
              std::cout<<" Could not Find the best Camera"<<std::endl;
          std::cout<<"Cam id is "<<camindex<<" Min id is "<<min_diff_id<<" and the diff is "<<min_diff<<" rank  is "<<mink<<std::endl;
        }
      }
    } // end of tiles
  }// end of zpones

  std::cout<<"Rank of GT location is "<<gt_count<<" out of "<<tot_count<<std::endl;

  return volm_io::SUCCESS;
}
