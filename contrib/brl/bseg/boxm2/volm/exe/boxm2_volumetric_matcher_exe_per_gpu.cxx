//:
// \file
// \brief executable to match a given volumetric query and a camera estimate to an indexed reference volume
// \author Ozge C. Ozcanli
// \date Oct 8, 2012

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
#include <boxm2/volm/boxm2_volm_matcher_p0.h>
#include <boxm2/volm/boxm2_volm_matcher_order.h>
#include <bbas/bocl/bocl_manager.h>
#include <bbas/bocl/bocl_device.h>
#include <vcl_set.h>
#include <vul/vul_timer.h>

int main(int argc,  char** argv)
{
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<vcl_string> input_file("-input", "text file with a list of hyp and index files to process", "");
  vul_arg<float> buffer_capacity("-buff", "index buffer capacity (GB)",10.0f);
  vul_arg<bool> use_order("-order", "choose to use order matcher or not", false);
  vul_arg<vcl_string> out_folder("-out", "output folder", "");
  vul_arg<vcl_string> score_folder("-score", "score folder", "");  // composer will read the score files from this folder for this job
  vul_arg<unsigned> devID("-gpuid", "the idx for the 1st GPU, e.g. 0 for gpu0", 0);
  vul_arg<bool> logger("-logger", "designate one of the exes as logger", false); // if -logger exists then this one is logger exe is to do logging and generate the status.xml file
  vul_arg_parse(argc, argv);

  vcl_stringstream log;
  bool do_log = false;
  if (logger())
    do_log = true;
  vcl_cout << "argc: " << argc << vcl_endl;
  // check input parameters
  if (  cam_file().compare("") == 0 || label_file().compare("") == 0 ||
      input_file().compare("") == 0 || out_folder().compare("") == 0) {
    vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
    log << "EXE_ARGUMENT_ERROR!\n";
    if (do_log) volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // check the query input file
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_file(), dm, img_category)) {
    log << "problem parsing: " << label_file() << vcl_endl;
    if (do_log) { volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR); volm_io::write_log(out_folder(), log.str()); }
    vcl_cout << log.str() << vcl_endl;
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  log << "input image category: " << img_category << " size: " << dm->ni() << " x " << dm->nj() << " regions:" << vcl_endl;
  if (dm->sky().size())
    log << "\t sky region, min_depth = " << 254 << vcl_endl;
  if (dm->scene_regions().size()) {
    for (unsigned i = 0; i < dm->scene_regions().size(); i++) {
      log << "\t " <<  (dm->scene_regions())[i]->name()  << " region "
          << ",\t min_depth = " << (dm->scene_regions())[i]->min_depth()
          << ",\t max_depth = " << (dm->scene_regions())[i]->max_depth()
          << ",\t order = " << (dm->scene_regions())[i]->order()
          << vcl_endl;
     }
  }

  // check the camera input file
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double top_fov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, top_fov, top_fov_dev, altitude, lat, lon)) {
    if (do_log) { volm_io::write_status(out_folder(), volm_io::CAM_FILE_IO_ERROR);  volm_io::write_log(out_folder(), log.str()); }
    vcl_cout << log.str() << vcl_endl;
    return volm_io::CAM_FILE_IO_ERROR;
  }
  vcl_cout << "cam params\nheading: " << heading << " dev: " << heading_dev << "\ntilt: " << tilt << " dev: " << tilt_dev << "\nroll: " << roll << " dev: " << roll_dev << "\ntop_fov: " << top_fov << " dev: " << top_fov_dev << " alt: " << altitude << vcl_endl;
  // read the input hyp and index file pairs, only from the image category
  vcl_ifstream input_fs(input_file().c_str());
  vcl_vector<vcl_pair<vcl_string, vcl_string> > inp_files;
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
    if (gpu_id != devID())
      continue;

    vcl_pair<vcl_string, vcl_string> p(hyp_file, ind_file);
    inp_files.push_back(p);
  }

  if (!inp_files.size()) {
    vcl_cerr << "cannot read any hyp index file pairs from: " << input_file() << " for category: " << img_category << "!\n";
    log << "cannot read any hyp index file pairs from: " << input_file() << " for category: " << img_category << "!\n";
    if (do_log) {
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
      volm_io::write_log(out_folder(), log.str()); }
    vcl_cout << log.str() << vcl_endl;
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  log << "read " << inp_files.size() << " hyp-index pairs to process\n";

  if (do_log) volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_STARTED);

  vul_timer t;

  // define the devices that will be used
  // make bocl manager

  bocl_manager_child_sptr mgr = bocl_manager_child::instance();

  if (devID() >= (unsigned)mgr->numGPUs()) {
    vcl_cerr << " GPU id " << devID() << " is invalid, only " << mgr->numGPUs() <<" are available\n";
    log << " GPU id " << devID() << " is invalid, only " << mgr->numGPUs() <<" are available\n";
    if (do_log) {
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
      volm_io::write_log(out_folder(), log.str()); }
    vcl_cout << log.str();
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // read the params of index 0, assume the container params are the same for all these indices
  boxm2_volm_wr3db_index_params q_params;
  if (!q_params.read_params_file(inp_files[0].second)) {
    log << " cannot read params file for " << inp_files[0].second << vcl_endl;
    if (do_log) {
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
      volm_io::write_log(out_folder(), log.str()); }
    vcl_cout << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // create containers
  volm_spherical_container_sptr sph = new volm_spherical_container(q_params.solid_angle,q_params.vmin,q_params.dmax);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, q_params.cap_angle, q_params.point_angle, q_params.top_angle, q_params.bottom_angle);
  log << "index layer size: " << sph_shell->get_container_size() << vcl_endl;

  // create query array
  volm_query_sptr query = new volm_query(cam_file(), label_file(), sph, sph_shell);
  log << " generated query has size " << query->get_query_size() << '\n'
      << " The query has " << query->get_cam_num() << " cameras: " << '\n'
      << " Generated query_size for 1 camera is " << query->get_query_size() << " byte, "
      << " gives total query size = " << query->get_cam_num() << " x " << query->get_query_size()
      << " = " << (double)query->get_cam_num()*(double)query->get_query_size()/(1024*1024*1024) << " GB" << vcl_endl;

  if (query->get_cam_num() == 0) {
    log << "query has 0 cameras! Exiting!\n";
    if (do_log) {
      volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FAILED);
      volm_io::write_log(out_folder(), log.str()); }
    vcl_cout << log.str();
    return volm_io::MATCHER_EXE_FAILED;
  }

  for (unsigned ind_i = 0; ind_i < inp_files.size(); ind_i++) {
    boxm2_volm_wr3db_index_params params;
    // read the params of index
    unsigned long ei;
    if (!params.read_params_file(inp_files[ind_i].second) || !boxm2_volm_wr3db_index_params::read_size_file(inp_files[ind_i].second, ei)) {
      log << " cannot read size file for " << inp_files[ind_i].second << vcl_endl;
      if (do_log) {
        volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
        volm_io::write_log(out_folder(), log.str()); }
      vcl_cout << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    if (!boxm2_volm_wr3db_index_params::query_params_equal(params, q_params)) {
      log << " volm_query params for indices are not the same for " << inp_files[ind_i].second << " and " << inp_files[0].second << vcl_endl;
      if (do_log) {
        volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
        volm_io::write_log(out_folder(), log.str()); }
      vcl_cout << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    // load the hypothesis and create indices
    boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index((unsigned)(sph_shell->get_container_size()), buffer_capacity());
    ind->initialize_read(inp_files[ind_i].second);

    vcl_cout << "\n Starting the volm matcher for " << ei << "indices in " << inp_files[ind_i].second  << '\n'
             << log.str() << vcl_endl; // print log here

    // create the volm_matcher class
    if (use_order()) {
      vcl_cout << "\n using order matcher " << vcl_endl;
      boxm2_volm_matcher_order matcher(query, ind, ei, mgr->gpus_[devID()]);
      log << "\t using device: " << mgr->gpus_[devID()]->device_identifier() << vcl_endl;
      // get the score for all indexes
      if (do_log) volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_STARTED);
      if (!matcher.matching_cost_layer_order()) {
        log << " matcher exe failed for " << inp_files[ind_i].second << vcl_endl;
        if (do_log) {
          volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FAILED);
          volm_io::write_log(out_folder(), log.str());
        }
        vcl_cout << log.str();
        return volm_io::MATCHER_EXE_FAILED;
      }
      log << " Volm matcher finished" << vcl_endl;
      // save the scores
      vcl_string ind_pre = vul_file::strip_directory(inp_files[ind_i].second);
      ind_pre = vul_file::strip_extension(ind_pre);
      vcl_string out_prefix = score_folder() + "/" + ind_pre;
      matcher.write_score_order(out_prefix);
      vcl_string score_fname = score_folder() + "/" + ind_pre + "_score.bin";
      vcl_string cam_fname = score_folder() + "/" + ind_pre + "_camera.bin";
      vcl_cout << " Results are stored in file " << score_fname << " and " << cam_fname << vcl_endl;
    }
    else {
      boxm2_volm_matcher_p0 matcher(query, ind, ei, mgr->gpus_[devID()]);
      log << "\t using device: " << mgr->gpus_[devID()]->device_identifier() << vcl_endl;

      // get the score for all indexes
      if (do_log) volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_STARTED);

      if (!matcher.matching_cost_layer()) {
        log << " matcher exe failed for " << inp_files[ind_i].second << vcl_endl;
        if (do_log) {
          volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FAILED);
          volm_io::write_log(out_folder(), log.str());
        }
        vcl_cout << log.str();
        return volm_io::MATCHER_EXE_FAILED;
      }
      log << " Volm matcher finished" << vcl_endl;
      // save the scores
      vcl_string ind_pre = vul_file::strip_directory(inp_files[ind_i].second);
      ind_pre = vul_file::strip_extension(ind_pre);
      vcl_string out_prefix = score_folder() + "/" + ind_pre;
      matcher.write_score(out_prefix);
      vcl_string score_fname = score_folder() + "/" + ind_pre + "_score.bin";
      vcl_string cam_fname = score_folder() + "/" + ind_pre + "_camera.bin";
      vcl_cout << " Results are stored in file " << score_fname << " and " << cam_fname << vcl_endl;
    }
  }

  if (do_log) {
    volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FINISHED);
    volm_io::write_log(out_folder(), log.str());
  }
  return volm_io::SUCCESS;
}

