//:
// \file
// \brief executable to read experiment parameter file and a camera kml file to create a camera space to be used by volumetric matchers
//        eliminates cameras not satisfying ground plane constraint if a ground plane is specified in the query depth map scene
//        created camera_space binary is stored in local_output folder
//
// \author Ozge C. Ozcanli
// \date Jan 25, 2013
// \verbatim
//   Modifications
//    Yi Dong   Feb-2013   added the creation of an empty weight parameter text file
// \endverbatim
//
#include <vcl_ios.h> // for vcl_ios_fixed etc.
#include <vcl_iomanip.h> // for vcl_fixed etc.
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
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");                                  // query camera kml
  
  vul_arg<vcl_string> params_file("-params", "text file with the incremental params to construct camera space", "");
  vul_arg<vcl_string> dms_file("-dms", "binary file with depth map scene", "");                // query labelme xml
  vul_arg<vcl_string> local_out_folder("-loc_out", "local output folder where the intermediate files are stored", "");
  vul_arg_parse(argc, argv);

  vcl_stringstream log;

  // check the input parameters
  if ( cam_file().compare("") == 0 || dms_file().compare("") == 0 ||
       local_out_folder().compare("") == 0 || params_file().compare("") == 0) {
    vcl_cerr << " ERROR: input file/folders can not be empty!\n";
    volm_io::write_status(local_out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    vul_arg_display_usage_and_exit();
    volm_io::write_status(local_out_folder(), volm_io::EXE_ARGUMENT_ERROR, 0, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }


  // load the depth_map_scene
  if (!vul_file::exists(dms_file())) {
    log << "problem opening depth_map_scene binary file: " << dms_file() << '\n';
    vcl_cerr << log.str();
    volm_io::write_status(local_out_folder(), volm_io::DEPTH_SCENE_FILE_IO_ERROR, 0, log.str());
    return volm_io::DEPTH_SCENE_FILE_IO_ERROR;
  }
  
  vsl_b_ifstream dms_is(dms_file().c_str());
  depth_map_scene_sptr dm = new depth_map_scene;
  dm->b_read(dms_is);
  dms_is.close();
  
#if 0
  // screen output
  vcl_cout << " Loaded depth_map_scene info" << vcl_endl;
  vcl_cout << " image path = " << dm->image_path() << vcl_endl;
  vcl_cout << " ----------------  SKY -------------------------- \n";
  vcl_vector<depth_map_region_sptr> sky_reg = dm->sky();
  for (vcl_vector<depth_map_region_sptr>::iterator rit = sky_reg.begin(); rit != sky_reg.end(); ++rit)
    vcl_cout << "\tname = " <<  (*rit)->name() << "\t orient = " << (*rit)->orient_type() << "\t NLCD = " << (*rit)->land_id()
             << "\t order = " << (*rit)->order() << vcl_endl;
  vcl_cout << " ----------------  GROUND ----------------------- \n";
  vcl_vector<depth_map_region_sptr> grd_reg = dm->ground_plane();
  for (vcl_vector<depth_map_region_sptr>::iterator rit = grd_reg.begin(); rit != grd_reg.end(); ++rit)
    vcl_cout << "\tname = " << (*rit)->name() << "\t orient_id = " << (*rit)->orient_type() << " orient = " << (*rit)->orient_string( (unsigned char)(*rit)->orient_type())
             << "\t land_id = " << (*rit)->land_id() << "\t land_name = " << volm_label_table::land_string( (unsigned char)(*rit)->land_id())
             << "\t order = " << (*rit)->order() << vcl_endl;
  vcl_cout << " ----------------- OBJECTS ----------------------\n";
  vcl_vector<depth_map_region_sptr> regs = dm->scene_regions();
  for(vcl_vector<depth_map_region_sptr>::iterator rit = regs.begin(); rit !=regs.end();++rit)
    vcl_cout << "name = " << (*rit)->name() << ",\t orient = " << (*rit)->orient_type() << "\t orient_string = " << (*rit)->orient_string( (unsigned char)(*rit)->orient_type())
             << "\t min_depth = " << (*rit)->min_depth() << ",\t order = " << (*rit)->order() 
             << "\t land_id = " << (*rit)->land_id() << "\t land_name = " << volm_label_table::land_string( (unsigned char)(*rit)->land_id())
             << vcl_endl;
#endif

  // read the params
  if (!vul_file::exists(params_file())) {
    log << "problem opening camera incremental file: " << params_file() << '\n';
    vcl_cerr << log.str();
    volm_io::write_status(local_out_folder(), volm_io::EXE_ARGUMENT_ERROR, 0, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_io_expt_params params; params.read_params(params_file());

  // check camera input file
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double tfov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, tfov, top_fov_dev, altitude, lat, lon)) {
    log << "problem parsing camera kml file: " << cam_file() << '\n';
    vcl_cerr << log.str() << vcl_endl;
    volm_io::write_status(local_out_folder(), volm_io::CAM_FILE_IO_ERROR, 0, log.str());
    return volm_io::CAM_FILE_IO_ERROR;
  }
  vcl_cout << " create camera space from " << cam_file() << vcl_endl;
  if ( vcl_abs(heading-0) < 1E-10) heading = 180.0;
  vcl_cout << "cam params:"
           << "\n head: " << heading << " dev: " << heading_dev
           << "\n tilt: " << tilt << " dev: " << tilt_dev << " inc: " << params.head_inc
           << "\n roll: " << roll << " dev: " << roll_dev << " inc: " << params.roll_inc
           << "\n  fov: " << tfov << " dev: " << top_fov_dev << " inc: " << params.fov_inc
           << "\n  alt: " << altitude << vcl_endl;

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

  // from loaded depth_map_scene, create an empty weight_parameter text file and store it in the local output folder
  vcl_string weight_file = local_out_folder() + "/weight_param.txt";
  vcl_ofstream ofs_weight(weight_file);
  ofs_weight << "Note: 1. for all objects, the summation of weight in the last col should be equal to 1\n";
  ofs_weight << "      2. for any objects, the summation of all weights from different attributes should be equal to 1\n\n";
  ofs_weight << "name                      type      orientation      land_class      min_distance      relative_order       obj_weight\n";
  ofs_weight.setf(vcl_ios::left);
  
  // sky weight
  if (dm->sky().size()) {
    ofs_weight.width(20);
    ofs_weight.fill(' ');
    ofs_weight << "sky" << "     sky\n";
  }
  
  // ground weight 
  if (dm->ground_plane().size()) {
    ofs_weight.width(20);
    ofs_weight.fill(' ');
    ofs_weight << "ground" << "     ground\n";
  }
  
  // object weight
  vcl_vector<depth_map_region_sptr> obj_reg = dm->scene_regions();
  for (vcl_vector<depth_map_region_sptr>::iterator rit = obj_reg.begin(); rit != obj_reg.end(); ++rit) {
    ofs_weight.width(20);
    ofs_weight.fill(' ');
    ofs_weight << (*rit)->name() << "     object\n";
  }
  ofs_weight.close();

#if 0
  // test binary I/O
  vcl_string cam_bin = local_out_folder() + "camera_space.bin";
  vsl_b_ifstream ifs(cam_bin.c_str());
  volm_camera_space_sptr csp_in = new volm_camera_space;
  csp_in->b_read(ifs);
  vcl_cout << " number of valid indics: " << (csp_in->valid_indices().size()) << vcl_endl;
  for (unsigned i = 0 ; i < csp_in->valid_indices().size(); i++) {
    cam_angles cam_ang = csp_in->camera_angles(i);
    vcl_cout << " first camera angle: heading = " << cam_ang.heading_ 
             << " tilt = " << cam_ang.tilt_ << " roll = " << cam_ang.roll_ << " tfov = " << cam_ang.top_fov_ << vcl_endl;
  }
#endif  
  volm_io::write_status(local_out_folder(), volm_io::SUCCESS, 0, log.str());
  return volm_io::SUCCESS;
}
