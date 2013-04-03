//:
// \file
// \brief dummy executable to test query submission interface
// \author Ozge C. Ozcanli
// \date Sept 20, 2012

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

int main(int argc,  char** argv)
{
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<vcl_string> category_file("-cat", "category file for transferring labelme type to land id", "");
  vul_arg<vcl_string> out_folder("-out", "output folder", "");
  vul_arg<bool> save_images("-save", "save out images or not", false);
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  if (cam_file().compare("") == 0 || label_file().compare("") == 0 || out_folder().compare("") == 0 || category_file().compare("") == 0) {
    vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
    volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // check the query input file
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_file(), category_file(), dm, img_category)) {
    volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR);
  }

  // check the camera input file
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev, altitude;
  double top_fov, top_fov_dev, lat, lon;
  if (!volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, top_fov, top_fov_dev, altitude, lat, lon)) {
    volm_io::write_status(out_folder(), volm_io::CAM_FILE_IO_ERROR);  
  }
  

  volm_io::write_status(out_folder(), volm_io::EXE_RUNNING, 0);

  // just generate dummy output
  vcl_vector<volm_tile> tiles = volm_tile::generate_p1_tiles();
  for (unsigned i = 0; i < tiles.size(); ++i) {
    vil_image_view<unsigned int> out(3601, 3601);
    out.fill(volm_io::UNEVALUATED);

    vcl_string out_name = out_folder() + "/VolM_" + tiles[i].get_string() + "_S1x1.tif";
    if (save_images()) {
      vcl_cout << "will write image to: " << out_name << vcl_endl;
      vil_save(out, out_name.c_str());
    }
    else {
      vcl_cout << "would write image to: " << out_name << vcl_endl;
    }
  }

  volm_io::write_status(out_folder(), volm_io::SUCCESS);
  vcl_cout << "returning SUCCESS!\n";
  return volm_io::SUCCESS;
}
