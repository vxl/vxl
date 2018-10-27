//:
// \file
// \executable to generate depth_map_scene, camera_space and volm_query for volm_matcher
// \author Yi Dong
// \date Apr 03, 2013

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
  vul_arg<std::string> cam_kml("-cam", "camera space binary", "");
  vul_arg<std::string> params_file("-params", "camera incremental param and parameters for depth interval", "");
  vul_arg<std::string> label_xml("-label", "labelme kml file", "");
  vul_arg<std::string> category_file("-cat", "category file for transferring labled type to land id", "");
  vul_arg<std::string> sph_bin("-sph", "spherical shell binary file", "");
  vul_arg<std::string> out_folder("-out", "output folder where the generate query binary, camspace binary and weight parameter stored", "");
  vul_arg<unsigned> id("-id", "job id", 100);
  vul_arg_parse(argc, argv);

  if (cam_kml().compare("") == 0 || params_file().compare("") == 0 ||
      label_xml().compare("") == 0 || category_file().compare("") == 0 ||
      sph_bin().compare("") == 0 || out_folder().compare("") == 0)
  {
    std::cerr << " ERROR: input file/folders can not be empty!\n";
    volm_io::write_status(out_folder(), volm_io::PRE_PROCESS_FAILED, 0);
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // create depth_map_scene from label me file
  if (!vul_file::exists(label_xml()) || !vul_file::exists(category_file())) {
    std::cerr << "problem opening labelme xml file or category file --> " << label_xml() << std::endl;
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR, 0);
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  depth_map_scene_sptr dm = new depth_map_scene;
  std::string img_category;
  if (!volm_io::read_labelme(label_xml(), category_file(), dm, img_category) ) {
    std::cerr << "problem parsing labelme xml file --> " << label_xml() << std::endl;
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR, 0);
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // save depth_map_scene as a binary
  std::string dms_bin_file = out_folder() + "/depth_map_scene.bin";
  vsl_b_ofstream ofs_dms(dms_bin_file);
  dm->b_write(ofs_dms);
  ofs_dms.close();

  // read the params
  if (!vul_file::exists(params_file())) {
    std::cerr << "problem opening camera incremental file --> " << params_file() << '\n';
    volm_io::write_status(out_folder(), volm_io::PRE_PROCESS_FAILED, 0);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_io_expt_params params;
  params.read_params(params_file());

  // create camera space and save it as binary for matcher
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double tfov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_kml(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, tfov, top_fov_dev, altitude, lat, lon)) {
    std::cerr << "problem parsing camera kml file --> " << cam_kml() << '\n';
    volm_io::write_status(out_folder(), volm_io::CAM_FILE_IO_ERROR, 0);
    return volm_io::CAM_FILE_IO_ERROR;
  }
  std::cout << " create camera space from " << cam_kml() << std::endl;
  if ( std::abs(heading-0) < 1E-10) heading = 180.0;
  std::cout << "cam params:"
           << "\n head: " << heading << " dev: " << heading_dev
           << "\n tilt: " << tilt << " dev: " << tilt_dev << " inc: " << params.head_inc
           << "\n roll: " << roll << " dev: " << roll_dev << " inc: " << params.roll_inc
           << "\n  fov: " << tfov << " dev: " << top_fov_dev << " inc: " << params.fov_inc
           << "\n  alt: " << altitude << std::endl;

  // construct camera space
  volm_camera_space_sptr cam_space = new volm_camera_space(tfov, top_fov_dev, params.fov_inc, altitude, dm->ni(), dm->nj(),
                                                           heading, heading_dev, params.head_inc,
                                                           tilt, tilt_dev, params.tilt_inc,
                                                           roll, roll_dev, params.roll_inc);

  if (dm->ground_plane().size() > 0)  // enforce ground plane constraint if user specified a ground plane
  {
    camera_space_iterator cit = cam_space->begin();
    for ( ; cit != cam_space->end(); ++cit) {
      unsigned current = cam_space->cam_index();
      vpgl_perspective_camera<double> cam = cam_space->camera(); // camera at current state of iterator
      bool success = true;
      for (unsigned i = 0; success && i < dm->ground_plane().size(); i++)
        success = dm->ground_plane()[i]->region_ground_2d_to_3d(cam);
      if (success) // add this camera
        cam_space->add_camera_index(current);
    }
  }
  else
    cam_space->generate_full_camera_index_space();

  //cam_space.print_valid_cams();
  std::string cam_bin_file = out_folder() + "/camera_space.bin";
  vsl_b_ofstream ofs_cam(cam_bin_file);
  cam_space->b_write(ofs_cam);
  ofs_cam.close();

  // create depth interval
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle,params.vmin,params.dmax);

  // load the spherical shell container
  if (!vul_file::exists(sph_bin())) {
    std::cerr << " ERROR: can not find spherical shell binary --> " << sph_bin() << std::endl;
    volm_io::write_status(out_folder(), volm_io::PRE_PROCESS_FAILED);
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container();
  vsl_b_ifstream ifs_sph(sph_bin());
  sph_shell->b_read(ifs_sph);
  ifs_sph.close();

  // create volm_query
  volm_query_sptr query = new volm_query(cam_space, dms_bin_file, sph_shell, sph);

  // save the volm_query
  std::string query_bin_file = out_folder() + "/volm_query.bin";
  vsl_b_ofstream ofs(query_bin_file);
  query->write_data(ofs);
  ofs.close();

  // screen output
  // sky
  depth_map_scene_sptr dmq = query->depth_scene();
  std::cout << " The " << dmq->ni() << " x " << dmq->nj() << " query image has following defined depth region" << std::endl;
  if (!dmq->sky().empty()) {
    std::cout << " -------------- SKYs --------------" << std::endl;
    for (unsigned i = 0; i < dmq->sky().size(); i++) {
      std::cout << "\t name = " << (dmq->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << (int)query->sky_orient()
               << ", land_id = " << dmq->sky()[i]->land_id()
               << ", land_name = " << volm_label_table::land_string(dmq->sky()[i]->land_id())
               << ", land_fallback_category = ";
      volm_fallback_label::print_id(dmq->sky()[i]->land_id());
      std::cout << ", land_fallback_weight = " ;
      volm_fallback_label::print_wgt(dmq->sky()[i]->land_id());
      std::cout << std::endl;
    }
  }

  // ground
  if (!dmq->ground_plane().empty()) {
    std::cout << " -------------- GROUND PLANE --------------" << std::endl;
    for (unsigned i = 0; i < dmq->ground_plane().size(); i++) {
      std::cout << "\t name = " << dmq->ground_plane()[i]->name()
                << ", depth = " << dmq->ground_plane()[i]->min_depth()
                << ", orient = " << dmq->ground_plane()[i]->orient_type()
                << ", land_id = " << dmq->ground_plane()[i]->land_id()
                << ", land_name = " << volm_label_table::land_string(dmq->ground_plane()[i]->land_id())
                << ", land_fallback = ";
      volm_fallback_label::print_id(dmq->ground_plane()[i]->land_id());
      std::cout << ", land_fallback_wgt = ";
      volm_fallback_label::print_wgt(dmq->ground_plane()[i]->land_id());
      std::cout << std::endl;
    }
  }

  std::vector<depth_map_region_sptr> drs = query->depth_regions();
  std::vector<std::vector<unsigned char> >& obj_land = query->obj_land_id();
  std::vector<std::vector<float> >& obj_land_wgt = query->obj_land_wgt();
  if (!drs.empty()) {
    std::cout << " -------------- NON GROUND/SKY OBJECTS --------------" << std::endl;
    for (unsigned i = 0; i < drs.size(); i++) {
      std::cout << "\t " <<  drs[i]->name()
               << " region,\t min_depth = " << drs[i]->min_depth()
               << ",\t max_depth = " << drs[i]->max_depth()
               << ",\t order = " << drs[i]->order()
               << ",\t orient = " << drs[i]->orient_type()
               << ",\t land_id = " << drs[i]->land_id()
               << ",\t land_name = " << volm_label_table::land_string( drs[i]->land_id() )
               << ",\t fallback_category = ";
      volm_fallback_label::print_id(drs[i]->land_id());
      std::cout << " (";
      for (unsigned char jj : obj_land[i])
        std::cout << volm_label_table::land_string(jj) << ", ";
      std::cout << " ),\t fallback_wgt = ";
      volm_fallback_label::print_wgt(drs[i]->land_id());
      std::cout << " (";
      for (float jj : obj_land_wgt[i])
        std::cout << jj << ' ';
      std::cout << ')' << std::endl;
    }
  }

  volm_io::write_status(out_folder(), volm_io::PRE_PROCESS_FINISHED, 30);
  return volm_io::PRE_PROCESS_FINISHED;
}
