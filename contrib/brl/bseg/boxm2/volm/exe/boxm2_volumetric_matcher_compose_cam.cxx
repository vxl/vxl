//:
// \file
// \brief executable to check how many cameras are excluded and count the ROI
// \author Yi Dong
// \date Nov 27, 2012

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
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
#include <bbas/bocl/bocl_manager.h>
#include <bbas/bocl/bocl_device.h>
#include <vcl_set.h>
#include <vul/vul_timer.h>
#include <vpl/vpl.h>

int main(int argc,  char** argv)
{
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<vcl_string> input_file("-input", "text file with a list of hyp and index files to process", "");
  vul_arg<vcl_string> score_folder("-score", "score folder", "");  // composer will read the score files from this folder for this job
  vul_arg<vcl_string> out_folder("-out", "job output folder", "");
  vul_arg<float> thres("-t", "threshold to scale the score values to [1,254], piecewise linear s.t. [0,t) -> [1,127), [t,1] -> [127,254]", 0.5);
  vul_arg<unsigned> thres_unknown("-tu", "threshold to count # of pixels less than", 127);
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
  unsigned threshold_unknown = thres_unknown();
  vcl_cout << "composer using threshold: " << threshold << " to scale the value to [1,255]\n";

  // read the input hyp and index file pairs, only from the image category
  vcl_ifstream input_fs(input_file().c_str());
  vcl_vector<vcl_string> hyp_files;
  vcl_vector<vcl_string> index_files;
  vcl_vector<vcl_string> score_files;
  vcl_vector<vcl_string> cam_files;
  vcl_vector<vcl_string> index_score_files;
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
    index_score_files.push_back(score_folder() + "/" + ind_file + "_score_all_cam.bin");
  }

  if (!hyp_files.size()) {
    log << "cannot read any hyp index file pairs from: " << input_file() << " for category: " << img_category << "!\n";
    volm_io::write_composer_log(out_folder(), log.str());

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
  vcl_cout << "\n Start to check valid cameras for each index" << vcl_endl;

  // readin the values
  for (unsigned i = 0; i < score_files.size(); i++)
  {
    boxm2_volm_wr3db_index_params params;
    params.read_params_file(index_files[i]);
    unsigned long ind_size;
    boxm2_volm_wr3db_index_params::read_size_file(index_files[i], ind_size);


    unsigned long total_cam_num = ind_size * query->get_cam_num();
    unsigned long cnt_below = 0;

    // read and scale the scores from binary
    vcl_map<unsigned, vcl_vector<vcl_pair<unsigned, float> > > index_score;
    vsl_b_ifstream is(index_score_files[i].c_str());
    unsigned ind_id, cam_id;
    float score;
    for (unsigned jj = 0; jj < total_cam_num; jj++) {
      vsl_b_read(is, ind_id);
      vsl_b_read(is, cam_id);
      vsl_b_read(is, score);
      index_score[ind_id].push_back(vcl_pair<unsigned, float>(cam_id, score));
    }

    vcl_map<unsigned, vcl_vector<vcl_pair<unsigned, float> > >::iterator it = index_score.begin();
    for (; it != index_score.end(); ++it) {
      for (unsigned cam_id = 0; cam_id < it->second.size(); cam_id++) {
        float score = volm_io::scale_score_to_1_255(threshold, it->second[cam_id].second);
#if 0
        vcl_cout << " ind_id = " << it->first
                 << ", cam_id = " << it->second[cam_id].first
                 << ", score = " << it->second[cam_id].second << vcl_endl;
#endif
        if (score < threshold_unknown)
          cnt_below++;
      }
    }

    vcl_cout << "For index file " << index_files[i] << '\n'
             << "camera evaluated per location " << query->get_cam_num() << '\n'
             << "tot location evaluated: " << ind_size << '\n'
             << "tot cameras evaluated: " << total_cam_num << '\n'
             << "tot cameras below threshold (" << threshold << ", " << threshold_unknown << "): " << cnt_below << '\n'
             << "so knocked out " << (float)cnt_below/total_cam_num * 100 << " % of the of the evaluated CAMERAS!" << vcl_endl;
  }

  volm_io::write_composer_log(out_folder(), log.str());
  volm_io::write_status(out_folder(), volm_io::SUCCESS, 100);
  return volm_io::SUCCESS;
}

