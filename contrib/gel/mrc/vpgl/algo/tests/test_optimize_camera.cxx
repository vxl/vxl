#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vpgl/algo/vpgl_optimize_camera.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h> // for pi

static void test_optimize_camera()
{
  const double max_t_err = 100.0; // maximum translation error to introduce
  const double max_r_err = vnl_math::pi/2; // maximum rotation error to introduce (radians)
  const double max_p_err = 0.5; // maximum image error to introduce (pixels)

  vcl_vector<vgl_homg_point_3d<double> > world;
  world.push_back(vgl_homg_point_3d<double>(1.0, 0.0, 0.0));
  world.push_back(vgl_homg_point_3d<double>(0.0, 1.0, 0.0));
  world.push_back(vgl_homg_point_3d<double>(0.0, 0.0, 1.0));
  world.push_back(vgl_homg_point_3d<double>(1.0, 1.0, 0.0));
  world.push_back(vgl_homg_point_3d<double>(0.0, 1.0, 1.0));
  world.push_back(vgl_homg_point_3d<double>(1.0, 0.0, 1.0));

  vpgl_calibration_matrix<double> K(2000.0,vgl_homg_point_2d<double>(512,384));
  vgl_homg_point_3d<double> c(10.0,10.0,10.0);
  vnl_vector<double> w = (vnl_math::pi - vcl_asin(vcl_sqrt(2.0/3.0)))/vcl_sqrt(2.0)*vnl_double_3(-1.0,1.0,0.0);
  vgl_h_matrix_3d<double> R(vnl_rotation_matrix(w),
                            vnl_vector_fixed< double, 3 >(0.0));
  vpgl_perspective_camera<double> cam(K,c,R);

  vcl_vector<vgl_point_2d<double> > image;
  vnl_random rnd;
  // project each point adding uniform noise in a [-max_p_err, max_p_err] pixel window
  for (unsigned int i=0; i<world.size(); ++i){
    vgl_homg_point_2d<double> hpt = cam(world[i]);
    vgl_vector_2d<double> err(rnd.drand32()-0.5, rnd.drand32()-0.5);
    err *= max_p_err;
    image.push_back(vgl_point_2d<double>(hpt.x()/hpt.w(), hpt.y()/hpt.w())+err);
    vcl_cout <<  err << '\t'
             <<  vgl_point_2d<double>(hpt.x()/hpt.w(), hpt.y()/hpt.w()) << vcl_endl;
  }
  vcl_cout << cam << vcl_endl;

  // select a random rotation axis
  vnl_double_3 dw = (vnl_double_3(rnd.drand32(), rnd.drand32(), rnd.drand32())-0.5).normalize();
  // select a random rotation angle
  dw *= max_r_err * rnd.drand32();
  vnl_matrix_fixed<double,3,3> dR(vnl_rotation_matrix(dw));
  vgl_h_matrix_3d<double> H(vnl_rotation_matrix(dw)*vnl_rotation_matrix(w), vnl_vector_fixed< double, 3 >(0.0));

  vgl_vector_3d<double> dc(rnd.drand32()-0.5, rnd.drand32()-0.5, rnd.drand32()-0.5);

  vcl_cout << "Initial position:" << cam.get_camera_center() << vcl_endl
           << "Initial principal ray:" << cam.principal_axis() << vcl_endl;

  vpgl_perspective_camera<double> err_cam(K,c+max_t_err*dc,H);
  vcl_cout << "Perturbed position:" << err_cam.get_camera_center() << vcl_endl
           << "Perturbed principal ray:" << err_cam.principal_axis() << vcl_endl;

  vpgl_perspective_camera<double> opt_cam = vpgl_optimize_camera::opt_orient_pos(err_cam,world,image);

  vcl_cout << "Estimated position:" << opt_cam.get_camera_center() << vcl_endl
           << "Estimated principal ray:" << opt_cam.principal_axis() << vcl_endl;
}

TESTMAIN(test_optimize_camera);
