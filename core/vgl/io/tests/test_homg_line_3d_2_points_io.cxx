#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <testlib/testlib_test.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/io/vgl_io_homg_line_3d_2_points.h>

void test_homg_line_3d_2_points_double_io()
{
  vcl_cout << "***********************\n"
           << "Testing vgl_homg_line_3d_2_points<double> io\n"
           << "***********************\n";

  //// test constructors, accessors
  vgl_homg_point_3d<float> p1(1.25f, 3.5f, -9.9e1f), p2(5.625f, 7.875f, 0.0f);
  vgl_homg_line_3d_2_points<float> p_out(p1, p2), p_in;

  vsl_b_ofstream bfs_out("vgl_homg_line_3d_2_points_test_double_io.bvl.tmp");
  TEST ("Made vgl_homg_line_3d_2_points_test_double_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_homg_line_3d_2_points_test_double_io.bvl.tmp");
  TEST ("Opened vgl_homg_line_3d_2_points_test_double_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("p_out == p_in", p_out, p_in);

  vsl_print_summary(vcl_cout, p_out);
  vcl_cout << vcl_endl;
}


void test_homg_line_3d_2_points_io()
{
  test_homg_line_3d_2_points_double_io();
}

TESTMAIN(test_homg_line_3d_2_points_io);
