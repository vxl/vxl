// This is core/vgl/io/tests/test_point_2d_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/io/vgl_io_point_2d.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_point_2d_double_io()
{
  std::cout << "*******************************\n"
           << "Testing vgl_point_2d<double> io\n"
           << "*******************************\n";

  // Test reading in a space delimited point.
  std::string point_with_spaces = "vgl_point_2d_test_spaces.tmp";
  std::ofstream t_out(point_with_spaces.c_str());
  vgl_point_2d<double> truth(1.2, 3.4);
  t_out << "1.2 3.4";
  t_out.close();
  std::ifstream t_in(point_with_spaces.c_str());
  vgl_point_2d<double> tmp;
  t_in >> tmp;
  vpl_unlink (point_with_spaces.c_str());
  TEST ("Finished reading space delimited point file successfully", (!t_in.fail()), true);
  TEST("point successfully read from space delimited file", tmp == truth, true);

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

  vsl_print_summary(std::cout, p_out);
  std::cout << std::endl;
  vsl_indent_clear_all_data();
}

void test_point_2d_io()
{
  test_point_2d_double_io();
}

TESTMAIN(test_point_2d_io);
