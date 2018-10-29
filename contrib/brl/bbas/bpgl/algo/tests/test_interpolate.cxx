#include <iostream>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/algo/bpgl_interpolate.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_math.h>
static void test_interpolate()
{
  //========= preliminary tests ====================
  // test log and exp on identity
  vnl_double_3x3 I;
  I.set_identity();
  std::cout << "I\n" << I;
  vnl_double_3x3 lR = bpgl_interpolate::logR(I);
  vnl_double_3x3 elR=bpgl_interpolate::expr(lR);
  std::cout << "log(I)\n" << lR
           << "exp(log(I))\n" << elR;
  vnl_double_3x3 dif = I - elR;
  double tolerance = 1.0e-8;
  TEST_NEAR("Lie operations on identity", dif.frobenius_norm(), 0.0, tolerance);
  vgl_rotation_3d<double> R0(I);

  //test log and exp on 180 degrees
  I[0][0]=-1;   I[0][1]=0;
  I[1][0]=0;   I[1][1]=-1;
  std::cout << "PI\n" << I;
  lR = bpgl_interpolate::logR(I);
  elR=bpgl_interpolate::expr(lR);
  std::cout << "log(PI)\n" << lR
           << "exp(log(PI))\n" << elR;
  dif = I - elR;
  TEST_NEAR("Lie operations on pi", dif.frobenius_norm(), 0.0, tolerance);

  // test log and exp on 45 degree rotation
  double s2 = vnl_math::sqrt1_2;
  I[0][0]=s2;   I[0][1]=-s2;
  I[1][0]=s2;   I[1][1]=s2;
  std::cout << "45I\n" << I;
  vnl_double_3x3 lR45 = bpgl_interpolate::logR(I);
  elR=bpgl_interpolate::expr(lR45);
  std::cout << "log(45I)\n" << lR45
           << "exp(log(45I))\n" << elR;
  dif = I - elR;
  TEST_NEAR("Lie operations on pi/4", dif.frobenius_norm(), 0.0, tolerance);
  vgl_rotation_3d<double> R1(I);

  //==========test rotation interpolation==========
  vnl_double_3x3 Rm0 = R0.as_matrix();
  vnl_double_3x3 Rm1 = R1.as_matrix();
  std::vector<vnl_double_3x3> Rintrps =
    bpgl_interpolate::interpolateR(Rm0, Rm1, 1);
  vnl_double_3x3 Rintrp = Rintrps[0];
  std::cout << "Interpolated R\n" << Rintrp << '\n';
  double interp_angle = std::acos(Rintrp[0][0]);
  std::cout << "Interpolated angle = " << vnl_math::deg_per_rad*interp_angle << '\n';
  TEST_NEAR("Interpolated rotation",interp_angle, 0.392699082,1e-8);

  //====test with real camera rotations computed from a video sequence====
  vnl_quaternion<double> q0(-0.0208205, 0.0889953, 0.00309305, 0.99581);
  R0 = vgl_rotation_3d<double>(q0);
  vnl_quaternion<double> q1(-0.0187549, 0.0782806, -0.00136849, 0.996754);
  R1 = vgl_rotation_3d<double>(q1);
  //test if the basic interpolation solution is valid
   Rm0 = R0.as_matrix(); Rm1 = R1.as_matrix();
   vnl_double_3x3 tm = Rm0.transpose()*Rm1;
   std::cout << "R0tR1\n"
            << '\n'<< tm << '\n';
   lR = bpgl_interpolate::logR(tm);
   std::cout << "Log(R0tR1)\n"
            << '\n'<< lR << '\n';
   elR=bpgl_interpolate::expr(lR);
   std::cout << "Should be R1\n"
            << '\n'<< Rm0*elR << '\n';
   dif = Rm1 - Rm0*elR;

  TEST_NEAR("Lie Operations on camera rotation",
            dif.frobenius_norm(), 0.0, tolerance);

  // =============== test A and Ainv ==================

  vnl_double_3x3 at = bpgl_interpolate::A(lR45);
  vnl_double_3x3 atinv = bpgl_interpolate::Ainv(lR45);
//UNUSED  vnl_double_3x3 id1 = at*atinv, id2 = atinv*at;
  //=========== test camera interpolation ==============
  //set up the cameras
  vnl_double_3x3 M;
  M.fill(0.0);
  M[0][0] = 2200; M[0][2] = 640;
  M[1][1] = 2200; M[1][2] = 360;
  M[2][2]=1.0;
  vpgl_calibration_matrix<double> K(M);

  vgl_homg_point_3d<double> c0(0.0, 0.0, 0.0);
  vgl_homg_point_3d<double> c1(10.0, 10.0, -10.0);
  vpgl_perspective_camera<double> cam0(K,c0,R0);

  vpgl_perspective_camera<double> cam1(K,c1,R1);
  std::vector<vpgl_perspective_camera<double> > cams;
  bool success = bpgl_interpolate::interpolate(cam0, cam1, 1, cams);
  if (success)
    for (const auto & cam : cams)
      std::cout << cam;
  vpgl_perspective_camera<double> cam_interp = cams[0];
  vgl_point_3d<double> ci = cam_interp.get_camera_center();
  vgl_point_3d<double> cact(5,5,-5);
  double d = vgl_distance(ci, cact);
  TEST_NEAR("test interpolate_cameras", d,0.1, 0.001);
}

TESTMAIN(test_interpolate);
