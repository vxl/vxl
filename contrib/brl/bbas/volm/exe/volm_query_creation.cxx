//:
// \file
// \brief query constructor to test generated query array from query image
// \author Yi Dong
// \date Novermber 01, 2012

#include <vul/vul_arg.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_io.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>

int main(int argc, char** argv)
{
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<float> vmin("-v", "minimum voxel size", 10.0f);
  vul_arg<float> dmax("-d", "maximum depth", 30000.0f);
  vul_arg<float> solid_angle("-sa", "solid angle (deg)", 2.0f);
  vul_arg<float> cap_angle("-ca", "cap angle(deg)", 180.0f);
  vul_arg<float> point_angle("-pa", "point angle(deg)", 10.0f);
  vul_arg<float> top_angle("-top", "top angle(deg)" , 0.0f);
  vul_arg<float> bottom_angle("-btm", "bottom angle(deg)", 0.0f);
  vul_arg<vcl_string> out_folder("-out", "output folder", "");
  vul_arg<int> save_images("-save", "save out query images or not", 0);

  vul_arg_parse(argc, argv);

  if (cam_file().compare("") == 0 || label_file().compare("") == 0 || out_folder().compare("") == 0) {
    vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // check the query input file
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_file(), dm, img_category)) {
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR);
    return volm_io::LABELME_FILE_IO_ERROR;
  }

  // check the camera input file
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev, altitude;
  double top_fov, top_fov_dev, lat, lon;
  if (!volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, top_fov, top_fov_dev, altitude, lat, lon)) {
    volm_io::write_status(out_folder(), volm_io::CAM_FILE_IO_ERROR);
    return volm_io::CAM_FILE_IO_ERROR;
  }
  vcl_cout << " heading = " << heading << ", heading_deviation = " << heading_dev << "\n"
           << "    tilt = " << tilt << ", tilt_deviation = " << tilt_dev << "\n" 
           << " top_fov = " << top_fov << ", top_fov_deviation = " << top_fov_dev << vcl_endl;
  // create containers
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle(),vmin(),dmax());
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, cap_angle(), point_angle(), top_angle(), bottom_angle());
  // create query
  volm_query_sptr query = new volm_query(cam_file(), label_file(), sph, sph_shell, false);
  // screen output
  unsigned total_size = query->obj_based_query_size_byte();
  vcl_cout << " For spherical surface, point angle = " << point_angle() << " degree, "
           << ", top_angle = " << top_angle()
           << ", bottom_angle = " << bottom_angle()
           << ", generated query has " << query->get_query_size() << " rays, "
           << query->get_cam_num() << " cameras:" << '\n'
           << " The query with " << query->get_cam_num() << " has " << (float)total_size/1024 << " Kbyte in total "
           << vcl_endl;
  // query check
  dm = query->depth_scene();
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;
  if (dm->sky().size()) {
    vcl_cout << " -------------- SKYs -------------- " << vcl_endl;
    for (unsigned i = 0; i < dm->sky().size(); i++)
      vcl_cout << "\t name = " << (dm->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << dm->sky()[i]->orient_type()
               << ", NLCD_id = " << dm->sky()[i]->nlcd_id() 
               << " ---> " << (int)volm_nlcd_table::land_id[dm->sky()[i]->nlcd_id()].first
               << vcl_endl;
  }
  if (dm->ground_plane().size()) {
    vcl_cout << " -------------- GROUND PLANE -------------- " << vcl_endl;
    for (unsigned i = 0; i < dm->ground_plane().size(); i++)
      vcl_cout << "\t name = " << dm->ground_plane()[i]->name()
               << ", depth = " << dm->ground_plane()[i]->min_depth()
               << ", orient = " << dm->ground_plane()[i]->orient_type()
               << ", NLCD_id = " << dm->ground_plane()[i]->nlcd_id() 
               << " ---> " << (int)volm_nlcd_table::land_id[dm->ground_plane()[i]->nlcd_id()].first
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
      vcl_cout << "\t " <<  (dm->scene_regions())[i]->name()  << " region "
               << ",\t min_depth = " << (dm->scene_regions())[i]->min_depth()
               << ",\t max_depth = " << (dm->scene_regions())[i]->max_depth()
               << ",\t order = " << (dm->scene_regions())[i]->order()
               << ",\t orient = " << (dm->scene_regions())[i]->orient_type()
               << ",\t NLCD_id = " << (dm->scene_regions())[i]->nlcd_id()
               << " ---> " << (int)volm_nlcd_table::land_id[dm->scene_regions()[i]->nlcd_id()].first
               << vcl_endl;
    }
  }
  // visualize query
  if (save_images()) {
    vcl_string prefix = out_folder();
    vcl_cout << " save the images in " << prefix << vcl_endl;
    query->draw_query_images(out_folder());
  }
#if 0
  // check the min_obj_dist and max_obj_dist
  vcl_vector<unsigned char> min_obj_dist = query->min_obj_dist();
  vcl_vector<unsigned char> max_obj_dist = query->max_obj_dist();
  vcl_vector<unsigned char> order_obj = query->order_obj();
  vcl_vector<unsigned char> obj_orient = query->obj_orient();
  vcl_vector<unsigned char> obj_nlcd = query->obj_nlcd();
  vcl_cout << " Check the min_obj and max_obj\n" << vcl_endl;
  for (unsigned i = 0; i < min_obj_dist.size(); i++) {
    vcl_cout << " i = " << i 
             << ", min_obj_dist = " << (int)min_obj_dist[i]
             << ", max_obj_dist = " << (int)max_obj_dist[i]
             << ", order_obj = " << (int)order_obj[i] 
             << ", orientation = " << (int)obj_orient[i]
             << ", nlcd = " << (int)obj_nlcd[i]
             << vcl_endl;
  }
  // check the ground_id and ground_dist
  vcl_vector<vcl_vector<unsigned> > ground_id = query->ground_id();
  vcl_vector<vcl_vector<unsigned char> > ground_dist = query->ground_dist();
  vcl_vector<vcl_vector<unsigned char> > ground_nlcd = query->ground_nlcd();
  unsigned char ground_orient = query->ground_orient();
  vcl_cout << " Check the ground plane dist and id\n";
  for (unsigned cam_id = 0; cam_id < query->get_cam_num(); cam_id++) {
    vcl_cout << " for camera " << cam_id << '\n';
    vcl_vector<unsigned> ground_id_layer = ground_id[cam_id];
    vcl_vector<unsigned char> ground_dist_layer = ground_dist[cam_id];
    vcl_vector<unsigned char> ground_nlcd_layer = ground_nlcd[cam_id];
    for (unsigned i = 0; i < ground_id_layer.size(); i++) {
      vcl_cout << "\t i = " << i 
               << ", ground_id = " << ground_id_layer[i]
               << ", ground_dist = " << (int)ground_dist_layer[i]
               << ", ground_nlcd = " << (int)ground_nlcd_layer[i]
               << ", ground_orient = " << (int)ground_orient
               << vcl_endl;
    }
  }
  // check sky_id
  vcl_vector<vcl_vector<unsigned> > sky_id = query->sky_id();
  vcl_cout << " Check the sky id\n";
  for (unsigned cam_id = 0; cam_id < query->get_cam_num(); cam_id++) {
    vcl_cout << " for camera " << cam_id << '\n';
    for (unsigned i = 0; i < sky_id[cam_id].size(); i++) {
      vcl_cout << "\t i = " << i << ", sky_id = " << sky_id[cam_id][i] << vcl_endl;
    }
  }
  // check dist_id
  vcl_vector<vcl_vector<vcl_vector<unsigned> > > dist_id = query->dist_id();
  vcl_cout << " total size of dist id = " << query->get_dist_id_size() << vcl_endl;
  for (unsigned cam_id = 0; cam_id < query->get_cam_num(); cam_id++) {
    vcl_cout << " for camera " << cam_id << ", size of dist_id = " << dist_id[cam_id].size() << vcl_endl;
    for (unsigned obj_id = 0; obj_id < query->depth_regions().size(); obj_id++) {
      for (unsigned i = 0; i < dist_id[cam_id][obj_id].size(); i++) {
        vcl_cout << "\t cam_id = " << cam_id
                 << " obj_id = " << obj_id
                 << " i = " << i
                 << " dist_id = " << dist_id[cam_id][obj_id][i] << vcl_endl;
      }
    }
  }
#endif

  return volm_io::SUCCESS;
}

