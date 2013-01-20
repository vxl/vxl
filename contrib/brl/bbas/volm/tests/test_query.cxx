#include <testlib/testlib_test.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_io.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_save.h>
#include <volm/volm_io.h>
//#include <volm/volm_query_pass2.h>
#include <bpgl/bpgl_camera_utils.h>

static void test_query()
{
  // create the depth interval using volm_spherical_container
  
  // parameter for coast
  float vmin = 2.0f;         // min voxel resolution
  float dmax = 3000.0f;      // maximum depth
  float solid_angle = 2.0f;
#if 0
  // parameters for desert
  float vmin = 5.0f;
  float dmax = 30000.0f;
  float solid_angle = 2.0f;
#endif

  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle,vmin,dmax);

  //: create spherical shell for query rays
  // parameter for coast
  float cap_angle = 180.0f;
  float point_angle = 2.0f;//for coast
  double radius = 1;
  float top_angle = 70.0f;
  float bottom_angle = 60.0f;
#if 0
  // parameter for desert
  float cap_angle = 180.0f;
  float point_angle = 2.0f;
  double radius = 1;
  float top_angle = 70.0f;
  float bottom_angle = 70.0f;
#endif
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(radius, cap_angle, point_angle, top_angle, bottom_angle);
#if 0 //files not available
  //: labelme file and camera files
  vcl_string out_folder = "D:\\work\\FINDER\\volumetric_algo\\queries\\coast\\job_160\\viewing_volume\\";
  vcl_string label_file = "D:\\work\\FINDER\\volm_matcher\\finderuploads\\coast\\job_160\\Result_USC_calibration_noFurther.xml";
  vcl_string cam_file =   "D:\\work\\FINDER\\volm_matcher\\finderuploads\\coast\\job_160\\Camera_test.kml";
  //: check the labelme file and camera file
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_file, dm, img_category)) {
    volm_io::write_status(out_folder, volm_io::LABELME_FILE_IO_ERROR);
  }
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev, altitude, lat, lon;
  double top_fov, top_fov_dev;
  if (!volm_io::read_camera(cam_file, dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, top_fov, top_fov_dev, altitude, lat, lon)) {
    volm_io::write_status(out_folder, volm_io::CAM_FILE_IO_ERROR);  
  }
#endif
#if 0
  vcl_string depth_scene_path = "c:/Users/mundy/VisionSystems/Finder/VolumetricQuery/Queries/p1a_res06_dirtroad_depthscene_v2.vsl";
  // create the query
  volm_query_sptr query = new volm_query(depth_scene_path, "desert", sph, sph_shell, 1.6);

  // query infomation
  vcl_cout << " for spherical surface, point angle = " << point_angle << " degree, "
           << ", top_angle = " << top_angle
           << ", bottom_angle = " << bottom_angle
           << ", number of rays = " << query->get_query_size()
           << "\n The query has " << query->get_cam_num() << " cameras:"
           << vcl_endl;
  // the depth_map_scene stored in query (can be used to fetch all attributes and polygons)
  depth_map_scene_sptr dm = query->depth_scene();  
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;
  if (dm->sky().size()) {
  vcl_cout << " -------------- SKY -------------- " << vcl_endl;
  for (unsigned i = 0; i < dm->sky().size(); i++) {
    vcl_cout << "\t name = " << (dm->sky()[i]->name())
             << ", depth = " << 254
             << ", orient = " << dm->sky()[i]->orient_type()
             << ", NLCD_id = " << dm->sky()[i]->nlcd_id() 
             << " ---> " << (int)volm_nlcd_table::land_id[dm->sky()[i]->nlcd_id()]
             << vcl_endl;
    }
  }
  if (dm->ground_plane().size()) {
    vcl_cout << " -------------- GROUND_PLANE ------ " << vcl_endl;
    for (unsigned i = 0; i < dm->ground_plane().size(); i++) {
      vcl_cout << "\t name = " << dm->ground_plane()[i]->name()
               << ", depth = " << dm->ground_plane()[i]->min_depth()
               << ", orient = " << dm->ground_plane()[i]->orient_type()
               << ", NLCD_id = " << dm->ground_plane()[i]->nlcd_id() 
               << " ---> " << (int)volm_nlcd_table::land_id[dm->ground_plane()[i]->nlcd_id()]
               << vcl_endl;
    }
  }
  if (dm->scene_regions().size()) {
    vcl_cout << " --------------- OBJECTS ------------------" << vcl_endl;
    for (unsigned i = 0; i < dm->scene_regions().size(); i++) {
      vcl_cout << "\t " <<  (dm->scene_regions())[i]->name()  << " region "
               << ",\t min_depth = " << (dm->scene_regions())[i]->min_depth()
               << ",\t max_depth = " << (dm->scene_regions())[i]->max_depth()
               << ",\t order = " << (dm->scene_regions())[i]->order()
               << ",\t orient = " << (dm->scene_regions())[i]->orient_type()
               << ",\t NLCD_id = " << (dm->scene_regions())[i]->nlcd_id()
               << " ---> " << (int)volm_nlcd_table::land_id[dm->scene_regions()[i]->nlcd_id()]
               << vcl_endl;
    }
  }

#if 0
  // attributes of all non_sky/non_ground objects
  vcl_vector<depth_map_region_sptr> drs = query->depth_regions();
  vcl_cout << " The depth regions map inside query follows on order" << vcl_endl;
  if (drs.size()) {
    for (unsigned i = 0; i < drs.size(); i++) {
      vcl_cout << "\t " << drs[i]->name() << " region "
               << ",\t\t order = " << drs[i]->order()
               << ",\t min_dist = " << drs[i]->min_depth()
			   << ",\t min_dist_interval = " << (int)sph->get_depth_interval(drs[i]->min_depth())
               << ",\t max_dist = " << drs[i]->max_depth()
			   << ",\t max_dist_interval = " << (int)sph->get_depth_interval(drs[i]->max_depth())
               << ",\t orientation = " << drs[i]->orient_type()
               << ",\t NLCD = " << drs[i]->nlcd_id()
               << " ---> " << (int)volm_nlcd_table::land_id[drs[i]->nlcd_id()]
               << vcl_endl;
    }
  }
#endif


  TEST("number of rays for current query", query->get_query_size(), 29440); // for 2 degree resolution
#endif//NEED FILES TO RUN!! JLM
}


TESTMAIN(test_query);
