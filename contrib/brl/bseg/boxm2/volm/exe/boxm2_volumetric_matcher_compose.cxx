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
#include <volm/volm_loc_hyp.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <boxm2/volm/boxm2_volm_matcher.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bbas/bocl/bocl_manager.h>
#include <bbas/bocl/bocl_device.h>
#include <vcl_set.h>
#include <vul/vul_timer.h>

// we want from highest score to lowest
bool top_matches_compare(const vcl_pair<float, unsigned>& p1, const vcl_pair<float, unsigned>& p2)
{
  return (p1.first > p2.first) ? true : false;
}

int main(int argc,  char** argv)
{
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<vcl_string> input_file("-input", "text file with a list of hyp and index files to process", "");
  vul_arg<vcl_string> score_folder("-score", "score folder", "");  // composer will read the score files from this folder for this job
  vul_arg<vcl_string> rat_folder("-rat", "rationale folder", "");  // composer will write top 30 to this folder
  vul_arg<vcl_string> out_folder("-out", "job output folder", "");
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
    cam_files.push_back(score_folder() + "/" + ind_file + "_cam.bin");
  }

  if (!hyp_files.size()) {
    log << "cannot read any hyp index file pairs from: " << input_file() << " for category: " << img_category << "!\n";
    volm_io::write_composer_log(out_folder(), log.str());

    // save images
    if (save_images()) {
      for (unsigned i = 0; i < out_imgs.size(); i++) {
        vcl_string out_name = out_folder() + "/VolM_" + tiles[i].get_string() + ".tif";
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
  volm_query_sptr query = new volm_query(cam_file(), label_file(), sph, sph_shell);

  vul_timer t;

  // check if all the score files are available, if not return error
  for (unsigned i = 0; i < score_files.size(); i++) {
    if (!vul_file::exists(score_files[i]) && !(vul_file::size(score_files[i]) > 0)) {
      vcl_cerr << " score file: " << score_files[i] << " is missing!\n";
      volm_io::write_status(out_folder(), volm_io::SCORE_FILE_MISSING, 100);
      return volm_io::SCORE_FILE_MISSING;
    }
  }

  volm_io::write_status(out_folder(), volm_io::COMPOSE_STARTED);
  // composer to generate final output
  vcl_cout << "\n Start to generate output images using tiles" << vcl_endl;


  // keep a map of scores to cam ids for top 30
  vcl_multiset<vcl_pair<float, unsigned>, std::greater<vcl_pair<float, unsigned> > > top_matches;
  vcl_multiset<vcl_pair<float, unsigned>, std::greater<vcl_pair<float, unsigned> > >::iterator top_matches_iter;

  vbl_array_2d<bool> mask;
  vbl_array_2d<float> kernel;
  volm_tile::get_uncertainty_region((float)10, (float)10, (float)0.01, mask, kernel);
  for (unsigned i = 0; i < score_files.size(); i++)
  {
    boxm2_volm_wr3db_index_params params;
    params.read_params_file(index_files[i]);
    unsigned long ind_size;
    boxm2_volm_wr3db_index_params::read_size_file(index_files[i], ind_size);

    //: read and scale the scores from binary
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
      vcl_cout << " for index " << i << ", score = " << score
               << ",\t camera id = " << cam_id
               << "\t with valid rays = " << query->get_valid_ray_num(cam_id)
               << vcl_endl;
    }

    //: read in the hypothese
    volm_loc_hyp hyp(hyp_files[i]);
    vcl_cout << hyp.size() << " hypotheses read from: " << hyp_files[i] << '\n'
             << scores.size() << " scores read from: " << score_files[i] << " params start: " << params.start << " skip: " << params.skip << vcl_endl;

    vgl_point_3d<float> h_pt;
    while (hyp.get_next(params.start, params.skip, h_pt))
    {
      unsigned ind_idx = hyp.current_ - params.skip;
#if 0
      vcl_cout << "Processing hypothesis: " << ind_idx << " x: " << h_pt.x() << " y: " << h_pt.y() << " z: " << h_pt.z()
               << ", score = " << scores[ind_idx] << ", best cam_id = " << cam_ids[ind_idx]
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
            current_score = (((float)out_imgs[k](u,v))-1.0f)/volm_io::SCALE_VALUE;
          if (scores[ind_idx] > current_score) {
            if (cam_ids[ind_idx] < query->get_cam_num()) {
              top_matches.insert(vcl_pair<float, unsigned>(scores[ind_idx], cam_ids[ind_idx]));
              vcl_cout << "inserting " << scores[ind_idx] << " cam id: " << cam_ids[ind_idx] << vcl_endl;
              if (top_matches.size() > 30) {
                top_matches_iter = top_matches.end();
                top_matches_iter--;
                top_matches.erase(top_matches_iter);
              }
            }
            else {
              vcl_stringstream log;
              log << "cam id: " << cam_id << " is invalid, query object has: " << query->get_cam_num() << " cams. In tile " << tiles[k].get_string() << " loc: (" << u << ", " << v << ") skipping rationale..\n"
                  << "score file is: " << score_files[i] << " id in the file: " << ind_idx << " hypo id: " << hyp.current_ << vcl_endl;
              vcl_cerr << log.str();
              volm_io::write_composer_log(out_folder(), log.str());
            }
            out_imgs[k](u,v) = (vxl_byte)(scores[ind_idx]*volm_io::SCALE_VALUE + 1);
            volm_tile::mark_uncertainty_region(u, v, scores[ind_idx], mask, kernel, out_imgs[k]);
          }
        }
      }
    }
  }

  // save images
  if (save_images()) {
    for (unsigned i = 0; i < out_imgs.size(); i++) {
      vcl_string out_name = out_folder() + "/VolM_" + tiles[i].get_string() + ".tif";
      vil_save(out_imgs[i], out_name.c_str());
    }
  }

  unsigned cnt = 0;
  vcl_cout << "creating rationale for top: " << top_matches.size() << vcl_endl;
  for (top_matches_iter = top_matches.begin(); top_matches_iter != top_matches.end(); top_matches_iter++) {
    vcl_string cam_postfix = query->get_cam_string(top_matches_iter->second);
    vcl_stringstream str;
    str << rat_folder() + "/" << "query_top_" << cnt++ << cam_postfix << ".png";
    vcl_cout << "writing rat to: " << str.str() << vcl_endl; vcl_cout.flush();
    query->draw_query_image(top_matches_iter->second,  str.str());
  }

  volm_io::write_status(out_folder(), volm_io::SUCCESS, 100);
  return volm_io::SUCCESS;
}

