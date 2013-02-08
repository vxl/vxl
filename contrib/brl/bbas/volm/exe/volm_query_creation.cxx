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
  vul_arg<vcl_string> in_folder("-in", "input folder where the camera space and spherical shell binary are stored", "");                                  // query camera kml
  vul_arg<vcl_string> params_file("-params", "text file with the params to construct camera space, spherical containers and query", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");                // query labelme xml
  vul_arg<vcl_string> out_folder("-out", "output folder where the generated query images are stored", "");
  vul_arg<bool> save_images("-save", "save out query images or not", false);
  vul_arg_parse(argc, argv);

  // check the input argument
  if(in_folder().compare("") == 0 || label_file().compare("") == 0 || params_file().compare("") == 0 || out_folder().compare("") == 0 ) {
    vcl_cerr << " ERROR: input file/folders can not be empty!\n";
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // read in the camera file
  vcl_string cam_space_bin = in_folder() + "camera_space.bin";
  if (!vul_file::exists(cam_space_bin)) {
    vcl_cerr << " ERROR: camera_space binary --> " << cam_space_bin << " can not be found!\n";
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vsl_b_ifstream is_cam(cam_space_bin);
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(is_cam);

  // read in the spherical shell container
  vcl_string sph_shell_fname = in_folder() + "sph_shell_container.bin";
  if (!vul_file::exists(sph_shell_fname)) {
    vcl_cerr << " ERROR: spherical shell binary --> " << sph_shell_fname << " cam not be found!\n";
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container();
  vsl_b_ifstream is_sph(sph_shell_fname);
  sph_shell->b_read(is_sph);

  vcl_cout << " For spherical surface, point angle = " << sph_shell->point_angle()*180/vnl_math::pi << " degree, "
           << ", top_angle = " << sph_shell->top_angle()*180/vnl_math::pi << " degree, "
           << ", bottom_angle = " << sph_shell->bottom_angle()*180/vnl_math::pi << " degree, " << vcl_endl;

  // create the depth interval from parameter file
  volm_io_expt_params params; params.read_params(params_file());
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle,params.vmin,params.dmax);

  // check the labelme xml
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_file(), dm, img_category)) {
    vcl_cerr << " ERROR: labelme xml file --> " << label_file() << " can not be used!\n";
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR);
    return volm_io::LABELME_FILE_IO_ERROR;
  }

  // create query
  volm_query_sptr query = new volm_query(cam_space, label_file(), sph, sph_shell);

  // screen output
  unsigned total_size = query->obj_based_query_size_byte();
  vcl_cout << " For spherical surface, point angle = " << sph_shell->point_angle()*180/vnl_math::pi << " degree, "
           << ", top_angle = " << sph_shell->top_angle()*180/vnl_math::pi << " degree, "
           << ", bottom_angle = " << sph_shell->bottom_angle()*180/vnl_math::pi << " degree, "
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

}
