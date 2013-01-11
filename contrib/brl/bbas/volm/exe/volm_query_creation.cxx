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

  // create containers

  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle(),vmin(),dmax());
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, cap_angle(), point_angle(), top_angle(), bottom_angle());

  // create query array
  volm_query_sptr query = new volm_query(cam_file(), label_file(), sph, sph_shell);

  // screen output
  // query check
  dm = query->depth_scene();
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;
  if (dm->sky().size()) {
    for (unsigned i = 0; i < dm->sky().size(); i++)
      vcl_cout << "\t " << (dm->sky()[i]->name()) << " region ,\t min_depth = " << 255 << vcl_endl;
  }
  if (dm->ground_plane().size()) {
    for (unsigned i = 0; i < dm->ground_plane().size(); i++)
      vcl_cout << "\t " << (dm->ground_plane()[i]->name()) << " region ,\t min_depth = " << 0 << vcl_endl;
  }
  if (dm->scene_regions().size()) {
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
      vcl_cout << "\t " << drs[i]->name() << " region "
               << ",\t\t order = " << drs[i]->order()
               << ",\t min_dist = " << drs[i]->min_depth()
               << ",\t max_dist = " << drs[i]->max_depth()
               << ",\t orientation = " << drs[i]->orient_type()
               << vcl_endl;
    }
  }

  vcl_cout << " for spherical surface, point angle = " << point_angle() << " degree, "
           << ", top_angle = " << top_angle()
           << ", bottom_angle = " << bottom_angle()
           << ", generated query has size " << query->get_query_size() << '\n'

           << " The query has " << query->get_cam_num() << " cameras:" << '\n'
           << " Generated query_size for 1 camera is " << query->get_query_size() << " byte, "
           << " gives total query size = " << query->get_cam_num() << " x " << query->get_query_size()
           << " = " << (double)query->get_cam_num()*(double)query->get_query_size()/(1024*1024*1024) << " GB"
           << vcl_endl;

  // visualize query
  if (save_images()) {
    vcl_string prefix = out_folder();
    query->draw_query_images(out_folder());
  }

  return volm_io::SUCCESS;
}
