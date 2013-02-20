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
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>
#include <vul/vul_file.h>

int main(int argc, char** argv)
{
  // input
  vul_arg<vcl_string> cam_bin("-cam", "camera space binary", "");                                       // query camera binary
  vul_arg<vcl_string> params_file("-params", "camera incremetal param file for depth interval", "");    // depth interval
  vul_arg<vcl_string> dms_bin("-dms", "xml file with labeled polygons", "");                            // query labelme xml
  vul_arg<vcl_string> sph_bin("-sph", "spherical shell binary file", "");                               // query spherical container
  vul_arg<vcl_string> out_folder("-out", "output folder where the generated query images are stored", ""); // options to save the query depth image
  vul_arg<bool> save_images("-save", "save out query images or not", false);
  vul_arg_parse(argc, argv);

  // check the input argument
  if(cam_bin().compare("") == 0 || dms_bin().compare("") == 0 || sph_bin().compare("") == 0 || params_file().compare("") == 0 || out_folder().compare("") == 0 ) {
    vcl_cerr << " ERROR: input file/folders can not be empty!\n";
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // read in the camera file
  if (!vul_file::exists(cam_bin())) {
    vcl_cerr << " ERROR: camera_space binary --> " << cam_bin() << " can not be found!\n";
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vsl_b_ifstream is_cam(cam_bin());
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(is_cam);
  is_cam.close();

  // read in the spherical shell container
  if (!vul_file::exists(sph_bin())) {
    vcl_cerr << " ERROR: spherical shell binary --> " << sph_bin() << " cam not be found!\n";
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container();
  vsl_b_ifstream is_sph(sph_bin());
  sph_shell->b_read(is_sph);
  is_sph.close();

  // create the depth interval from parameter file
  if (!vul_file::exists(params_file())) {
    vcl_cerr << " ERROR: camera incremental parameter file can not be found ---> " << params_file() << vcl_endl;
    volm_io::write_status(out_folder(),volm_io::EXE_ARGUMENT_ERROR);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_io_expt_params params; 
  params.read_params(params_file());
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle,params.vmin,params.dmax);

  // load the depth map scene
  if (!vul_file::exists(dms_bin())) {
    vcl_cerr << " ERROR: depth map scene binary can not be found ---> " << dms_bin() << vcl_endl;
    volm_io::write_status(out_folder(), volm_io::DEPTH_SCENE_FILE_IO_ERROR);
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // create query
  volm_query_sptr query = new volm_query(cam_space, dms_bin(), sph_shell, sph);

  // screen output
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
      vcl_cout << "\t " <<  (dm->scene_regions())[i]->name()  << " region "
               << ",\t min_depth = " << (dm->scene_regions())[i]->min_depth()
               << ",\t max_depth = " << (dm->scene_regions())[i]->max_depth()
               << ",\t order = " << (dm->scene_regions())[i]->order()
               << ",\t orient = " << (dm->scene_regions())[i]->orient_type()
               << ",\t land_id = " << (dm->scene_regions())[i]->land_id()
               << ",\t land_name = " << volm_label_table::land_string( (dm->scene_regions())[i]->land_id() )
               << vcl_endl;
    }
  }
  // visualize query
  if (save_images()) {
    vcl_string prefix = out_folder();
    vcl_cout << " save the images in " << prefix << vcl_endl;
    query->draw_query_images(out_folder());
  }

  volm_io::write_status(out_folder(), volm_io::SUCCESS);
  return volm_io::SUCCESS;
}
