// This is vxl/vgl/io/tests/test_line_3d_2_points_io.cxx
#include <vcl_iostream.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/io/vgl_io_line_3d_2_points.h>
#include <testlib/testlib_test.h>

void test_line_3d_2_points_float_io()
{
  vcl_cout << "**************************************\n"
           << "Testing vgl_line_3d_2_points<float> io\n"
           << "**************************************\n";

  //// test constructors, accessors
  vgl_point_3d<float> p1(1.25f, 3.5f, -99.f), p2(5.625f, 7.875f, 0.0f);
  vgl_line_3d_2_points<float> p_out(p1, p2), p_in;

  vsl_b_ofstream bfs_out("vgl_line_3d_2_points_test_float_io.bvl.tmp");
  TEST ("Writing to vgl_line_3d_2_points_test_float_io.bvl.tmp",
        (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  TEST ("Finished writing file successfully", (!bfs_out), false);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_line_3d_2_points_test_float_io.bvl.tmp");
  TEST ("Opened vgl_line_3d_2_points_test_float_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("p1 == p_in.point1()", p1, p_in.point1());
  TEST ("p2 == p_in.point2()", p2, p_in.point2());
  TEST ("p_out == p_in", p_out, p_in);

  vsl_print_summary(vcl_cout, p_out);
  vcl_cout << '\n';
}


void test_line_3d_2_points_io()
{
  test_line_3d_2_points_float_io();
}

TESTMAIN(test_line_3d_2_points_io);
