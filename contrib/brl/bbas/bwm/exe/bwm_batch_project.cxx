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

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

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
    vil_image_view<float> *img;
    if (img_base->pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
     img = dynamic_cast<vil_image_view<float>*>(img_base.ptr());
    else {
      std::cerr << "only float format supported -- modify code\n";
      return 0;
    }
    vpgl_camera_double_sptr ratcam = read_rational_camera<double>(cam_file);

    //With polygons saved using bwm_main
    //the projection only works with the normal rational camera
    //this should be fixed to work with the local camera
    //
#if 0
    vpgl_camera_double_sptr ratcam = read_local_rational_camera<double>(cam_file);

    if ( !ratcam.as_pointer() ) {
      std::cerr << "Rational camera isn't local... trying global\n";
      ratcam = read_rational_camera<double>(cam_file);
    }

    if ( !ratcam.as_pointer() ) {
      std::cerr << "Failed to load rational camera from file" <<cam_file << std::endl;
      return -1;
    }
#endif // 0

    bwm_observer_cam::project_meshes(poly_paths, ratcam.as_pointer(), poly_2d_list);

    //scan through polygons, retrieve contained pixels
    vgl_polygon<double> this_poly;
    for (unsigned i = 0; i < poly_2d_list.size(); i++)
    {
      this_poly = poly_2d_list[i];
      vgl_polygon_scan_iterator<double> psi(this_poly);
      std::stringstream polyfile_out;
      polyfile_out.clear();
      polyfile_out << output_dir() << "/image_" << img_idx << ".txt";
      std::ofstream out_fs(polyfile_out.str().c_str(),std::ios::app);

      //save the polygon x-y coordinates
      for (psi.reset(); psi.next();)
      {
        int y = psi.scany();
        for (int x = psi.startx(); x<= psi.endx(); ++x)
        {
          out_fs << x << ' ' << y << std::endl;
        }
      }

      //save intensity within polygon for each plane
      for (unsigned p =0; p<img->nplanes(); p++)
      {
        std::stringstream colorfile_out;
        colorfile_out.clear();
        colorfile_out << output_dir() << "/image_" << img_idx << "_plane_" << p << ".txt";
        std::ofstream color_ofs(colorfile_out.str().c_str(), std::ios::app);
        for (psi.reset(); psi.next();)
        {
          unsigned int y = psi.scany();
          for (unsigned int x = psi.startx(); (int)x<= psi.endx(); ++x)
          {
            if (x < img->ni() && y < img ->nj())
              color_ofs << (*img)(x,y,p) << std::endl;
            else
              color_ofs << std::endl;
          }
        }
      }
    }
  }
}
