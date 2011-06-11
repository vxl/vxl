#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_ios.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_point_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_math.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_numeric_traits.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_bounds.h>
#include <icam/icam_minimizer.h>
#include <vpgl/algo/vpgl_ray.h>

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
  //This section rotates a correct camera keeping the center of projection
  //fixed. The rotated camera can be used to generate an expected image
  //from a voxel model to be used in correcting the camera
#if 0 // use this section to prepare a camera rotated from the correct orientation
  vcl_string true_camera_path = "f:/downtown_camera_cal/cameras_KRT/camera_00142.txt";
  vcl_ifstream is(true_camera_path.c_str());

  vpgl_perspective_camera<double> cam;
  if (is.is_open())
    is >> cam;
  else
    return;
  vgl_point_3d<double> xc = cam.get_camera_center();
  vgl_rotation_3d<double> rot = vpgl_ray::rot_to_point_ray(0.0, 5.0);
  vcl_cout << rot.as_matrix() << '\n';
  vgl_point_3d<double> rc = rot*xc;
  vgl_vector_3d<double> tr = (xc - rc);
  vpgl_perspective_camera<double> rcam = vpgl_perspective_camera<double>::
    postmultiply(cam, rot, tr);
  vgl_point_3d<double> xcp = rcam.get_camera_center();
  //it can be noted that xcp == xc
  //Save the rotated camera to generate a test expected image
  vcl_string rot_camera_path = "f:/downtown_camera_cal/camera_00142_rotated.txt";
  vcl_ofstream os(rot_camera_path.c_str());
  if (!os.is_open)
    return;
  os << rcam;
  os.close();
#endif

#if 1  // use this section to find the rotation of the rotated test image
       //by remapping the source to dest. The error measure is entropy difference

  //load the resampled original image (current test is at 0.25 scale)
  //this image will act as the destination image in the minimizer
  vcl_string dest_image_path = "f:/downtown_camera_cal/resamp_image_00142.tiff";
  //load the scalled expected image generated using the rotated camera
  //this image will act as the source with an "unknown" camera
  vcl_string source_image_path = "f:/downtown_camera_cal/f00142_scaled_rotated_exp_image.tiff";
  vil_image_view<float> dest_img = vil_load(dest_image_path.c_str(), true);
  vil_image_view<float> source_img = vil_load(source_image_path.c_str(), true);
  if (!dest_img||!source_img)
    return;
  //the minimizer works on images with intensity range [0, 255]
  vil_math_scale_values(dest_img, 255);
  vil_math_scale_values(source_img, 255);
  //the correct camera but at 0.25 scale
  //being loaded to get the calibration matrix, K
  vcl_string camera_path = "f:/downtown_camera_cal/resamp_camera_00142.txt";
  vcl_ifstream is(camera_path.c_str());
  vpgl_perspective_camera<double> cam;
  if (is.is_open())
    is >> cam;
  else
    return;
  vnl_double_3x3 K = cam.get_calibration().get_matrix();
  //make a uniform depth image since the search is over only rotation
  vil_image_view<double> depth(dest_img.ni(), dest_img.nj());
  depth.fill(1.0);
  vgl_rotation_3d<double> rot, min_rot;
  //solve only for rotation translation = 0
  vgl_vector_3d<double> tr(0, 0, 0);
  //form the depth transform (rotation only)
  icam_depth_transform dt(K, depth, rot, tr);

  // set up the minimizer (default params)
  icam_minimizer_params icam_params;
  icam_minimizer minimizer(source_img, dest_img, dt, icam_params, true);

  // search over a set of principal axes within the specified cone
  double min_cost, min_overlap;
  double polar_range = 0.0;
  unsigned n_polar_steps = 0;
  unsigned n_axis_steps = 100;
  double cone_half_angle = 0.15;
  minimizer.rot_search(tr, rot, n_axis_steps, cone_half_angle, n_polar_steps,
                       polar_range, 0, 0.5, min_rot, min_cost, min_overlap);
  // check accuracy the result for finite steps in principal ray
  vgl_rotation_3d<double> R = cam.get_rotation();
  vgl_rotation_3d<double> prot = vpgl_ray::rot_to_point_ray(0.0, 5.0);
  vgl_rotation_3d<double> Rr = R*prot;
  vgl_rotation_3d<double> Rc = Rr*(R.inverse());
  vcl_cout << Rc.as_matrix() << '\n'
           << "angle between principal rays after cone search "
           << vpgl_ray::angle_between_rays(Rc, min_rot)*180.0/vnl_math::pi
           << " degrees. \nEntropy diff = " << min_cost << '\n';

  // refine the rotation using the Powell algorithm
  double min_allowed_overlap = 0.25;
  unsigned pyramid_level = 0;
  minimizer.minimize_rot(min_rot, tr, pyramid_level, min_allowed_overlap);
  vil_image_view<float> mapped_source = minimizer.view(min_rot, tr, 0);
  vcl_cout << "angle between principal rays after Powell "
           << vpgl_ray::angle_between_rays(Rc, min_rot)*180.0/vnl_math::pi
           << " degrees. \nEntropy diff = "<< minimizer.end_error() << '\n';

  // write out the mapped source image using the rotation corresponding
  // to minimum error
  vcl_string mapped_source_path = "f:/downtown_camera_cal/mapped_00142_min_rot.tiff";
  vil_save(mapped_source, mapped_source_path.c_str());
#endif
}


TESTMAIN( test_rotation_only );
