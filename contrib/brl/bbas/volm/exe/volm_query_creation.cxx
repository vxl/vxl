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
#include <vpgl/vpgl_perspective_camera.h>

int main(int argc, char** argv)
{
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");
  vul_arg<unsigned> img_ni("-ni", "query img ni", 0);
  vul_arg<unsigned> img_nj("-nj", "query img nj", 0);
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<float> vmin("-v", "minimum voxel size", 10.0f);
  vul_arg<float> dmax("-d", "maximum depth", 60000.0f);
  vul_arg<float> solid_angle("-sa", "solid angle (deg)", 2.0f);
  vul_arg<double> cap_angle("-ca", "cap angle(deg)", 360.0);
  vul_arg<double> point_angle("-pa", "point angle(deg)", 10.0);
  vul_arg<double> init_focal("-f", "focal initial", 1000.0);
  vul_arg<double> conf_focal("-cf", "focal confidence", 0.0);
  vul_arg<double> init_heading("-h", "heading initial", 180.0);
  vul_arg<double> conf_heading("-ch", "heading confidence", 0.0);
  vul_arg<double> init_tilt("-t", "tilt initial", 90.0);
  vul_arg<double> conf_tilt("-ct", "tilt confidence", 0.0);
  vul_arg<double> init_roll("-r", "roll initial", 0.0);
  vul_arg<double> conf_roll("-cr", "roll confidence", 0.0);
  vul_arg<double> altitude("-alt", "altitude", 1.6);
  vul_arg<vcl_string> out_folder("-out", "output folder", "");
  vul_arg<bool> save_images("-save", "save out query images or not", false);
  
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  
  if (img_ni() == 0 || img_nj() == 0 || cam_file().compare("") == 0 || label_file().compare("") == 0 || out_folder().compare("") == 0) {
    vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  
  
  //: load the camera
  vpgl_perspective_camera<double> cam;
  if (!volm_io::read_camera(cam_file(), cam, img_ni(), img_nj())) {
    volm_io::write_status(out_folder(), volm_io::CAM_FILE_IO_ERROR);
    return volm_io::CAM_FILE_IO_ERROR;
  }
  //: load the depth map
  depth_map_scene_sptr dm = new depth_map_scene(img_ni(), img_nj());
  if (!volm_io::read_labelme(label_file(), cam, dm)) {
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR);
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  volm_io::write_status(out_folder(), volm_io::EXE_RUNNING, 0);
  
  //: create containers
  
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle(),vmin(),dmax());
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, cap_angle(), point_angle());

  //: create query array
  volm_query_sptr query = new volm_query(dm, sph, sph_shell, img_ni(), img_nj(),
	                                     init_focal(), conf_focal(), 
										 init_heading(), conf_heading(),
										 init_tilt(),conf_tilt(),
										 init_roll(),conf_roll(),
										 altitude());
  //: screen output
  vcl_cout << " for spherical surface, point angle = " << point_angle() << " degree, number of rays: " << sph_shell->get_container_size() << vcl_endl;
  vcl_cout << " for " << img_ni() << " x " << img_nj() << " query image, there are following depth map region"  << vcl_endl;
  vcl_cout << " generated depth_map_scene " << vcl_endl;
  if(dm->sky()){
    vcl_cout << " sky region, min_depth = " << 255 << vcl_endl;
  }
  if(dm->scene_regions().size()) {
    for(unsigned i = 0; i < dm->scene_regions().size(); i++) {
	  vcl_cout << ' ' << (dm->scene_regions())[i]->name() << " region, min_depth = " << (dm->scene_regions())[i]->min_depth() << vcl_endl;
	}
  }
  //: check camera hypothesis
  vcl_vector<double>& focals = query->focals();
  vcl_vector<double>& headings = query->headings();
  vcl_vector<double>& tilts = query->tilts();
  vcl_vector<double>& rolls = query->rolls();
  vcl_cout << " generated " << query->get_cam_num() << " camera hypothesis, with " 
           << focals.size() << " focals, "
		   << headings.size() << " headings, " 
		   << tilts.size() << " tilts, and " 
		   << rolls.size() << " rolls "
		   << vcl_endl;
  for(unsigned i = 0; i < focals.size(); i++)
    for(unsigned j = 0; j < headings.size(); j++)
      for(unsigned k = 0; k < tilts.size(); k++)
        for(unsigned l = 0; l < rolls.size(); l++) {
          double focal = focals[i]; double heading = headings[j]; double tilt = tilts[k];  double roll = rolls[l];
		  unsigned idx = l + (unsigned)rolls.size()*(k + (unsigned)tilts.size()*(j + (unsigned)headings.size()*i));
		  vcl_cout << " camera " << idx << " ---> "
                   << " f = " << focal << ",   " 
				   << " h = " << heading << ",   "
				   << " t = " << tilt << ",   "
				   << " r = " << roll << vcl_endl;
		}
  vcl_cout << " For each camera hypothesis, generated query_size is " << query->get_query_size() << " byte" << vcl_endl;
  //: visualize query
  if(save_images()){
    vcl_string vrml_fname = out_folder() + "cam_hypo_schematic.vrml";
    query->draw_template(vrml_fname, sph_shell, dm);
    query->draw_query_images(out_folder(), dm);
  }
  
  return volm_io::SUCCESS;
}