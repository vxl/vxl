
// First define testmain

#include <vgl/vgl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)


#include <vgl/io/tests/test_homg_plane_3d_io.cxx>
#include <vgl/io/tests/test_point_2d_io.cxx>
#include <vgl/io/tests/test_point_3d_io.cxx> 
#include <vgl/io/tests/test_line_segment_2d_io.cxx>
#include <vgl/io/tests/test_line_segment_3d_io.cxx>
#include <vgl/io/tests/test_line_2d_io.cxx>
#include <vgl/io/tests/test_homg_line_2d_io.cxx>
#include <vgl/io/tests/test_box_2d_io.cxx>
#include <vgl/io/tests/test_box_3d_io.cxx>
#include <vgl/io/tests/test_homg_point_2d_io.cxx>
#include <vgl/io/tests/test_homg_point_3d_io.cxx>
#include <vgl/io/tests/test_homg_line_3d_2_points_io.cxx>
#include <vgl/io/tests/test_polygon_io.cxx>
#include <vgl/io/tests/golden_test_vgl_io.cxx>


#undef TESTMAIN
#define TESTMAIN(x) int main() \
  { vgl_test_start(#x); x(); return vgl_test_summary(); }

void run_test_vgl_io()
{
  test_homg_line_3d_2_points_double_io();
  test_point_2d_double_io();
  test_point_3d_double_io();
  test_homg_plane_3d_double_io();
  test_homg_point_2d_double_io();
  test_homg_point_3d_double_io();
  test_line_segment_2d_double_io();
  test_line_segment_3d_double_io();
  test_line_2d_double_io();
  test_homg_line_2d_double_io();
  test_box_3d_double_io();
  test_box_2d_double_io();
  test_polygon_io();
  golden_test_vgl_io();

}


TESTMAIN(run_test_vgl_io);
