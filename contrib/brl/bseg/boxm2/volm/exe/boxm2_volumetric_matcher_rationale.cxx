//:
// \file
// \brief executable to create rationale files for output of matching a query, only for debugging purposes for now
// \author Ozge C. Ozcanli
// \date Nov 18, 2012

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
#include <bbas/bocl/bocl_manager.h>
#include <bbas/bocl/bocl_device.h>
#include <vcl_set.h>
#include <vul/vul_timer.h>

#if 0
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<vcl_string> input_file("-input", "text file with a list of hyp and index files to process", "");
  vul_arg<vcl_string> score_folder("-score", "score folder", "");  // composer will read the score files from this folder for this job
  vul_arg<vcl_string> rat_folder("-rat", "rationale folder", "");  // composer will write top 30 to this folder
  vul_arg<vcl_string> out_folder("-out", "job output folder", "");
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
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
    cam_files.push_back(score_folder() + "/" + ind_file + "_camera.bin");
  }

  if (!hyp_files.size()) {
    vcl_cerr << "cannot read any hyp index file pairs from: " << input_file() << " for category: " << img_category << "!\n";
    return volm_io::SUCCESS;
  }

  // read the params of index 0, assume the container params are the same for all these indices
  boxm2_volm_wr3db_index_params q_params;
  if (!q_params.read_params_file(index_files[0])) {
    vcl_cerr << " cannot read params file for " << index_files[0] << '\n';
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // create query array
  // create containers
  volm_spherical_container_sptr sph = new volm_spherical_container(q_params.solid_angle,q_params.vmin,q_params.dmax);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, q_params.cap_angle, q_params.point_angle, q_params.top_angle, q_params.bottom_angle);
  volm_query_sptr query = new volm_query(cam_file(), label_file(), sph, sph_shell);

  vul_timer t;

  // read top 30 from rationale folder
  vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > > top_matches;
  vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > >::iterator iter;

  vcl_string top_matches_filename = rat_folder() + "/top_matches.txt";
  if (!volm_rationale::read_top_matches(top_matches, top_matches_filename)) {
    vcl_cerr << "cannot read " << top_matches_filename;
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // need to read one index at a time
  // go through top matches and figure out index file names
  vcl_map<vcl_string, vcl_map<unsigned, volm_rationale> > temp;
  for (iter = top_matches.begin(); iter != top_matches.end(); iter++)
  {
    vcl_map<vcl_string, vcl_map<unsigned, volm_rationale> >::iterator it = temp.find(iter->second.index_file);
    if (it == temp.end())
    {
      vcl_map<unsigned, volm_rationale> tmp;
      tmp[iter->second.index_id] = iter->second;
      temp[iter->second.index_file] = tmp;
    }
    else
    {
      it->second[iter->second.index_id] = iter->second;
    }
  }

  for (vcl_map<vcl_string, vcl_map<unsigned, volm_rationale> >::iterator it = temp.begin(); it != temp.end(); it++)
  {
    boxm2_volm_wr3db_index_params params;
    params.read_params_file(it->first);
    unsigned long ind_size;
    boxm2_volm_wr3db_index_params::read_size_file(it->first, ind_size);

    for (vcl_map<unsigned, volm_rationale>::iterator ii = it->second.begin(); ii != it->second.end(); ii++) {
      if (ii->first >= ind_size) {
        vcl_cerr << "problem here, index: " << ii->first << " is greater than size of index\n"
        return false;
      }
    }

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
                  << "score file is: " << score_files[i] << " id in the file: " << ind_idx << " hypo id: " << hyp.current_ << '\n';
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

  unsigned cnt = 0;
  vcl_cout << "creating rationale for top: " << top_matches.size() << vcl_endl;
  for (top_matches_iter = top_matches.begin(); top_matches_iter != top_matches.end(); top_matches_iter++) {
    vcl_string cam_postfix = query->get_cam_string(top_matches_iter->second);
    vcl_stringstream str;
    str << rat_folder() << "/query_top_" << cnt++ << cam_postfix << ".png";
    vcl_cout << "writing rat to: " << str.str() << vcl_endl; vcl_cout.flush();
    query->draw_query_image(top_matches_iter->second,  str.str());
  }

  volm_io::write_status(out_folder(), volm_io::SUCCESS, 100);
  return volm_io::SUCCESS;
}
#endif

// read a camera and labeling, generate the index image of this camera
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<vcl_string> input_file("-index", "binary index file", "");
  vul_arg<vcl_string> hyp_file("-hyp", "hypothesis file", "");
  vul_arg<vcl_string> out_folder("-out", "job output folder", "");
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
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

  volm_loc_hyp hyp(hyp_file());
  vcl_cout << hyp.size() << " hypotheses read from: " << hyp_file() << '\n';

  double lat, lon, head, head_d, tilt, tilt_d, roll, roll_d, tfov, tfov_d, altitude;
  // load the camera kml, fetch the camera value and deviation
  volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), head, head_d, tilt, tilt_d, roll, roll_d, tfov, tfov_d, altitude, lat, lon);
  vcl_cout << "from camera file, read location: " << lat << ' ' << lon << '\n';

  // figure out closest hypothesis
  vgl_point_3d<double> closest_h; unsigned closest_id;
  hyp.get_closest(lat, lon, closest_h, closest_id);
  vcl_cout << "closest hypothesis to camera location has id: " << closest_id << " its lat, lon is: " << closest_h.y() << ' ' << closest_h.x() << '\n'
           << "CAUTION: this exe assumes that index file has params start = 0 and skip = 1, so the index id will be the same as hypothesis id in the index file!\n";

  // read the params of index 0, assume the container params are the same for all these indices
  boxm2_volm_wr3db_index_params q_params;
  if (!q_params.read_params_file(input_file())) {
    vcl_cerr << " cannot read params file for " << input_file() << '\n';
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  if (q_params.start != 0 && q_params.skip != 1) {
    vcl_cerr << " index file: " << input_file() << " does not have start 0 and skip 1, cannot use this index! [for now..]!\n";
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_spherical_container_sptr sph = new volm_spherical_container(q_params.solid_angle,q_params.vmin,q_params.dmax);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, q_params.cap_angle, q_params.point_angle, q_params.top_angle, q_params.bottom_angle);
  unsigned layer_size = sph_shell->get_container_size();

  unsigned long ind_size;
  boxm2_volm_wr3db_index_params::read_size_file(input_file(), ind_size);
  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(layer_size, 1.0);
  ind->initialize_read(input_file());

  vcl_vector<unsigned char> values(layer_size, 0);
  // read index file and get to closest id
  for (unsigned indIdx = 0; indIdx <= closest_id; indIdx++) {
    // create index buffer and index bocl_mem
    ind->get_next(values);
  }

  // create query array
  // create containers
  volm_query_sptr query = new volm_query(cam_file(), label_file(), sph, sph_shell, false);
  vcl_cout << "query has: " << query->get_cam_num() << " cams!\n";
  vil_image_view<vil_rgb<vxl_byte> > out_img(dm->ni(), dm->nj());
  for (unsigned i = 0; i < dm->ni(); i++)
    for (unsigned j = 0; j < dm->nj(); j++) {
      out_img(i,j).r = (unsigned char)120;
      out_img(i,j).g = (unsigned char)120;
      out_img(i,j).b = (unsigned char)120;
    }
#if 0
    vcl_cout << " values array:\n";
    for (unsigned i = 0; i < layer_size; i++) {
      vcl_cout << (int)values[i] << ' ';
    }
    vcl_cout << vcl_endl;
#endif
  query->depth_rgb_image(values, 0, out_img);
  vcl_stringstream out_name; out_name << out_folder() << "/img_index_closest_id_" << closest_id << ".png";
  vil_save(out_img, out_name.str().c_str());
  vcl_stringstream out_name2;
  out_name2 << out_folder() << "/img_query_closest_id_" << closest_id << ".png";
  query->draw_query_image(0, out_name2.str().c_str());

  return volm_io::SUCCESS;
}
