#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_ios.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_point_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_numeric_traits.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_bounds.h>
#include <icam_ocl/icam_ocl_minimizer.h>
#include <icam_ocl/icam_ocl_view_sphere.h>

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
bool load_image(vil_image_view_base_sptr const& base_img, vil_image_view<T>*& image)
{
  if (base_img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float>* fimage = static_cast<vil_image_view<float>*> (base_img.ptr());
    image = new vil_image_view<T>(base_img->ni(), base_img->nj());
    vil_convert_cast<float,T>(*fimage, *image);
  }
  else if (base_img->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte>* byte_image = static_cast<vil_image_view<vxl_byte>*> (base_img.ptr());
    image = new vil_image_view<T>(base_img->ni(), base_img->nj());
    vil_convert_cast<vxl_byte,T>(*byte_image, *image);
  }
  else if (base_img->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double>* img = static_cast<vil_image_view<double>*> (base_img.ptr());
    image = new vil_image_view<T>(base_img->ni(), base_img->nj());
    vil_convert_cast<double,T>(*img, *image);
  }
  else {
    vcl_cout << "icam_register_image_process -- image type " << base_img->pixel_format() << " is not supported!" << vcl_endl;
    return false;
  }
  return true;
}

template <class T>
bool load_image(vcl_string const& path, vil_image_view<T>*& image)
{
  vcl_cout << path.c_str() << vcl_endl;
  vil_image_view_base_sptr base_img = vil_load(path.c_str(),true);
  if (!base_img)
    return false;
  else
    return load_image(base_img, image);
}

void create_view_sphere(icam_ocl_view_sphere_sptr& view_sphere)
{
  vgl_box_3d<double> world_bb(orig_x, orig_y, orig_z, orig_x+dim_x, orig_y+dim_y, orig_z+dim_z);
  view_sphere =new icam_ocl_view_sphere(world_bb, radius);

  // generate the view points-cameras
  view_sphere->create_view_points(elevation, view_angle, image_ni, image_nj);
}

static void test_minimizer()
{
  vcl_string root_dir = testlib_root_dir();
  vcl_vector<vcl_string> image_f, depth_f, camera_f;

  // view 0 
  image_f.push_back("C:/images/Calibration/expected142.tiff");
  depth_f.push_back("C:/images/Calibration/depth_142.tif");
  camera_f.push_back("C:/images/Calibration/camera_00142.txt");

  // view 1
  image_f.push_back("C:/images/Calibration/frame_142.png");
  depth_f.push_back("C:/images/Calibration/depth_142.tif");
  camera_f.push_back("C:/images/Calibration/camera_00142.txt");

  // view 2(the closest one)
  image_f.push_back("C:/images/Calibration/expected142.tiff");
  depth_f.push_back("C:/images/Calibration/depth_142.tif");
  camera_f.push_back("C:/images/Calibration/camera_00142.txt");

  //view 3
  image_f.push_back("C:/images/Calibration/frame_142.png");
  depth_f.push_back("C:/images/Calibration/depth_142.tif");
  camera_f.push_back("C:/images/Calibration/camera_00142.txt");

  // source image
  vcl_string source_file = "C:/images/Calibration/frame_145.png";

  // set the images to view points
  vcl_map<unsigned, vil_image_view<float>* > images;
  vcl_map<unsigned, vil_image_view<double>* > depth_images;

  // load dest images
  for (unsigned i=0; i<image_f.size(); i++) {
    vil_image_view<float> *image;
    if (load_image<float>(image_f[i], image)) {
      images[i] = image;
    } else {
      vcl_cerr << "error loading image.\n";
      return;
    }
  }
  
  //load depth images
  for (unsigned i=0; i<depth_f.size(); i++) {    
    vil_image_view<double> *image;
    if (load_image<double>(depth_f[i], image)) {
      depth_images[i] = image;
    } else {
      vcl_cerr << "error loading image.\n";
      return;
    }
  }

  icam_ocl_view_sphere_sptr view_sphere;
  create_view_sphere(view_sphere);
  // set cameras
  vcl_map<unsigned, vpgl_camera_double_sptr> cam_map;
  for (unsigned i=0; i<camera_f.size(); i++) { 
    vcl_ifstream ifs(camera_f[i].c_str());
    vpgl_perspective_camera<double>* cam=new vpgl_perspective_camera<double>();
    ifs >> *cam;
    cam_map[i] = cam;
  }
  view_sphere->set_cameras(cam_map);

  view_sphere->set_images(images, depth_images);

  vil_image_view<float> *source_img;
  if (load_image<float>(source_file, source_img)) {
    view_sphere->register_image(*source_img);
  }
}


TESTMAIN( test_minimizer );

