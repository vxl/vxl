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
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double top_fov, top_fov_dev;
  if (!volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, top_fov, top_fov_dev)) {
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
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region " << vcl_endl;
  if(dm->sky()){
    vcl_cout << "\t sky region, min_depth = " << 255 << vcl_endl;
  }
  if(dm->scene_regions().size()) {
    for(unsigned i = 0; i < dm->scene_regions().size(); i++) {
      vcl_cout << "\t " <<  (dm->scene_regions())[i]->name()  << " region "  
               << ",\t min_depth = " << (dm->scene_regions())[i]->min_depth() 
               << ",\t max_depth = " << (dm->scene_regions())[i]->max_depth() 
               << ",\t order = " << (dm->scene_regions())[i]->order()
               << vcl_endl;
    }
  }
  vcl_cout << " for spherical surface, point angle = " << point_angle() << " degree, " 
           << ", top_angle = " << top_angle()
           << ", bottom_angle = " << bottom_angle()
           << ", generated query has size " << query->get_query_size() << vcl_endl;

  vcl_cout << " The query has " << query->get_cam_num() << " cameras: " << vcl_endl;
  vcl_cout << " \t" << query->headings().size() << " headings = " << query->headings()[0] << " +/- " << query->head_d_ << " increment: " << query->head_inc_ << vcl_endl;
  vcl_cout << " \t" << query->tilts().size() << " tilts = " << query->tilts()[0] << " +/- " << query->tilt_d_ << " increments: " << query->tilt_inc_ << vcl_endl;
  vcl_cout << " \t" << query->rolls().size() << " rolls = " << query->rolls()[0] << " +/- " << query->roll_d_ << " increments: " << query->roll_inc_ << vcl_endl;
  vcl_cout << " \t" << query->top_fovs().size() << " top_fov = " << query->top_fovs()[0] << " +/- " << query->tfov_d_ << " increment: " << query->tfov_inc_ << vcl_endl;
  vcl_cout << " For each camera hypothesis, generated query_size is " << query->get_query_size() << " byte" << vcl_endl;
  
//#if 0  
  // check generated camera parameters
  vcl_vector<double>& headings = query->headings();
  vcl_vector<double>& tilts = query->tilts();
  vcl_vector<double>& rolls = query->rolls();
  vcl_vector<double>& top_fovs = query->top_fovs();
  vcl_vector<unsigned>& ray_count = query->valid_ray_num();

  for(unsigned i = 0; i < top_fovs.size(); i++)
    for(unsigned j = 0; j < headings.size(); j++)
      for(unsigned k = 0; k < tilts.size(); k++)
        for(unsigned l = 0; l < rolls.size(); l++) {
          double top_fov = top_fovs[i]; double heading = headings[j]; double tilt = tilts[k];  double roll = rolls[l];
          unsigned idx = l + (unsigned)rolls.size()*(k + (unsigned)tilts.size()*(j + (unsigned)headings.size()*i));
          vcl_cout << " camera " << idx << " ---> "
                   << " valid ray = " << query->get_valid_ray_num(idx) 
                   << " f = " << top_fov << ",   " 
                   << " h = " << heading << ",   "
                   << " t = " << tilt << ",   "
                   << " r = " << roll << vcl_endl;
        }
//#endif

  // visualize query
  if(save_images()){
    vcl_string vrml_fname = out_folder() + "cam_hypo_schematic.vrml";
    query->draw_template(vrml_fname);
    vcl_string prefix = out_folder();
    query->visualize_query(prefix);
    query->draw_query_images(out_folder());
  }
  
  return volm_io::SUCCESS;
}
