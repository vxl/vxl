#include <testlib/testlib_test.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_io.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_save.h>
#include <bpgl/bpgl_camera_utils.h>

static void test_query()
{
  // input files
  vcl_string cam_bin_file = "D:\\work\\find\\volm_matcher\\test1\\local_output\\pa_5\\p1a_test1_40\\camera_space.bin";
  vcl_string dms_bin_file = "Z:\\projects\\FINDER\\test1\\p1a_test1_40\\p1a_test1_40_1.vsl";
  vcl_string sph_shell_file = "Z:\\projects\\FINDER\\index\\sph_shell_vsph_ca_180_pa_5_ta_75_ba_75.bin";
  
  // parameter for depth_map_interval
  float vmin = 2.0f;         // min voxel resolution
  float dmax = 3000.0f;      // maximum depth
  float solid_angle = 5.0f;
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle,vmin,dmax);

  // load the spherical_shell_container
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container;
  vsl_b_ifstream sph_ifs(sph_shell_file);
  sph_shell->b_read(sph_ifs);
  sph_ifs.close();

  // load cam_space 
  vsl_b_ifstream cam_ifs(cam_bin_file);
  volm_camera_space_sptr csp_in = new volm_camera_space;
  csp_in->b_read(cam_ifs);

  // create volm_query
  volm_query_sptr query = new volm_query(csp_in, dms_bin_file, sph_shell, sph);

  // screen output for query information
  unsigned total_size = query->obj_based_query_size_byte();
  vcl_cout << " For spherical surface, point angle = " << sph_shell->point_angle() << " degree, "
           << ", top_angle = " << sph_shell->top_angle() << " degree, "
           << ", bottom_angle = " << sph_shell->bottom_angle() << " degree, "
           << ", generated query has " << query->get_query_size() << " rays, "
           << query->get_cam_num() << " cameras:" << '\n'
           << " The query with " << query->get_cam_num() << " has " << (float)total_size/1024 << " Kbyte in total "
           << vcl_endl;

  // query check
  depth_map_scene_sptr dm = query->depth_scene();
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;

  if (dm->sky().size()) {
    vcl_cout << " -------------- SKYs -------------- " << vcl_endl;
    for (unsigned i = 0; i < dm->sky().size(); i++)
      vcl_cout << "\t name = " << (dm->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << (int)query->sky_orient()
               << ", land_id = " << dm->sky()[i]->land_id() 
               << ", land_name = " << volm_label_table::land_string(dm->sky()[i]->land_id())
               << vcl_endl;
  }
  if (dm->ground_plane().size()) {
    vcl_cout << " -------------- GROUND PLANE -------------- " << vcl_endl;
    for (unsigned i = 0; i < dm->ground_plane().size(); i++)
      vcl_cout << "\t name = " << dm->ground_plane()[i]->name()
               << ", depth = " << dm->ground_plane()[i]->min_depth()
               << ", orient = " << dm->ground_plane()[i]->orient_type()
               << ", land_id = " << dm->ground_plane()[i]->land_id() 
               << ", land_name = " << volm_label_table::land_string(dm->ground_plane()[i]->land_id())
               << vcl_endl;
  }
  if (dm->scene_regions().size()) {
    vcl_cout << " -------------- DEPTH REGIONS -------------- " << vcl_endl;
    for (unsigned i = 0; i < dm->scene_regions().size(); i++) {
      vcl_cout << "\t " <<  (dm->scene_regions())[i]->name()  << " region "
               << ",\t min_depth = " << (dm->scene_regions())[i]->min_depth()
               << ",\t max_depth = " << (dm->scene_regions())[i]->max_depth()
               << ",\t order = " << (dm->scene_regions())[i]->order()
               << vcl_endl;
    }
  }

  vcl_vector<depth_map_region_sptr> drs = query->depth_regions();
  vcl_cout << " The depth regions map inside query follows on order" << vcl_endl;
  if (drs.size()) {
    for (unsigned i = 0; i < drs.size(); i++) {
      vcl_cout << "\t " <<  drs[i]->name()  << " region "
               << ",\t min_depth = " << drs[i]->min_depth()
               << ",\t max_depth = " << drs[i]->max_depth()
               << ",\t order = " << drs[i]->order()
               << ",\t orient = " << drs[i]->orient_type()
               << ",\t land_id = " << drs[i]->land_id()
               << ",\t land_name = " << volm_label_table::land_string( drs[i]->land_id() )
               << vcl_endl;
    }
  }

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
  vcl_cout << " -------------- SKY --------------" << vcl_endl;
  for (unsigned i = 0; i < dm->sky().size(); ++i) {
    vcl_cout << "\t name = " << (dm->sky()[i]->name())
             << ", depth = " << 254
             << ", orient = " << dm->sky()[i]->orient_type()
             << ", NLCD_id = " << dm->sky()[i]->nlcd_id()
             << " ---> " << (int)volm_nlcd_table::land_id[dm->sky()[i]->nlcd_id()]
             << vcl_endl;
    }
  }
  if (dm->ground_plane().size()) {
    vcl_cout << " -------------- GROUND_PLANE ------" << vcl_endl;
    for (unsigned i = 0; i < dm->ground_plane().size(); ++i) {
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
    for (unsigned i = 0; i < dm->scene_regions().size(); ++i) {
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
    for (unsigned i = 0; i < drs.size(); ++i) {
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

  if (!query->write_query_binary(out_folder) ){
    vcl_cerr << "ERROR: write query binary file failed" << vcl_endl;
  }


  volm_query query_l;
  query_l.read_query_binary(out_folder);

  //TEST("number of rays for current query", query->get_query_size(), 29440); // for 2 degree resolution
#endif//NEED FILES TO RUN!! JLM

}


TESTMAIN(test_query);
