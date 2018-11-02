// This is core/vsl/tests/test_string_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_string_io.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_string_io()
{
  std::cout << "****************************\n"
           << "Testing std::string binary io\n"
           << "****************************\n";

  std::string outstring("This is a test");

  vsl_b_ofstream bfs_out("vsl_string_io_test.bvl.tmp");
  TEST("Created vsl_string_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, outstring);
  bfs_out.close();

  std::string instring;

  vsl_b_ifstream bfs_in("vsl_string_io_test.bvl.tmp");
  TEST("Opened vsl_string_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, instring);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_string_io_test.bvl.tmp");

  TEST("std::string<int> out == std::string<int> in", outstring, instring);
}

TESTMAIN(test_string_io);
