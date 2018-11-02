//:
// \file

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <bwm/bwm_observer_cam.h>


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vul/vul_awk.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>

#include <vpgl/vpgl_perspective_camera.h>


#include <vil/vil_load.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

//:
// An executable that projects polygons into a list of images
// and writes to disk the pixel location within the polygon as well
// as the intensity at those pixels
int main(int argc, char** argv)
{
  //Get Inputs
  vul_arg<std::string> cam_list("-cam_list", "list of camera filenames", "");
  vul_arg<std::string> img_list("-img_list", "list of images filenames", "");
  vul_arg<std::string> poly_list("-poly_list", "list of polygon filenames", "");
  vul_arg<std::string> output_dir("-output_dir", "dir where pixel values inside poly ar saved", "");
  vul_arg_parse(argc, argv);

  std::ifstream poly_fs( poly_list().c_str() );
  std::ifstream cam_fs( cam_list().c_str() );
  std::ifstream img_fs( img_list().c_str() );

  if (!poly_fs)
  {
    std::cerr << "Error in bwm_batch_porject: Failed to open polygon list file\n";
    return -1;
  }

  //Retrieve poly filenames
  std::vector<std::string> poly_paths;
  vul_awk awk_poly(poly_fs);
  for (; awk_poly; ++awk_poly)
  {
    std::string poly_file = awk_poly.line();
    //check for empty lines so that and empty line at the end of file won't cause error
    if (poly_file.empty())
      continue;
    poly_paths.push_back(poly_file);
  }

  //For each camera project all polygons and retrieve pixel location.
  std::vector<vgl_polygon<double> > poly_2d_list;
  vul_awk awk_cam(cam_fs);
  vul_awk awk_img(img_fs);

  unsigned img_idx = 0;
  for (; awk_cam; ++awk_cam, ++img_idx)
  {
    std::string cam_file = awk_cam.line();
    std::string img_file = awk_img.line();
    if (cam_file.empty()|| img_file.empty())
      continue;

    vil_image_view_base_sptr img_base = vil_load(img_file.c_str());
    vil_image_view<unsigned char> img(img_base->ni(),img_base->nj());
    img.fill(0);

    // read projection matrix from the file.
    std::ifstream ifs(cam_file.c_str());
    if (!ifs.is_open()) {
      std::cerr << "Failed to open file " << cam_file << std::endl;
      return -1;
    }

    vpgl_perspective_camera<double>* cam = new vpgl_perspective_camera<double>();

    ifs >> *cam;
    ifs.close();

    bwm_observer_cam::project_meshes(poly_paths, cam, poly_2d_list);

    //scan through polygons, retrieve contained pixels
    vgl_polygon<double> this_poly;
    for (unsigned i = 0; i < poly_2d_list.size(); i++)
    {
      this_poly = poly_2d_list[i];
      vgl_polygon_scan_iterator<double> psi(this_poly);

      //save the polygon x-y coordinates
      for (psi.reset(); psi.next();)
      {
        unsigned int y = psi.scany();
        if (y<img.nj())
          for (unsigned int x = psi.startx(); (int)x<= psi.endx() && x<img.ni(); ++x)
            img(x,y)=255;
      }
    }
    std::stringstream polyfile_out;
    polyfile_out.clear();
    polyfile_out << output_dir() << "/image_" << img_idx << ".png";
    vil_save(img,polyfile_out.str().c_str());
    poly_2d_list.clear();
  }
}
