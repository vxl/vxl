#include <testlib/testlib_test.h>
#include <vsph/vsph_unit_sphere.h>
#include <vsph/vsph_sph_point_2d.h>
#include <vsph/vsph_sph_box_2d.h>
#include <vsph/vsph_utils.h>
#include <vgl/vgl_polygon.h>
#include <bpgl/bpgl_camera_utils.h>
#include <vpl/vpl.h>


static void test_utils()
{
  unsigned ni = 1280, nj = 720;
  auto nid = static_cast<double>(ni), njd = static_cast<double>(nj);
  double heading = 180.0, tilt = 90.0, roll =0.0, altitude = 1.0;
  double top_fov = 10.0;
  double right_fov = 17.47;
  vpgl_perspective_camera<double> cam =
    bpgl_camera_utils::camera_from_kml(ni, nj, right_fov, top_fov,altitude,
                                       heading, tilt, roll);

    double elevation = 0.0, azimuth = 0.0;
  // ==============   test ray direction on unit sphere ======
  vsph_utils::ray_spherical_coordinates(cam, 640.0, 360.0, elevation,
                                        azimuth, "degrees");
  double er = std::fabs(elevation - (180.0-tilt));
  er += std::fabs(azimuth - (90.0 -heading));
  TEST_NEAR("ray spherical coordinates", er, 0.0, 0.001);

  // ================= test project poly onto sphere =======
  // remove roll for poly angle tests
  roll = 0.0;
  vpgl_perspective_camera<double> cam0 =
    bpgl_camera_utils::camera_from_kml(nid, njd, right_fov, top_fov,
                                       altitude, heading, tilt, roll);

  vgl_point_2d<double> p0(640.0, 0.0), p1(1280.0, 0.0);
  vgl_point_2d<double> p2(1280.0, 360.0), p3(640.0, 360.0);//<==principal pt
  std::vector<vgl_point_2d<double> > sheet;
  sheet.push_back(p0);  sheet.push_back(p1);
  sheet.push_back(p2);  sheet.push_back(p3);
  vgl_polygon<double> image_poly, sph_poly;
  image_poly.push_back(sheet);
  std::cout << "in image\n" << image_poly << '\n';
  sph_poly =
   vsph_utils::project_poly_onto_unit_sphere(cam0, image_poly, "degrees");
  std::cout << "on sphere\n" << sph_poly << '\n';
  vgl_point_2d<double> sp0 = sph_poly[0][0];
  vgl_point_2d<double> sp2 = sph_poly[0][2];
  vgl_point_2d<double> sp3 = sph_poly[0][3];
  //x is elevation and y is azimuth
  double t_fov = std::fabs(sp0.x()-sp3.x());
  double r_fov = std::fabs(sp2.y()-sp3.y());
  // shouldn't expect exact recovery since focal length is
  // the average of top and right fov
  er = std::fabs(t_fov-top_fov) + std::fabs(r_fov-right_fov);
  TEST_NEAR("spherical polygon ", er, 0.0, 1.0);//1 degree
}

TESTMAIN(test_utils);
