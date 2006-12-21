// Some tests for vgl_fit_plane_3d
// Gamze Tunali, Dec 18, 2006.
#include <testlib/testlib_test.h>
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for rand()

// creates points on the z=0 plane
static void test_fit_plane_z()
{
  vcl_cout << "Fit Points to a plane\n";
  vcl_vector<vgl_homg_point_3d<double> > points;
  vgl_fit_plane_3d<double> fit_plane;

  // add points in the z=0 plane
  for (unsigned i=0; i<10; i++) {
    for (unsigned j=0; j<10; j++) {
      vgl_homg_point_3d<double> p(i, j, 0);
      fit_plane.add_point(p);
    }
  }
  fit_plane.fit();
  vgl_homg_plane_3d<double> plane = fit_plane.get_plane();

  // test if origin is on the plane
  double d = vgl_distance_origin (plane); 
  TEST_NEAR("on the plane", d, 0, 1e-6);

  d = vgl_distance(plane, vgl_homg_point_3d<double> (0, 10, 0)); 
  TEST_NEAR("on the plane", d, 0, 1e-6);

  d = vgl_distance(plane, vgl_homg_point_3d<double> (10, 0, 0)); 
  TEST_NEAR("on the plane", d, 0, 1e-6);

  d = vgl_distance(plane, vgl_homg_point_3d<double> (10, 10, 0)); 
  TEST_NEAR("on the plane", d, 0, 1e-6);
}

// creates points a on the x+3y+2z=5 plane
static void test_fit_plane_xyz()
{
  vcl_cout << "Fit Points to a plane\n";
  vcl_vector<vgl_homg_point_3d<double> > points;
  vgl_fit_plane_3d<double> fit_plane;
  double a=1, b=3, c=2, d=-5;
  
  for (int x=-5; x<=5; x++) {
    for (int y=-5; y<=5; y++) {
      // compute z from the plane formula
      double z = (-1*d - a*x - b*y)/c;
      double r1 = vcl_rand()/(RAND_MAX+1.0f);
      double r2 = vcl_rand()/(RAND_MAX+1.0f);
      double r3 = vcl_rand()/(RAND_MAX+1.0f);
      vgl_homg_point_3d<double> p(x, y, z);
      fit_plane.add_point(p);
    }
  }
  fit_plane.fit();
  vgl_homg_plane_3d<double> plane = fit_plane.get_plane();

  // test if origin is on the plane
  // get a point on the plane and test if it is on it
  double y=11, z=20;
  double x = (-1*d - b*y - c*z)/a;
  double dist = vgl_distance(plane, vgl_homg_point_3d<double> (x, y, z)); 
  TEST_NEAR("on the plane", dist, 0, 1e-6);
}

static void test_fit_plane_3d() 
{
  test_fit_plane_z();
  test_fit_plane_xyz();
}

TESTMAIN(test_fit_plane_3d);
