#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_project.h>
#include <vpgl/algo/vpgl_camera_from_box.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgl/vgl_homg_line_3d_2_points.h>

static void test_camera_from_box()
{
  vgl_point_3d<double> p0(0, 0, 0), p1(1, 1, 1);
  vgl_box_3d<double> box;
  box.add(p0); box.add(p1);
  vgl_point_3d<double> cent = box.centroid();
  unsigned ni = 150, nj = 100;
  double sq3 = 1.0/vcl_sqrt(3.0);
  vgl_vector_3d<double> ray(-sq3, -sq3, -sq3);
  vpgl_affine_camera<double> C = vpgl_camera_from_box::affine_camera_from_box(box, ray, ni, nj);
  vgl_box_2d<double> b2d = vpgl_project::project_bounding_box(C, box);
  vgl_point_2d<double> pmin = b2d.min_point(), pmax = b2d.max_point();
  vgl_point_2d<double> imin(0,0), imax(150, 100);
  double len = (pmin-imin).length() + (pmax-imax).length();
  TEST_NEAR("affine_cam_from_box", len, 0.0, 1e-6); 
  vgl_homg_point_2d<double> ph(ni/2.0+10, nj/2.0+10);
  vgl_homg_line_3d_2_points<double> line = C.backproject(ph);
  vgl_point_3d<double> cam_center(10, 10, 10);
  vpgl_perspective_camera<double> Cp = vpgl_camera_from_box::persp_camera_from_box(box, cam_center, ni, nj);
  vgl_box_2d<double> b2dp = vpgl_project::project_bounding_box(Cp, box);
  vgl_point_2d<double> c2d = b2dp.centroid();
  pmin = b2dp.min_point(); pmax = b2dp.max_point();
  len = (pmin-imin).length() + (pmax-imax).length() + (c2d-vgl_point_2d<double>(ni/2.0, nj/2.0)).length();
  TEST_NEAR("Test perspective cam from box", len, 0.0, 0.005);
}

TESTMAIN(test_camera_from_box);
