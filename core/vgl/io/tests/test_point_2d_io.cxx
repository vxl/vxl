// This is core/vgl/io/tests/test_point_2d_io.cxx
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/io/vgl_io_point_2d.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_point_2d_double_io()
{
  vcl_cout << "*******************************\n"
           << "Testing vgl_point_2d<double> io\n"
           << "*******************************\n";

  //// test constructors, accessors
  vgl_point_2d<double> p_out(1.2,3.4), p_in;

  vsl_b_ofstream bfs_out("vgl_point_2d_test_double_io.bvl.tmp");
  TEST("Created vgl_point_2d_test_double_io.bvl.tmp for writing",
       (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_point_2d_test_double_io.bvl.tmp");
  TEST ("Opened vgl_point_2d_test_double_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vgl_point_2d_test_double_io.bvl.tmp");

  TEST ("p_out == p_in", p_out == p_in, true);

  vsl_print_summary(vcl_cout, p_out);
  vcl_cout << vcl_endl;
  vsl_indent_clear_all_data();
}

void test_point_2d_io()
{
  test_point_2d_double_io();
}

TESTMAIN(test_point_2d_io);
