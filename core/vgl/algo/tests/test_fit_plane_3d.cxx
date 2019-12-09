// Some tests for vgl_fit_plane_3d
// Gamze Tunali, Dec 18, 2006.
#include <iostream>
#include <cstdlib>
#include "testlib/testlib_test.h"
#include <vgl/algo/vgl_fit_plane_3d.h>
#include "vgl/vgl_distance.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

// creates points on the z=0 plane
static void
test_fit_plane_z()
{
  std::cout << "Fit Points to a plane\n";
  std::vector<vgl_homg_point_3d<double>> points;
  vgl_fit_plane_3d<double>               fit_plane;

  // add points in the z=0 plane
  for (unsigned i = 0; i < 10; i++)
  {
    for (unsigned j = 0; j < 10; j++)
    {
      vgl_homg_point_3d<double> p(i, j, 0);
      fit_plane.add_point(p);
    }
  }
  double tol = 1e-3;
  bool   success = fit_plane.fit(tol, &std::cerr);
  TEST("exact fit", success, true);
  vgl_homg_plane_3d<double> plane = fit_plane.get_plane();

  // test if origin is on the plane
  double d = vgl_distance_origin(plane);
  TEST_NEAR("on the plane", d, 0, 1e-6);

  d = vgl_distance(plane, vgl_homg_point_3d<double>(0, 10, 0));
  TEST_NEAR("on the plane", d, 0, 1e-6);

  d = vgl_distance(plane, vgl_homg_point_3d<double>(10, 0, 0));
  TEST_NEAR("on the plane", d, 0, 1e-6);

  d = vgl_distance(plane, vgl_homg_point_3d<double>(10, 10, 0));
  TEST_NEAR("on the plane", d, 0, 1e-6);
}

// creates points a on the x+3y+2z=5 plane
static void
test_fit_plane_xyz()
{
  std::cout << "Fit Points to a plane\n";
  std::vector<vgl_homg_point_3d<double>> points;
  vgl_fit_plane_3d<double>               fit_plane;
  double                                 a = 1, b = 3, c = 2, d = -5;

  {
    for (int x = -5; x <= 5; x++)
    {
      for (int y = -5; y <= 5; y++)
      {
        // compute z from the plane formula
        double                    z = (-1 * d - a * x - b * y) / c;
        vgl_homg_point_3d<double> p(x, y, z);
        fit_plane.add_point(p);
      }
    }
  }
  double tol = 1e-3;
  bool   success = fit_plane.fit(tol, &std::cerr);
  TEST("test fit", success, true);
  vgl_homg_plane_3d<double> plane = fit_plane.get_plane();

  // test if origin is on the plane
  // get a point on the plane and test if it is on it
  double y = 11, z = 20;
  double x = (-1 * d - b * y - c * z) / a;
  double dist = vgl_distance(plane, vgl_homg_point_3d<double>(x, y, z));
  TEST_NEAR("on the plane", dist, 0, 1e-6);
}

// creates 20 random points
static void
test_fit_plane_random()
{
  std::cout << "Fit random points to a plane\n";
  std::vector<vgl_homg_point_3d<double>> points;
  vgl_fit_plane_3d<double>               fit_plane;

  for (int x = 0; x < 20; ++x)
  {
    double                    r1 = std::rand() / (RAND_MAX + 1.0);
    double                    r2 = std::rand() / (RAND_MAX + 1.0);
    double                    r3 = std::rand() / (RAND_MAX + 1.0);
    vgl_homg_point_3d<double> p(r1, r2, r3);
    fit_plane.add_point(p);
  }
  double tol = 1e-3;
  bool   success = fit_plane.fit(tol, &std::cerr);
  TEST("random fit", success, false);
  vgl_homg_plane_3d<double> plane = fit_plane.get_plane();

  std::cout << plane;
}

static void
test_fit_plane_3d()
{
  test_fit_plane_z();
  test_fit_plane_xyz();
  test_fit_plane_random();
}

TESTMAIN(test_fit_plane_3d);
