#include <testlib/testlib_test.h>

#if HAS_OPENCL
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <bbas/volm/volm_query.h>
#include <bbas/volm/volm_query_sptr.h>
#include <boxm2/volm/boxm2_volm_obj_based_matcher.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <vul/vul_timer.h>
#include <bbas/volm/volm_io.h>
#include <bbas/volm/volm_tile.h>
#include <vil/vil_save.h>
#include <boxm2/volm/boxm2_volm_locations.h>
#include <boxm2/volm/boxm2_volm_locations_sptr.h>
#include <bbas/bocl/bocl_manager.h>
#include <bbas/bocl/bocl_device.h>
#include <vcl_set.h>
#include <vcl_iostream.h>


static void test_volm_obj_based_matcher()
{
  // setup input for container, queue, index etc..
  vcl_string label_file = "Y:\\job_160\\Result_USC.xml";
  vcl_string cam_file = "Y:\\job_160\\Camera_test.kml";
  vcl_string input_file = "W:\\projects\\FINDER\\world1_octree2\\integration\\coast_drainage_indices_50_50.txt";
  unsigned dev_id = 0;
  float buffer_capacity = 2.0;
  // check the labelme xml file
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_file, dm, img_category))
    vcl_cout << "volm_io::LABELME_FILE_IO_ERROR" << vcl_endl;

  // check the camera kml file
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double top_fov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_file, dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev,
                            roll, roll_dev, top_fov, top_fov_dev, altitude, lat, lon))
    vcl_cout << "volm_io::CAM_FILE_IO_ERROR" << vcl_endl;

  // read the parameters
  vcl_ifstream input_fs(input_file.c_str());
  vcl_vector<vcl_pair<vcl_string, vcl_string> > inp_files;
  while (!input_fs.eof()) {
    vcl_string region_type, hyp_file, ind_file;
    unsigned gpu_id;
    input_fs >> region_type;
    input_fs >> hyp_file;
    input_fs >> ind_file;
    input_fs >> gpu_id;
    if (region_type.compare("") == 0 || hyp_file.compare("") == 0 || ind_file.compare("") == 0)
      break;
    if (region_type.compare(img_category) != 0)
      continue;
    vcl_pair<vcl_string, vcl_string> p(hyp_file, ind_file);
    inp_files.push_back(p);
  }
  vcl_cout << "hyp_file = " << inp_files[0].first << "\nind_file = " << inp_files[0].second << vcl_endl;

  // create container
  boxm2_volm_wr3db_index_params q_params;
  if (!q_params.read_params_file(inp_files[0].second)) {
    vcl_cout << "ERROR: cannot read params file for " << inp_files[0].second << vcl_endl;
  }
  vcl_cout << " point_angle = " << q_params.point_angle << vcl_endl;
  volm_spherical_container_sptr sph = new volm_spherical_container(q_params.solid_angle, q_params.vmin, q_params.dmax);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, q_params.cap_angle, q_params.point_angle, q_params.top_angle, q_params.bottom_angle);
  vcl_cout << "index layer size: " << sph_shell->get_container_size() << vcl_endl;

  // create query
  vul_timer query_prep;
  volm_query_sptr query = new volm_query(cam_file, label_file, sph, sph_shell, false);
  vcl_cout << "\n==================================================================================================\n"
           << " 1. Creating query costs --------------------------------------------> " << query_prep.all()/1000.0 << " seconds.\n"
           << "\t The query has " << query->get_cam_num()
           << " cameras, " << query->depth_regions().size() + 2 << " labeled objects"
           << " and has resolution p_a = " << q_params.point_angle
           << ", top_a = " << q_params.top_angle
           << ", bottom_a = " << q_params.bottom_angle << '\n'
           << "==================================================================================================\n" << vcl_endl;
#if 0
  dm = query->depth_scene();
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;
  if (dm->sky().size()) {
    vcl_cout << " -------------- SKYs --------------" << vcl_endl;
    for (unsigned i = 0; i < dm->sky().size(); ++i)
      vcl_cout << "\t " << (dm->sky()[i]->name()) << " region ,\t min_depth = " << 255 << vcl_endl;
  }
  if (dm->ground_plane().size()) {
    vcl_cout << " -------------- GROUND PLANE --------------" << vcl_endl;
    for (unsigned i = 0; i < dm->ground_plane().size(); ++i)
      vcl_cout << "\t " << (dm->ground_plane()[i]->name()) << " region ,\t min_depth = " << 0 << vcl_endl;
  }
  if (dm->scene_regions().size()) {
    vcl_cout << " -------------- DEPTH REGIONS --------------" << vcl_endl;
    for (unsigned i = 0; i < dm->scene_regions().size(); ++i) {
      vcl_cout << "\t " <<  (dm->scene_regions())[i]->name()  << " region "
               << ",\t min_depth = " << (dm->scene_regions())[i]->min_depth()
               << ",\t max_depth = " << (dm->scene_regions())[i]->max_depth()
               << ",\t order = " << (dm->scene_regions())[i]->order()
               << vcl_endl;
    }
  }
#endif

  // create device
  bocl_manager_child_sptr mgr = bocl_manager_child::instance();
  if (dev_id >= (unsigned)mgr->numGPUs())
    vcl_cout << "volm_io::EXE_DEVICE_ID_ERROR" << vcl_endl;
  vcl_cout << "\n==================================================================================================\n"
           << " 2. Finding available devices for current platform --------> Use device " << mgr->gpus_[dev_id]->info().device_name_ << '\n'
           << "==================================================================================================\n" << vcl_endl;

  // create index
  boxm2_volm_wr3db_index_params params;
  unsigned long ei;
  if (!params.read_params_file(inp_files[0].second) ||
      !boxm2_volm_wr3db_index_params::read_size_file(inp_files[0].second, ei)) {
    vcl_cout << " cannot read size file for " << inp_files[0].second << vcl_endl;
  }
  if (!boxm2_volm_wr3db_index_params::query_params_equal(params, q_params))
    vcl_cout << " volm_query params for indices are not the same" << vcl_endl;

  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index((unsigned)(sph_shell->get_container_size()),buffer_capacity);
  ind->initialize_read(inp_files[0].second);
  vcl_cout << "\n==================================================================================================\n"
           << " 3. Initialize indices ------------------------------------> " << ei << " indices\n"
           << "==================================================================================================\n" << vcl_endl;

#if 0
  vcl_cout << " read in index from " <<  inp_files[0].second << " for checking purpose" << vcl_endl;
  boxm2_volm_wr3db_index_sptr ind_out = new boxm2_volm_wr3db_index((unsigned)(sph_shell->get_container_size()),buffer_capacity);
  ind_out->initialize_read(inp_files[0].second);

  vcl_vector<unsigned char> values(query->get_query_size());
  for (unsigned long i = 0; i < ei; ++i) {
    ind_out->get_next(values);
    for (unsigned long j = 0; j < values.size(); ++j) {
      vcl_cout << " INDEX_out " << i << ", values_out[" << j << "] = " << (int)values[j] << vcl_endl;
    }
  }
  ind_out->finalize();
#endif

  vcl_cout << "\n==================================================================================================\n"
           << " 4. Start the Object-Based matcher for " << ei << " indices and " << query->get_cam_num() << " cameras\n"
           << "==================================================================================================\n" << vcl_endl;

  boxm2_volm_obj_based_matcher matcher(query, ind, ei, mgr->gpus_[dev_id]);
  TEST("obj_based_matcher ", matcher.obj_based_matcher(), true);
}

#else // HAS_OPENCL
static void test_volm_obj_based_matcher() {}
#endif // HAS_OPENCL

TESTMAIN(test_volm_obj_based_matcher);