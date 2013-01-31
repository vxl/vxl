//:
// \file
// \brief executable to read experiment parameter file and a camera kml file to create a camera space to be used by volumetric matchers
//        eliminates cameras not satisfying ground plane constraint if a ground plane is specified in the query xml
//        also constructs spherical constrainer (TODO: and volm_query)
//        
//
//
//
//
// \author Ozge C. Ozcanli 
// \date Jan 25, 2013

#include <volm/volm_io.h>
//#include <vul/vul_file.h>
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
  vul_arg<vcl_string> params_file("-params", "text file with the params to construct camera space, spherical containers and query", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");                // query labelme xml
  vul_arg<vcl_string> out_folder("-out", "output folder where the results are stored", "");
  vul_arg<vcl_string> local_out_folder("-loc_out", "local output folder where the intermediate files are stored", "");
  vul_arg_parse(argc, argv);
  
  vcl_stringstream log;

  // check the input parameters
  if ( cam_file().compare("") == 0 || label_file().compare("") == 0 || 
       out_folder().compare("") == 0 || local_out_folder().compare("") == 0 || params_file().compare("") == 0) {
    vcl_cerr << " ERROR: input file/folders can not be empty!\n";
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    vul_arg_display_usage_and_exit();
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 0, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  
  // check the query input files
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  vcl_cout << label_file() << vcl_endl;
  if (!volm_io::read_labelme(label_file(), dm, img_category)) {
    log << "problem parsing: " << label_file() << vcl_endl;
    vcl_cerr << log.str() << vcl_endl;
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR, 0, log.str());
    return volm_io::LABELME_FILE_IO_ERROR;
  }

  // read the params
  volm_io_expt_params params; params.read_params(params_file());
  
  // check camera input file
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double tfov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, tfov, top_fov_dev, altitude, lat, lon)) {
    vcl_cerr << log.str() << vcl_endl;
    volm_io::write_status(out_folder(), volm_io::CAM_FILE_IO_ERROR, 0, log.str());
    return volm_io::CAM_FILE_IO_ERROR;
  }
  vcl_cout << "cam params\nheading: " << heading << " dev: " << heading_dev 
           << "\ntilt: " << tilt << " dev: " << tilt_dev 
           << "\nroll: " << roll << " dev: " << roll_dev 
           << "\ntop_fov: " << tfov << " dev: " << top_fov_dev 
           << " alt: " << altitude << vcl_endl;
  
  // construct camera space
  volm_camera_space cam_space(tfov, top_fov_dev, params.fov_inc, altitude, dm->ni(), dm->nj(), 
                              heading, heading_dev, params.head_inc, 
                              tilt, tilt_dev, params.tilt_inc,
                              roll, roll_dev, params.roll_inc);

  if (dm->ground_plane().size() > 0) {  // enforce ground plane constraint if user specified a ground plane

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

  } else 
    cam_space.generate_full_camera_index_space();
  
  //cam_space.print_valid_cams();

  vsl_b_ofstream ofs(local_out_folder() + "camera_space.bin");
  cam_space.b_write(ofs);
  ofs.close();
  
  // construct volm sphere 
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, params.cap_angle, params.point_angle, params.top_angle, params.bottom_angle);
  vsl_b_ofstream ofs2(local_out_folder() + "sph_shell_container.bin");
  sph_shell->b_write(ofs2);
  ofs2.close();

  log << "\n<query_category>" << img_category << "</query_category>\n";
  volm_io::write_status(out_folder(), volm_io::EXE_STARTED, 0, log.str());
  return volm_io::SUCCESS;
}