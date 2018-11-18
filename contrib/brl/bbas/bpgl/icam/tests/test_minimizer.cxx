#include <iostream>
#include <iomanip>
#include <ios>
#include <string>
#include <vector>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_box_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_load.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vsph/vsph_camera_bounds.h>
#include <icam/icam_minimizer.h>
#include <icam/icam_view_sphere.h>

#define elevation 0.2
#define view_angle 0.5
#define orig_x -2.45
#define orig_y -2.35
#define orig_z -0.4
#define dim_x 4.2
#define dim_y 4.2
#define dim_z 1.6
#define radius 4.9
#define image_ni 1870
#define image_nj 720

template <class T>
bool load_image(std::string const& path, vil_image_view<T>*& image)
{
  std::cout << path.c_str() << std::endl;
  vil_image_view_base_sptr base_img = vil_load(path.c_str(),true);
  if (!base_img)
    return false;
  else
    return load_image(base_img, image);
}

void create_view_sphere(icam_view_sphere_sptr& view_sphere)
{
  vgl_box_3d<double> world_bb(orig_x, orig_y, orig_z, orig_x+dim_x, orig_y+dim_y, orig_z+dim_z);
  view_sphere =new icam_view_sphere(world_bb, radius);

  // generate the view points-cameras
  view_sphere->create_view_points(elevation, view_angle, image_ni, image_nj);
}

static void test_minimizer()
{
#if 0 // need actual data (TO DO add test that doesn't)
  std::string root_dir = testlib_root_dir();
  std::map<unsigned int, std::string> images, depth_images;
  std::vector<std::string> camera_f;

  // view 0
  images[0] = root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/expected142.tiff";
  depth_images[0] = root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/depth_142.tif";
  camera_f.push_back(root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/camera_00142.txt");

  // view 1
  images[1] = root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/frame_142.png";
  depth_images[1] = root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/depth_142.tif";
  camera_f.push_back(root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/camera_00142.txt");

  // view 2(the closest one)
  images[2] = root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/expected142.tiff";
  depth_images[2] = root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/depth_142.tif";
  camera_f.push_back(root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/camera_00142.txt");

  //view 3
  images[3] = root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/frame_142.png";
  depth_images[3] = root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/depth_142.tif";
  camera_f.push_back(root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/camera_00142.txt");

  // source image
  std::string source_file = root_dir + "/contrib/gel/mrc/vpgl/tests/images/calibration/frame_145.png";

  icam_view_sphere_sptr view_sphere;
  create_view_sphere(view_sphere);
  // set cameras
  std::map<unsigned, vpgl_camera_double_sptr> cam_map;
  for (unsigned i=0; i<camera_f.size(); i++) {
    std::ifstream ifs(camera_f[i].c_str());
    vpgl_perspective_camera<double>* cam=new vpgl_perspective_camera<double>();
    ifs >> *cam;
    cam_map[i] = cam;
  }
  view_sphere->set_cameras(cam_map);

  view_sphere->set_images(images, depth_images);
  icam_minimizer_params params;
  vil_image_view<float> *source_img;
  if (load_image<float>(source_file, source_img)) {
    view_sphere->register_image(*source_img, params);
  }
#endif
}


TESTMAIN( test_minimizer );
