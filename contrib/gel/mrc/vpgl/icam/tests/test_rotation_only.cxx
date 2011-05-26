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
#include <icam/icam_minimizer.h>
#include <icam/icam_view_sphere.h>
#include <vpgl/algo/vpgl_ray.h>
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
bool load_image(vcl_string const& path, vil_image_view<T>*& image)
{
  vcl_cout << path.c_str() << vcl_endl;
  vil_image_view_base_sptr base_img = vil_load(path.c_str(),true);
  if (!base_img)
    return false;
  else
    return load_image(base_img, image);
}

static void test_rotation_only()
{
  vcl_string exp_image_path = "C:/images/Calibration/expected142.tiff";
  vcl_string true_camera_path = "C:/images/Calibration/camera_00142.txt";
  vcl_string act_image_path = "C:/images/Calibration/frame_142.png";
  vil_image_view<vxl_byte> *exp_img, *act_img;
  load_image<vxl_byte>(exp_image_path, exp_img);
  load_image<vxl_byte>(act_image_path, act_img);
  vgl_rotation_3d<double> rot = vpgl_ray::rot_to_point_ray(0.0, 5.0);
  vcl_cout << rot.as_matrix() << '\n';
}


TESTMAIN( test_rotation_only );

