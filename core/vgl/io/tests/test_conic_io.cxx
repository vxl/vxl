// This is core/vgl/io/tests/test_conic_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_indent.h>
#include <vgl/io/vgl_io_conic.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_conic_double_io()
{
  std::cout << "****************************\n"
           << "Testing vgl_conic<double> io\n"
           << "****************************\n";

  //// test constructors, accessors
  vgl_conic<double> p_out(1.25,-3.5,5.75,-6.5,4.25,2.0), p_in;

  vsl_b_ofstream bfs_out("vgl_conic_test_double_io.bvl.tmp");
  TEST ("Created vgl_conic_test_double_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_conic_test_double_io.bvl.tmp");
  TEST ("Opened vgl_conic_test_double_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vgl_conic_test_double_io.bvl.tmp");

  TEST ("p_out == p_in", p_out, p_in);

  vsl_print_summary(std::cout, p_out);
  std::cout << std::endl;
  vsl_indent_clear_all_data();
}

void test_conic_float_io()
{
  std::cout << "***************************\n"
           << "Testing vgl_conic<float> io\n"
           << "***************************\n";

  //// test constructors, accessors
  vgl_conic<double> p_out(1.25f,-3.5f,5.75f,-6.5f,4.25f,2.0f), p_in;

  vsl_b_ofstream bfs_out("vgl_conic_test_float_io.bvl.tmp");
  TEST ("Created vgl_conic_test_float_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_conic_test_float_io.bvl.tmp");
  TEST ("Opened vgl_conic_test_float_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vgl_conic_test_float_io.bvl.tmp");

  TEST ("p_out == p_in", p_out, p_in);

  vsl_print_summary(std::cout, p_out);
  std::cout << std::endl;
  vsl_indent_clear_all_data();
}

void test_conic_io()
{
  test_conic_double_io();
  test_conic_float_io();
}

TESTMAIN(test_conic_io);
