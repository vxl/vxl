#include <testlib/testlib_register.h>

DECLARE(test_vsol_conic_2d);
DECLARE(test_vsol_group_2d);
DECLARE(test_vsol_group_3d);
DECLARE(test_vsol_line_2d);
DECLARE(test_vsol_line_3d);
DECLARE(test_vsol_point_2d);
DECLARE(test_vsol_point_3d);
DECLARE(test_vsol_polygon_2d);
DECLARE(test_vsol_polygon_3d);
DECLARE(test_vsol_rectangle_2d);
DECLARE(test_vsol_rectangle_3d);
DECLARE(test_vsol_tetrahedron);
DECLARE(test_vsol_triangle_2d);
DECLARE(test_vsol_triangle_3d);
DECLARE(test_vsol_io);

void
register_tests()
{
  REGISTER(test_vsol_conic_2d);
  REGISTER(test_vsol_group_2d);
  REGISTER(test_vsol_group_3d);
  REGISTER(test_vsol_line_2d);
  REGISTER(test_vsol_line_3d);
  REGISTER(test_vsol_point_2d);
  REGISTER(test_vsol_point_3d);
  REGISTER(test_vsol_polygon_2d);
  REGISTER(test_vsol_polygon_3d);
  REGISTER(test_vsol_rectangle_2d);
  REGISTER(test_vsol_rectangle_3d);
  REGISTER(test_vsol_tetrahedron);
  REGISTER(test_vsol_triangle_2d);
  REGISTER(test_vsol_triangle_3d);
  REGISTER(test_vsol_io);
}

DEFINE_MAIN;
