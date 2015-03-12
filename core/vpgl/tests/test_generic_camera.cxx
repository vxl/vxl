#include <testlib/testlib_test.h>

#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>

static void simple_test()
{
  //construct a 5x7 image to test pyramid
  vbl_array_2d<vgl_ray_3d<double> > img(5, 7);
  // simple rays along -z
  vgl_vector_3d<double> dir(0.0, 0.0, -1.0);
  for (int r =0; r<5; ++r)
    for (int c =0; c<7; ++c)
    {
      vgl_point_3d<double> p(c, r, 10.0);// 10 above the ground plane
      img[r][c]=vgl_ray_3d<double>(p, dir);
    }
  vpgl_generic_camera<double> c(img);
#if 0
  for (int i = 0; i<2; ++i) {
    vcl_cout << "level " << i << '\n';
    c.print_orig(i);
  }
#endif
  vgl_ray_3d<double> interp_ray = c.ray(3.5, 2.5);
  vgl_point_3d<double> org = interp_ray.origin();
  double er = vcl_fabs(org.x()-3.5) + vcl_fabs(org.y()-2.5);
  TEST_NEAR("interpolated ray", er, 0.0, 0.0001);
  double x = 1.5, y = 2.5, z = 0.0, u = 0.0, v= 0.0;
  c.project(x, y, z, u, v);
  er = vcl_fabs(u-1.5) + vcl_fabs(v-2.5);
  x = 3.2, y = 4.1, z = 1.0;
  double u1, v1;
  c.project(x, y, z, u1, v1);
  er += vcl_fabs(u1-3.2) + vcl_fabs(v1-4.1);
  x = 6.8; y = 5.2; z = 0.0;
  double u2, v2;
  c.project(x, y, z, u2, v2);
  er += vcl_fabs(u2-6.8) + vcl_fabs(v2-5.2);
  TEST_NEAR("project", er, 0.0, 0.0001);
}


static void proj_test()
{
  unsigned ni = 640;
  unsigned nj = 480;
  // construct a perspective camera for reference
  vpgl_calibration_matrix<double> K(ni, vgl_point_2d<double>((double)ni/2.0, (double)nj/2.0));
  vgl_point_3d<double> center(10.0, 5.0, 15.0);
  vgl_rotation_3d<double> R;
  vpgl_perspective_camera<double> pcam(K, center, R);

  // set each ray in the generic camera to be same as perspective camera
  vbl_array_2d<vgl_ray_3d<double> > rays(nj,ni);
  for (unsigned j=0; j<nj; ++j) {
    for (unsigned i=0; i<ni; ++i) {
      rays(j,i) = pcam.backproject_ray(i, j); 
    }
  }
  vpgl_generic_camera<double> gcam(rays);

  // project some random 3-d points into the image
  vcl_vector<vgl_point_3d<double> > test_pts;
  vgl_vector_3d<double> offset(center.x(), center.y(), center.z());
  test_pts.push_back(vgl_point_3d<double>(-4.2, -1.0, 10.9)+offset);
  test_pts.push_back(vgl_point_3d<double>(-4.0, 3.4, 9.1)+offset);
  test_pts.push_back(vgl_point_3d<double>(4.0, 2.9, 11.3)+offset);
  test_pts.push_back(vgl_point_3d<double>(3.5, -2.0, 10.2)+offset);
  test_pts.push_back(vgl_point_3d<double>(1.3, 0.1, 5.4)+offset);
  test_pts.push_back(vgl_point_3d<double>(0.8, 3.0, 10.6)+offset);
  test_pts.push_back(vgl_point_3d<double>(-1.5, -2.1, 10.0)+offset);
  test_pts.push_back(vgl_point_3d<double>(0.1, 0.7, 5.8)+offset);

  for (vcl_vector<vgl_point_3d<double> >::const_iterator pit = test_pts.begin();
       pit != test_pts.end(); ++pit) {
    // first project using perspective camera
    vgl_point_2d<double> p2d = pcam.project(*pit);
    // project same point using generic camera
    double u,v;
    gcam.project(pit->x(), pit->y(), pit->z(), u, v);
    vcl_stringstream testname;
    testname << "Projection of test point " << *pit << ": u" << vcl_endl;
    TEST_NEAR(testname.str().c_str(), u, p2d.x(), 1e-3);
    testname.str("");
    testname << "Projection of test point " << *pit << ": v" << vcl_endl;
    TEST_NEAR(testname.str().c_str(), v, p2d.y(), 1e-3);
  }


  vcl_vector<vgl_point_2d<double> > test_pts1;
  test_pts1.push_back(vgl_point_2d<double>(639.496, 97.5777));
  test_pts1.push_back(vgl_point_2d<double>(280.9, 1.9)); // original failing point (interior)
  // exterior points
  test_pts1.push_back(vgl_point_2d<double>(0.5, -0.5));
  test_pts1.push_back(vgl_point_2d<double>(-0.5, -0.5));
  test_pts1.push_back(vgl_point_2d<double>(-0.5, 0.5));
  test_pts1.push_back(vgl_point_2d<double>(638.5, -0.5));
  test_pts1.push_back(vgl_point_2d<double>(639.5, -0.5));
  test_pts1.push_back(vgl_point_2d<double>(639.5, 0.0));
  test_pts1.push_back(vgl_point_2d<double>(639.5, 0.5));
  test_pts1.push_back(vgl_point_2d<double>(-0.5, 478.5));
  test_pts1.push_back(vgl_point_2d<double>(-0.5, 479.5));
  test_pts1.push_back(vgl_point_2d<double>(0.5, 479.5));
  test_pts1.push_back(vgl_point_2d<double>(638.5, 479.5));
  test_pts1.push_back(vgl_point_2d<double>(639.5, 479.5));
  test_pts1.push_back(vgl_point_2d<double>(638.5, 479.5));

  for (vcl_vector<vgl_point_2d<double> >::const_iterator pit = test_pts1.begin();
       pit != test_pts1.end(); ++pit) {
    double u=pit->x(), v=pit->y();
    vgl_ray_3d<double> ray = pcam.backproject_ray(u, v);
    vgl_ray_3d<double> r = gcam.ray(u, v);

    vcl_cout << vcl_endl;
    vcl_stringstream testname;
    testname << "Ray origin for test point " << *pit << ": x" << vcl_endl;
    TEST_NEAR(testname.str().c_str(), r.origin().x(), ray.origin().x(), 1e-3);
    testname.str("");
    testname << "Ray origin for test point " << *pit << ": y" << vcl_endl;
    TEST_NEAR(testname.str().c_str(), r.origin().y(), ray.origin().y(), 1e-3);
    testname.str("");
    testname << "Ray origin for test point " << *pit << ": z" << vcl_endl;
    TEST_NEAR(testname.str().c_str(), r.origin().z(), ray.origin().z(), 1e-3);
    testname.str("");
    testname << "Ray direction for test point " << *pit << ": dx" << vcl_endl;
    TEST_NEAR(testname.str().c_str(), r.direction().x(), ray.direction().x(), 1e-3);
    testname.str("");
    testname << "Ray direction for test point " << *pit << ": dy" << vcl_endl;
    TEST_NEAR(testname.str().c_str(), r.direction().y(), ray.direction().y(), 1e-3);
    testname.str("");
    testname << "Ray direction for test point " << *pit << ": dz" << vcl_endl;
    TEST_NEAR(testname.str().c_str(), r.direction().z(), ray.direction().z(), 1e-3);
  }
}


static void test_generic_camera()
{
  simple_test();
  proj_test();
}

TESTMAIN(test_generic_camera);

