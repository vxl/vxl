#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <bpgl/algo/bpgl_project.h>
#include <bpgl/algo/bpgl_camera_from_box.h>
#include <vgl/vgl_homg_line_3d_2_points.h>

static void test_camera_from_box()
{
  vgl_point_3d<double> p0(0, 0, 0), p1(1, 1, 1);
  vgl_box_3d<double> box;
  box.add(p0); box.add(p1);
  vgl_point_3d<double> cent = box.centroid();
  TEST("Centroid", cent.x(), 0.5);
  unsigned ni = 150, nj = 100;
  double sq3 = 1.0/std::sqrt(3.0);
  vgl_vector_3d<double> ray(-sq3, -sq3, -sq3);
  vpgl_affine_camera<double> C = bpgl_camera_from_box::affine_camera_from_box(box, ray, ni, nj);
  vgl_box_2d<double> b2d = bpgl_project::project_bounding_box(C, box);
  vgl_point_2d<double> pmin = b2d.min_point(), pmax = b2d.max_point();
  vgl_point_2d<double> imin(0,0), imax(150, 100);
  double len = (pmin-imin).length() + (pmax-imax).length();
  TEST_NEAR("affine_cam_from_box", len, 0.0, 1e-6);
  vgl_homg_point_2d<double> ph(ni/2.0+10, nj/2.0+10);
  vgl_homg_line_3d_2_points<double> line = C.backproject(ph);
  TEST_NEAR("Backproject", line.point_infinite().x(), -0.57735, 0.1);
  vgl_point_3d<double> cam_center(10, 10, 10);
  vpgl_perspective_camera<double> Cp = bpgl_camera_from_box::persp_camera_from_box(box, cam_center, ni, nj);
  vgl_box_2d<double> b2dp = bpgl_project::project_bounding_box(Cp, box);
  vgl_point_2d<double> c2d = b2dp.centroid();
  pmin = b2dp.min_point(); pmax = b2dp.max_point();
  len = (pmin-imin).length() + (pmax-imax).length() + (c2d-vgl_point_2d<double>(ni/2.0, nj/2.0)).length();
  TEST_NEAR("Test perspective cam from box", len, 0.0, 0.005);

  vgl_point_3d<double> bmin(350, 350, 0), bmax(550, 550, 100);
  vgl_box_3d<double> sun_box;
  sun_box.add(bmin); sun_box.add(bmax);
  vgl_vector_3d<double> sun_ray(-0.29355871677398682, 0.76474428176879883, -0.57357603311538696);
  ni = 256; nj = 256;
  vpgl_affine_camera<double> sun_cam = bpgl_camera_from_box::affine_camera_from_box(sun_box, sun_ray, ni, nj);
  sun_cam.set_viewing_distance(300000);
  vgl_box_2d<double> sun_b2d = bpgl_project::project_bounding_box(sun_cam, sun_box);
  TEST_NEAR("Project bounding box", sun_b2d.centroid().x(), 128, 10);
  vgl_homg_point_2d<double> sun_ph(ni/2.0, nj/2.0);
  vgl_homg_line_3d_2_points<double> sun_line = sun_cam.backproject(sun_ph);
  vgl_homg_point_3d<double> pi = sun_line.point_infinite();
  vgl_vector_3d<double> vpi(pi.x(), pi.y(), pi.z());
  len = (sun_ray-vpi).length();
  TEST_NEAR("Affine camera from sun", len, 0.0, 1e-6);
}

TESTMAIN(test_camera_from_box);
