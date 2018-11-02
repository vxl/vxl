// This is core/vgl/io/tests/test_homg_point_1d_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_homg_point_1d.h>
#include <vsl/vsl_indent.h>
#include <vgl/io/vgl_io_homg_point_1d.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_homg_point_1d_float_io()
{
  std::cout << "***********************************\n"
           << "Testing vgl_homg_point_1d<float> io\n"
           << "***********************************\n";

  //// test constructors, accessors
  vgl_homg_point_1d<float> p_out(1.25f,3.75f), p_in;

  vsl_b_ofstream bfs_out("vgl_homg_point_1d_test_float_io.bvl.tmp");
  TEST ("Created vgl_homg_point_1d_test_float_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_homg_point_1d_test_float_io.bvl.tmp");
  TEST ("Opened vgl_homg_point_1d_test_float_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vgl_homg_point_1d_test_float_io.bvl.tmp");

  TEST ("p_out.x() == p_in.x()", p_out.x(), p_in.x());
  TEST ("p_out.w() == p_in.w()", p_out.w(), p_in.w());
  TEST ("p_out == p_in", p_out, p_in);

  vsl_print_summary(std::cout, p_out);
  std::cout << '\n';
  vsl_indent_clear_all_data();
}

void test_homg_point_1d_io()
{
  test_homg_point_1d_float_io();
}

TESTMAIN(test_homg_point_1d_io);
