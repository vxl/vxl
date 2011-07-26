// This is a minimal test suite for vgl_ellipsoid
#include <testlib/testlib_test.h>
#include <vgl/algo/vgl_ellipsoid_3d.h>

static void test_ellipsoid()
{
  // 1. Test constructors
  vcl_cout << "\n\t=== test constructors ===\n";
  // sphere, centre (1,2,3), axes lengths 2,2,2
  vgl_ellipsoid_3d<double> e(vgl_point_3d<double>(1,2,3), 2, 2, 2);
  vgl_ellipsoid_3d<double> ee(vgl_point_3d<double>(1,2,3), 2, 2, 2, vgl_rotation_3d<double>());
  TEST("ellipsoid equality", e,ee);

  // 2. Degenerate ellipsoids
}

TESTMAIN(test_ellipsoid);
