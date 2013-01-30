//:
// \file
// \brief executable to match a given volumetric query and a camera estimate to an indexed reference volume using different matcher
// \author Yi Dong
// \date Jan 19, 2013

#include <volm/volm_io.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <boxm2/volm/boxm2_volm_matcher_p1.h>
#include <bbas/bocl/bocl_manager.h>
#include <bbas/bocl/bocl_device.h>

int main(int argc, char** argv)
{
  // input
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");                                  // query camera kml
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");                // query labelme xml
  vul_arg<vcl_string> geo_index_folder("-geo", "folder to read the geo index and the hypo", ""); // folder to read the geo_index and hypos for each leaf
  vul_arg<vcl_string> candidate_list("-cand", "candidate list for given query (txt file)", "");  // candidate list file containing polygons
  vul_arg<float>      buffer_capacity("-buff", "index buffer capacity (GB)", 1.0f);
  vul_arg<unsigned>   tile_id("-tile", "ID of the tile that current matcher consider",30);
  vul_arg<unsigned>   dev_id("-gpuid", "device used for current matcher", 0);
  vul_arg<vcl_string> out_folder("-out", "output folder where store the results", "");
  vul_arg<bool>        use_ps0("-ps0", "choose to use pass 0 regional matcher", false);
  vul_arg<bool>        use_ps1("-ps1", "choose to use pass 1 obj_based order matcher", false);
  vul_arg<bool>        use_ps2("-ps2", "choose to use pass 2 obj_based orient matcher", false);
  vul_arg<bool>       logger("-logger", "designate one of the exes as logger", false); // if -logger exists then this one is logger exe is to do logging and generate the status.xml file

  vul_arg_parse(argc, argv);

  vcl_stringstream log;
  bool do_log = false;
  if (logger())
    do_log = true;
  vcl_cout << "argc: " << argc << vcl_endl;
  // check the input parameters
  if ( cam_file().compare("") == 0 || label_file().compare("") == 0 ||
       geo_index_folder().compare("") == 0 || out_folder().compare("") == 0) {
    vcl_cerr << " ERROR: input file/folders can not be empty!\n";
    log << "EXE_ARGUMENT_ERROR!\n";
    if (do_log) volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  
  
  // check the query input files
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  vcl_cout << label_file() << vcl_endl;
  if (!volm_io::read_labelme(label_file(), dm, img_category)) {
    
    log << "problem parsing: " << label_file() << vcl_endl;
    vcl_cerr << log.str() << vcl_endl;
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  
  // check camera input file
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double top_fov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, top_fov, top_fov_dev, altitude, lat, lon)) {
    log << "problem parsing: " << cam_file() << '\n';
    if (do_log) { volm_io::write_log(out_folder(), log.str()); }
    volm_io::write_status(out_folder(), volm_io::CAM_FILE_IO_ERROR);
    vcl_cerr << log.str() << vcl_endl;
    return volm_io::CAM_FILE_IO_ERROR;
  }
  vcl_cout << "cam params\nheading: " << heading << " dev: " << heading_dev 
           << "\ntilt: " << tilt << " dev: " << tilt_dev 
           << "\nroll: " << roll << " dev: " << roll_dev 
           << "\ntop_fov: " << top_fov << " dev: " << top_fov_dev 
           << " alt: " << altitude << vcl_endl;
  
  // To be decided, ---> based on initial camera space, how to create a global camera space for all passes
  // TO BE IMPLEMENTED

  // read the geo_index, create geo_index tree, read in parameters
  vcl_stringstream file_name_pre;
  file_name_pre << geo_index_folder() << "geo_index_tile_" << tile_id();
  vcl_cout << " geo_index_hyps_file = " << file_name_pre.str() + ".txt" << vcl_endl;

  if (!vul_file::exists(file_name_pre.str() + ".txt")) {
    log << " ERROR: gen_index_folder is wrong (missing last slash/ ?), no geo_index_files found in " << geo_index_folder() << vcl_endl;
    if (do_log) { volm_io::write_log(out_folder(), log.str()); }
    vcl_cerr << log.str() << vcl_endl;
    volm_io::write_status(out_folder(), volm_io::GEO_INDEX_FILE_MISSING);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());

  // check whether we have candidate list for this query
  bool is_candidate = false;
  vgl_polygon<double> cand_poly;
  vcl_cout << " candidate list = " <<  candidate_list() << vcl_endl;
  
  if ( candidate_list().compare("") != 0) {
    if ( vul_file::extension(candidate_list()).compare(".txt") == 0) {
      is_candidate = true;
      volm_io::read_polygons(candidate_list(), cand_poly);
    } else {
      log << " ERROR: candidate list exist but with wrong format, only txt allowed" << candidate_list() << '\n';
      if (do_log) {
        volm_io::write_composer_log(out_folder(), log.str()); 
      }
      vcl_cerr << log;
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
      return volm_io::EXE_ARGUMENT_ERROR;
    }
  }
  // prune the tree, only leaves with non-zero hypos are left
  if (is_candidate) {
    volm_geo_index::prune_tree(root, cand_poly);
  }
  vcl_vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves); 
  
  // read in the parameter, create depth_interval table and spherical shell container
  boxm2_volm_wr3db_index_params params;
  vcl_string index_file = leaves[0]->get_index_name(file_name_pre.str());
  
  if (!params.read_params_file(index_file)) {
    log << " ERROR: cannot read params file from " << index_file << '\n';
    if (do_log) {
       volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
       volm_io::write_log(out_folder(), log.str()); 
    }
    vcl_cerr << log.str() << vcl_endl;
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, params.cap_angle, params.point_angle, params.top_angle, params.bottom_angle);
  unsigned layer_size = (unsigned)sph_shell->get_container_size();
  // construct depth_interval table for pass 1 matcher
  vcl_map<double, unsigned char>& depth_interval_map = sph->get_depth_interval_map();
  vcl_vector<float> depth_interval;
  vcl_map<double, unsigned char>::iterator iter = depth_interval_map.begin();
  for (; iter != depth_interval_map.end(); ++iter)
    depth_interval.push_back((float)iter->first);

  // create index vector
#if 0
  vcl_cout << "\n==================================================================================================\n";
  vcl_cout << "\t  1. Load indeice based on geo_index_tree (and candidate list)\n";
  vcl_cout << "\t  point_angle = " << params.point_angle << ", top_angle = " << params.top_angle
           << ", bottom_angle = " << params.bottom_angle << ", index_layer_size = " << layer_size << '\n';
  vcl_cout << "==================================================================================================\n " << vcl_endl;
  vcl_vector<boxm2_volm_wr3db_index_sptr> ind_vec;
  for (unsigned i = 0; i < leaves.size(); i++) {
    if(!leaves[i]->hyps_)
      continue;
    boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(layer_size, buffer_capacity());
    vcl_string index_file = leaves[i]->get_index_name(file_name_pre.str());
    if(!ind->initialize_read(index_file)) {
      log << " ERROR: cannot load index from " << index_file << '\n';
      if (do_log) {
        volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
        volm_io::write_log(out_folder(), log.str());
      }
      vcl_cerr << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    ind_vec.push_back(ind);
  }
  if (ind_vec.size() != leaves.size()) {
    log << " create indices for every leaf failed " << vcl_endl;
    if (do_log) {
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
      volm_io::write_log(out_folder(), log.str());
    }
    vcl_cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
#endif

  
  // create query
  volm_query_sptr query = new volm_query(cam_file(), label_file(), sph, sph_shell, false);
  // screen output of query
  unsigned total_size = query->obj_based_query_size_byte();
  vcl_cout << "\n==================================================================================================\n";
  vcl_cout << "\t\t  2. Create query from given camera space and Labelme geometry\n";
  vcl_cout << "\t\t  generate query has " << query->get_cam_num() << " cameras "
           << " and " << (float)total_size/1024 << " Kbyte in total\n";
  vcl_cout << "==================================================================================================\n " << vcl_endl;
  dm = query->depth_scene();
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;
  if (dm->sky().size()) {
    vcl_cout << " -------------- SKYs -------------- " << vcl_endl;
    float sky_weight = query->sky_weight();
    for (unsigned i = 0; i < dm->sky().size(); i++)
      vcl_cout << "\t name = " << (dm->sky()[i]->name())
               << ",\t depth = " << 254
               << ",\t orient = " << (int)query->sky_orient()
               << ",\t weight = " << sky_weight
               << vcl_endl;
  }
  if (dm->ground_plane().size()) {
    vcl_cout << " -------------- GROUND PLANE -------------- " << vcl_endl;
    for (unsigned i = 0; i < dm->ground_plane().size(); i++)
      vcl_cout << "\t name = " << dm->ground_plane()[i]->name()
               << ",\t depth = " << dm->ground_plane()[i]->min_depth()
               << ",\t orient = " << dm->ground_plane()[i]->orient_type()
               << ",\t NLCD_id = " << dm->ground_plane()[i]->nlcd_id() 
               << " ---> " << (int)volm_nlcd_table::land_id[dm->ground_plane()[i]->nlcd_id()].first
               << ",\t weight = " << query->grd_weight()
               << vcl_endl;
  }
  vcl_vector<depth_map_region_sptr> dmr = query->depth_regions();
  if (dmr.size()) {
    vcl_cout << " -------------- DEPTH REGIONS -------------- " << vcl_endl;
    vcl_vector<float>& obj_weight = query->obj_weight();
    for (unsigned i = 0; i < dmr.size(); i++) {
      vcl_cout << "\t\t " <<  dmr[i]->name()  << " region "
               << ",\t\t min_depth = " << dmr[i]->min_depth()
               << " ---> interval = " << (int)sph->get_depth_interval(dmr[i]->min_depth())
               << ",\t\t max_depth = " << dmr[i]->max_depth()
               << ",\t\t order = " << dmr[i]->order()
               << ",\t\t orient = " << dmr[i]->orient_type()
               << ",\t\t NLCD_id = " << dmr[i]->nlcd_id()
               << " ---> " << (int)volm_nlcd_table::land_id[dmr[i]->nlcd_id()].first
               << " weight = " << obj_weight[i]
               << vcl_endl;
    }
  }
  
  // define the device that will be used
  
  bocl_manager_child_sptr mgr = bocl_manager_child::instance();
  if (dev_id() >= (unsigned)mgr->numGPUs()) {
    log << " GPU is " << dev_id() << " is invalid, only " << mgr->numGPUs() << " are available\n";
    if (do_log) {
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
      volm_io::write_log(out_folder(), log.str());
    }
    vcl_cerr << log.str();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << "\n==================================================================================================\n";
  vcl_cout << "\t\t  3. Following device is used for volm_matcher\n";
  vcl_cout << "\t\t  " << mgr->gpus_[dev_id()]->info() << '\n';
  vcl_cout << "==================================================================================================\n " << vcl_endl;


  vcl_cout << "\n==================================================================================================\n";
  vcl_cout << "\t\t  4. Start volumetric matching with following matchers\n";
  vcl_cout << "==================================================================================================\n " << vcl_endl;

  // to be implemented, what kind of infomation needs for pass 0 and what the output will be
  bool is_last_pass = false;
  if (use_ps0()) {
    vcl_cout << " we will use pass 0, i.e. regional matcher... TO be implemented " << vcl_endl;
    is_last_pass = true;
  } else {
    vcl_cout << " regional matcher (pass 0) is avoided " << vcl_endl;
  }

  
  // start pass 1 matcher
  if (use_ps1()) {
    boxm2_volm_matcher_p1 obj_order_matcher(query, leaves, buffer_capacity(), geo_index_folder(), tile_id(),
                                            depth_interval, cand_poly, mgr->gpus_[dev_id()], is_candidate, is_last_pass, out_folder());
    if(! obj_order_matcher.volm_matcher_p1()) {
      log << " ERROR: pass 1 volm_matcher failed for geo_index " << index_file << '\n';
      if (do_log) {
        volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FAILED);
        volm_io::write_log(out_folder(), log.str()); 
      }
      vcl_cerr << log.str() << vcl_endl;
      return volm_io::MATCHER_EXE_FAILED;
    }
    // output will be a probability map
    vcl_cout << "\n==================================================================================================\n";
    vcl_cout << "\t\t  5. Generate output for pass 1 matcher and store it in\n";
    vcl_cout << "\t\t     " << out_folder() << vcl_endl;
    vcl_cout << "==================================================================================================\n " << vcl_endl;
    vcl_stringstream out_fname_pre;
    out_fname_pre << out_folder() << "geo_index_tile_" << tile_id();
    bool good = obj_order_matcher.write_matcher_result(out_fname_pre.str());


  } else {
    vcl_cout << " object based depth/order matcher (pass 1) is avoided" << vcl_endl;
  }

  

  // start pass 2 matcher
  if (use_ps2()) {
    vcl_cout << " we will use pass 2, i.e. object based, ray based ORIENT/NLCD matcher " << vcl_endl;
    vcl_cout << " input: query, index, leaves, candidate list(is_candidate), depth_interval" << vcl_endl;
    vcl_cout << " NEED TO CHECK WHETHEER WE HAVE PASS 0 MATCHER RESULT, IF SO, LOAD THE REDUCED SPACE FROM PASS 0" << vcl_endl;
  } else {
    vcl_cout << " object based orientation/land classification matcher (pass 2) is avoided" << vcl_endl;
  }

  

  return volm_io::SUCCESS;
}