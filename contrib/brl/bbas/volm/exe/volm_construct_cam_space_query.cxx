//:
// \file
// \brief executable to read experiment parameter file and a camera kml file to create a camera space to be used by volumetric matchers
// Eliminates cameras not satisfying ground plane constraint if a ground plane
// is specified in the query depth map scene.
// Created camera_space binary is stored in local_output folder
//
// \author Ozge C. Ozcanli
// \date Jan 25, 2013
// \verbatim
//  Modifications
//   Yi Dong   Feb-2013   added the creation of an empty weight parameter text file
// \endverbatim
//
#include <ios>
#include <iomanip>
#include <iostream>
#include <algorithm>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_io.h>
#include <vul/vul_file.h>
#include <volm/volm_camera_space.h>
#include <vul/vul_arg.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>

int main(int argc, char** argv)
{
  // input
  vul_arg<std::string> cam_file("-cam", "cam kml filename", "");                                                        // query camera kml
  vul_arg<std::string> params_file("-params", "text file with the incremental params to construct camera space", "");   // query camera incremental file
  vul_arg<std::string> dms_file("-dms", "binary file with depth map scene", "");                                        // query label binary
  vul_arg<std::string> local_out_folder("-loc_out", "local output folder where the intermediate files are stored", ""); // output folder where camera binary will be stored
  vul_arg_parse(argc, argv);

  std::stringstream log;

  // check the input parameters
  if (cam_file().compare("") == 0 || dms_file().compare("") == 0 ||
      local_out_folder().compare("") == 0 || params_file().compare("") == 0)
  {
    std::cerr << " ERROR: input file/folders can not be empty!\n";
    volm_io::write_status(local_out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    vul_arg_display_usage_and_exit();
    volm_io::write_status(local_out_folder(), volm_io::EXE_ARGUMENT_ERROR, 0, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }


  // load the depth_map_scene
  if (!vul_file::exists(dms_file())) {
    log << "problem opening depth_map_scene binary file: " << dms_file() << '\n';
    std::cerr << log.str();
    volm_io::write_status(local_out_folder(), volm_io::DEPTH_SCENE_FILE_IO_ERROR, 0, log.str());
    return volm_io::DEPTH_SCENE_FILE_IO_ERROR;
  }

  vsl_b_ifstream dms_is(dms_file().c_str());
  depth_map_scene_sptr dm = new depth_map_scene;
  dm->b_read(dms_is);
  dms_is.close();

#if 0
  // screen output
  std::cout << " Loaded depth_map_scene info\n"
           << " image path = " << dm->image_path() << '\n'
           << " ----------------  SKY --------------------------\n";
  std::vector<depth_map_region_sptr> sky_reg = dm->sky();
  for (std::vector<depth_map_region_sptr>::iterator rit = sky_reg.begin(); rit != sky_reg.end(); ++rit)
    std::cout << "\tname = " <<  (*rit)->name() << "\t orient = " << (*rit)->orient_type() << "\t NLCD = " << (*rit)->land_id()
             << "\t order = " << (*rit)->order() << std::endl;
  std::cout << " ----------------  GROUND -----------------------\n";
  std::vector<depth_map_region_sptr> grd_reg = dm->ground_plane();
  for (std::vector<depth_map_region_sptr>::iterator rit = grd_reg.begin(); rit != grd_reg.end(); ++rit)
    std::cout << "\tname = " << (*rit)->name() << "\t orient_id = " << (*rit)->orient_type() << " orient = " << (*rit)->orient_string( (unsigned char)(*rit)->orient_type())
             << "\t land_id = " << (*rit)->land_id() << "\t land_name = " << volm_label_table::land_string( (unsigned char)(*rit)->land_id())
             << "\t order = " << (*rit)->order() << std::endl;
  std::cout << " ----------------- OBJECTS ----------------------\n";
  std::vector<depth_map_region_sptr> regs = dm->scene_regions();
  for (std::vector<depth_map_region_sptr>::iterator rit = regs.begin(); rit !=regs.end();++rit)
    std::cout << "name = " << (*rit)->name() << ",\t orient = " << (*rit)->orient_type() << "\t orient_string = " << (*rit)->orient_string( (unsigned char)(*rit)->orient_type())
             << "\t min_depth = " << (*rit)->min_depth() << ",\t order = " << (*rit)->order()
             << "\t land_id = " << (*rit)->land_id() << "\t land_name = " << volm_label_table::land_string( (unsigned char)(*rit)->land_id())
             << std::endl;
#endif

  // read the params
  if (!vul_file::exists(params_file())) {
    log << "problem opening camera incremental file: " << params_file() << '\n';
    std::cerr << log.str();
    volm_io::write_status(local_out_folder(), volm_io::EXE_ARGUMENT_ERROR, 0, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_io_expt_params params; params.read_params(params_file());

  // check camera input file
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double tfov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, tfov, top_fov_dev, altitude, lat, lon)) {
    log << "problem parsing camera kml file: " << cam_file() << '\n';
    std::cerr << log.str() << std::endl;
    volm_io::write_status(local_out_folder(), volm_io::CAM_FILE_IO_ERROR, 0, log.str());
    return volm_io::CAM_FILE_IO_ERROR;
  }
  std::cout << " create camera space from " << cam_file() << std::endl;
  if ( std::abs(heading-0) < 1E-10) heading = 180.0;
  std::cout << "cam params:"
           << "\n head: " << heading << " dev: " << heading_dev
           << "\n tilt: " << tilt << " dev: " << tilt_dev << " inc: " << params.head_inc
           << "\n roll: " << roll << " dev: " << roll_dev << " inc: " << params.roll_inc
           << "\n  fov: " << tfov << " dev: " << top_fov_dev << " inc: " << params.fov_inc
           << "\n  alt: " << altitude << std::endl;

  // construct camera space
  volm_camera_space cam_space(tfov, top_fov_dev, params.fov_inc, altitude, dm->ni(), dm->nj(),
                              heading, heading_dev, params.head_inc,
                              tilt, tilt_dev, params.tilt_inc,
                              roll, roll_dev, params.roll_inc);

  if (dm->ground_plane().size() > 0)  // enforce ground plane constraint if user specified a ground plane
  {
    camera_space_iterator cit = cam_space.begin();
    for ( ; cit != cam_space.end(); ++cit) {
      unsigned current = cam_space.cam_index();
      vpgl_perspective_camera<double> cam = cam_space.camera(); // camera at current state of iterator
      bool success = true;
      for (unsigned i = 0; success && i < dm->ground_plane().size(); i++)
        success = dm->ground_plane()[i]->region_ground_2d_to_3d(cam);
      if (success) // add this camera
        cam_space.add_camera_index(current);
    }
  }
  else
    cam_space.generate_full_camera_index_space();

  //cam_space.print_valid_cams();
  vsl_b_ofstream ofs(local_out_folder() + "camera_space.bin");
  cam_space.b_write(ofs);
  ofs.close();

  volm_io::write_status(local_out_folder(), volm_io::SUCCESS, 0, log.str());
  return volm_io::SUCCESS;
}
