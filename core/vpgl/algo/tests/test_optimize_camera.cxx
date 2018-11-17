#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_optimize_camera.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h> // for pi

void test_opt_orient_pos(vpgl_perspective_camera<double> const& cam,
                         std::vector<vgl_homg_point_3d<double> > const& world,
                         std::vector<vgl_point_2d<double> > const& image,
                         vnl_random &rnd) {
  constexpr double max_t_err = 10.0; // maximum translation error to introduce
  const double max_r_err = vnl_math::pi/4; // maximum rotation error to introduce (radians)

  // select a random rotation axis
  vnl_double_3 dw = (vnl_double_3(rnd.drand32(), rnd.drand32(), rnd.drand32())-0.5).normalize();
  // select a random rotation angle
  dw *= max_r_err * rnd.drand32();
  vgl_rotation_3d<double> dR(dw);

  vgl_vector_3d<double> dc(rnd.drand32()-0.5, rnd.drand32()-0.5, rnd.drand32()-0.5);

  std::cout << "Initial position:" << cam.get_camera_center() << std::endl
           << "Initial principal ray:" << cam.principal_axis() << std::endl;

  vgl_point_3d<double> c = cam.get_camera_center();
  const vpgl_calibration_matrix<double>& K = cam.get_calibration();
  const vgl_rotation_3d<double>& R = cam.get_rotation();

  vgl_point_3d<double> new_center = c + max_t_err*dc;
  std::cout << "new center = " << new_center << std::endl;
  vpgl_perspective_camera<double> err_cam(K,c+max_t_err*dc,dR*R);
  std::cout << "Perturbed position:" << err_cam.get_camera_center() << std::endl
           << "Perturbed principal ray:" << err_cam.principal_axis() << std::endl;

  vpgl_perspective_camera<double> opt_cam = vpgl_optimize_camera::opt_orient_pos(err_cam,world,image);

  std::cout << "Estimated position:" << opt_cam.get_camera_center() << std::endl
           << "Estimated principal ray:" << opt_cam.principal_axis() << std::endl;

  double dist = vgl_distance(opt_cam.get_camera_center(), cam.get_camera_center());
  TEST_NEAR("opt_orient_pos: position", dist, 0, 0.25);

  double cos_angle = dot_product( opt_cam.principal_axis(), cam.principal_axis() );
  if (cos_angle > 1) { cos_angle = 1; }
  double angle = std::acos( cos_angle );
  TEST_NEAR("opt_orient_pos: principal_axis", angle, 0, 0.1);
}

void test_opt_orient_pos_f(vpgl_perspective_camera<double> const& cam,
                           std::vector<vgl_homg_point_3d<double> > const& world,
                           std::vector<vgl_point_2d<double> > const& image,
                           vnl_random &rnd) {

  constexpr double max_t_err = 0;//10.0; // maximum translation error to introduce
  constexpr double max_r_err = 0;//vnl_math::pi/4; // maximum rotation error to introduce (radians)

  // select a random rotation axis
  vnl_double_3 dw = (vnl_double_3(rnd.drand32(), rnd.drand32(), rnd.drand32())-0.5).normalize();
  // select a random rotation angle
  dw *= max_r_err * rnd.drand32();
  vgl_rotation_3d<double> dR(dw);

  vgl_vector_3d<double> dc(rnd.drand32()-0.5, rnd.drand32()-0.5, rnd.drand32()-0.5);

  std::cout << "Initial position:" << cam.get_camera_center() << std::endl
           << "Initial principal ray:" << cam.principal_axis() << std::endl
           << "Initial focal length:" << cam.get_calibration().focal_length() << std::endl;

  vgl_point_3d<double> c = cam.get_camera_center();
  const vpgl_calibration_matrix<double>& K = cam.get_calibration();
  const vgl_rotation_3d<double>& R = cam.get_rotation();

  vgl_point_3d<double> new_center = c + max_t_err*dc;
  std::cout << "new center = " << new_center << std::endl;
  vpgl_perspective_camera<double> err_cam(K,c+max_t_err*dc,dR*R);
  std::cout << "Perturbed position:" << err_cam.get_camera_center() << std::endl
           << "Perturbed principal ray:" << err_cam.principal_axis() << std::endl
           << "Perturbed focal length:" << err_cam.get_calibration().focal_length() << std::endl;

  vpgl_perspective_camera<double> opt_cam = vpgl_optimize_camera::opt_orient_pos_f(err_cam,world,image);

  std::cout << "Estimated position:" << opt_cam.get_camera_center() << std::endl
           << "Estimated principal ray:" << opt_cam.principal_axis() << std::endl
           << "Estimated focal length:" << opt_cam.get_calibration().focal_length() << std::endl;

  double dist = vgl_distance(opt_cam.get_camera_center(), cam.get_camera_center());
  TEST_NEAR("opt_orient_pos_f: position", dist, 0, 0.25);

  double cos_angle = dot_product( opt_cam.principal_axis(), cam.principal_axis() );
  if (cos_angle > 1) { cos_angle = 1; }
  double angle = std::acos( cos_angle );
  TEST_NEAR("opt_orient_pos_f: principal_axis", angle, 0, 0.1);

  double f_opt = opt_cam.get_calibration().focal_length();
  double f_cam = cam.get_calibration().focal_length();
  TEST_NEAR_REL("opt_orient_pos_f: focal_len", f_opt, f_cam, 0.1);
}

static void test_optimize_camera()
{
  constexpr double max_p_err = 0.25; // maximum image error to introduce (pixels)

  std::vector<vgl_homg_point_3d<double> > world;
  double side_len = 1.0;
  world.emplace_back(0.0,0.0,0.0);
  world.emplace_back(0.0, 0.0, side_len);
  world.emplace_back(0.0, side_len, 0.0);
  world.emplace_back(0.0, side_len, side_len);
  world.emplace_back(side_len, 0.0, 0.0);
  world.emplace_back(side_len, 0.0, side_len);
  world.emplace_back(side_len, side_len, 0.0);
  world.emplace_back(side_len, side_len, side_len);

  vpgl_calibration_matrix<double> K(2000.0,vgl_homg_point_2d<double>(512,384));
  vgl_homg_point_3d<double> c(4.0,4.0,4.0);
  vpgl_perspective_camera<double> cam(K,c,vgl_rotation_3d<double>());
  // look at the center of the cube
  cam.look_at(vgl_homg_point_3d<double>(side_len/2, side_len/2, side_len/2));

  std::vector<vgl_point_2d<double> > image;
  // seed with fixed number for repeatable results
  vnl_random rnd(1234);
  // project each point adding uniform noise in a [-max_p_err, max_p_err] pixel window
  for (const auto & i : world){
    vgl_homg_point_2d<double> hpt = cam(i);
    vgl_vector_2d<double> err(rnd.drand32()-0.5, rnd.drand32()-0.5);
    err *= max_p_err;
    image.push_back(vgl_point_2d<double>(hpt.x()/hpt.w(), hpt.y()/hpt.w())+err);
    std::cout <<  err << '\t'
             <<  vgl_point_2d<double>(hpt.x()/hpt.w(), hpt.y()/hpt.w()) << std::endl;
  }
  std::cout << cam << std::endl;

  test_opt_orient_pos(cam, world, image, rnd);
  test_opt_orient_pos_f(cam, world, image, rnd);
}

TESTMAIN(test_optimize_camera);
