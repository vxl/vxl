// This is core/vsl/tests/test_complex_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_complex_io.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_complex_io()
{
  std::cout << "*****************************\n"
           << "Testing std::complex binary io\n"
           << "*****************************\n";

  std::complex<float> c_float_out(1.23f, 4.56f);

  vsl_b_ofstream bfs_out("vsl_complex_io_test.bvl.tmp");
  TEST("Created vsl_complex_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, c_float_out);
  bfs_out.close();

  std::complex<float> c_float_in;

  vsl_b_ifstream bfs_in("vsl_complex_io_test.bvl.tmp");
  TEST("Opened vsl_complex_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, c_float_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_complex_io_test.bvl.tmp");

  TEST("std::complex<float> out == in", c_float_out, c_float_in);

  vsl_print_summary(std::cout, c_float_in);
  std::cout << std::endl;
}

TESTMAIN(test_complex_io);
