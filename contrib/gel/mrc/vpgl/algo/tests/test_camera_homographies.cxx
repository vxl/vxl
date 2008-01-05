#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_quaternion.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_homographies.h>

static void test_camera_homographies()
{
  vnl_matrix_fixed<double, 3, 4> p;
  p.fill(0.0);
  p[0][0]=1.0;   p[1][1]=1.0;   p[2][2]=1.0;
  p[2][3]=2.0;
  vpgl_proj_camera<double> pc(p);
  vgl_plane_3d<double> pl(0.0,0.0,1.0,-1.0);
  vgl_h_matrix_2d<double> Ht =vpgl_camera_homographies::homography_to_camera(pc, pl);
  
  TEST_NEAR("test plane to image",Ht.get(2,2), 3.0, 0.001);
  vgl_plane_3d<double> p2(0.707107,0, 0.707107,-1.414);
  vgl_h_matrix_2d<double> Ht2 =
    vpgl_camera_homographies::homography_to_camera(pc, p2);
  vgl_homg_point_2d<double> pt1(-1.414, 0,1), pt2(-4,0,1), pt3(0,0,1);
 
  vgl_point_2d<double> ptp1 = Ht2(pt1);
  TEST_NEAR("test plane to image 45degrees point1",ptp1.x(), 0, 0.001);
  vgl_point_2d<double> ptp2 = Ht2(pt2);
  TEST_NEAR("test plane to image 45degrees point2",ptp2.x(), -0.31374, 0.0001);
  vpgl_perspective_camera<double> psc;
  //starts with identity rotation and center at (0, 0, 0)
  vgl_point_3d<double> center(0, 0, -2);
  psc.set_camera_center(center);
  vgl_h_matrix_2d<double> Ht3 =
    vpgl_camera_homographies::homography_to_camera(psc, p2);
  vgl_point_2d<double> ptp3 = Ht3(pt1);
  TEST_NEAR("test plane to image 45degrees perspective point1",
            ptp3.x(), 0, 0.001);
  vgl_h_matrix_2d<double> Ht4 =vpgl_camera_homographies::homography_from_camera(psc, p2);
  vgl_point_2d<double> ptp4 = Ht4(pt3); 
  TEST_NEAR("test plane from image 45degrees perspective point1",
            ptp4.x(), -1.414, 0.001);
// An  set of data corresponding to an actual image sequence
  //World plane derived from 3 world points
  vgl_plane_3d<double> world_plane(-0.0585478, +0.685533, +0.725683, -10.9752);
  vgl_point_3d<double> w0(34.89, -9.92, 27.31);
  vgl_point_3d<double> w1(48.92, -1.46, 20.45);
  vgl_point_3d<double> w2(48.5, -16.03, 34.18);

  // Several cameras from a video sequence
  vnl_double_3x3 M; 
  M.fill(0.0);
  M[0][0] = 2200; M[0][2] = 640;
  M[1][1] = 2200; M[1][2] = 360;
  M[2][2]=1.0;
  vpgl_calibration_matrix<double> K(M);
  vnl_quaternion<double> q(0.00256019, 0.0841515, 0.0344611, 0.995854);
  vgl_rotation_3d<double> R(q);
  vgl_point_3d<double> cen( 35.2729,11.2042,-321.021);
  vpgl_perspective_camera<double> pact(K, cen, R);
  vgl_point_2d<double> proj_w0 = pact.project(w0);
  vgl_point_2d<double> proj_w1 = pact.project(w1);
  vgl_point_2d<double> proj_w2 = pact.project(w2);
  
  vgl_h_matrix_2d<double> Hact = 
    vpgl_camera_homographies::homography_from_camera(pact, world_plane);
  
  vgl_homg_point_2d<double> hi0(1022, 225);
  vgl_homg_point_2d<double> hi1(1111, 283);
  vgl_homg_point_2d<double> hi2(1111, 195);
  vgl_point_2d<double> hwi0 = Hact(hi0);
  vgl_point_2d<double> hwi1 = Hact(hi1);
  vgl_point_2d<double> hwi2 = Hact(hi2);
  double dw1 = vgl_distance(w0, w1);
  double diw1 = vgl_distance(hwi0, hwi1);
  double dw2 = vgl_distance(w0, w2);
  double diw2 = vgl_distance(hwi0, hwi2);


  vnl_quaternion<double> q45(-0.00123478, 0.0259162, 0.00083254, 0.999663);
  vgl_rotation_3d<double> R45(q45);
  vgl_point_3d<double> cen45(1.28374, 2.36223,-321.679);
  vpgl_perspective_camera<double> pact_45(K, cen45, R45);

  vgl_point_2d<double> proj_w0_45 = pact_45.project(w0);
  vgl_point_2d<double> proj_w1_45 = pact_45.project(w1);
  vgl_point_2d<double> proj_w2_45 = pact_45.project(w2);


  vgl_h_matrix_2d<double> Hact_45 = 
    vpgl_camera_homographies::homography_from_camera(pact_45, world_plane);
  vgl_homg_point_2d<double> hi0_45(966, 288);
  vgl_homg_point_2d<double> hi1_45(1063, 341);
  vgl_homg_point_2d<double> hi2_45(1049, 252);
  vgl_point_2d<double> hwi0_45 = Hact_45(hi0_45);
  vgl_point_2d<double> hwi1_45 = Hact_45(hi1_45);
  vgl_point_2d<double> hwi2_45 = Hact_45(hi2_45);
  double diw1_45 = vgl_distance(hwi0_45, hwi1_45);
  double diw2_45 = vgl_distance(hwi0_45, hwi2_45);

  vnl_quaternion<double> q23(0.00887156, 0.0380153, 0.0129012, 0.999154);
  vgl_rotation_3d<double> R23(q23);
  vgl_point_3d<double> cen23(18.8361,6.97435,-321.786);

  vpgl_perspective_camera<double> pact_23(K, cen23, R23);
  vgl_point_2d<double> proj_w0_23 = pact_23.project(w0);
  vgl_point_2d<double> proj_w1_23 = pact_23.project(w1);
  vgl_point_2d<double> proj_w2_23 = pact_23.project(w2);

  vgl_h_matrix_2d<double> Hact_23 = 
    vpgl_camera_homographies::homography_from_camera(pact_23, world_plane);
  vgl_homg_point_2d<double> hi0_23(913, 218);
  vgl_homg_point_2d<double> hi1_23(1005, 272);
  vgl_homg_point_2d<double> hi2_23(996, 183);
  vgl_point_2d<double> hwi0_23 = Hact_23(hi0_23);
  vgl_point_2d<double> hwi1_23 = Hact_23(hi1_23);
  vgl_point_2d<double> hwi2_23 = Hact_23(hi2_23);
  double diw1_23 = vgl_distance(hwi0_23, hwi1_23);
  double diw2_23 = vgl_distance(hwi0_23, hwi2_23);
  //the average error in camera to world distances (in units of gsd)
  double error =   vcl_fabs(dw1-diw1);
  error +=  vcl_fabs(dw2-diw2);
  error +=  vcl_fabs(dw1-diw1_45);
  error +=  vcl_fabs(dw2-diw2_45);
  error +=  vcl_fabs(dw1-diw1_23);
  error +=  vcl_fabs(dw2-diw2_23);
  error /= 6;
TEST_NEAR("Map from image to world average distance error", error, 0.0, 0.2);
}

TESTMAIN(test_camera_homographies);
