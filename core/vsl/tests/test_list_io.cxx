// This is core/vsl/tests/test_list_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_list_io.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_list_io()
{
  std::cout << "**************************\n"
           << "Testing std::list binary io\n"
           << "**************************\n";

  int n = 10;
  std::list<int> l_int_out;
  for (int i=0;i<n;++i)
    l_int_out.push_back(i);
  std::list<float> l_float_out;
  for (int i=0;i<n;++i)
    l_float_out.push_back(0.1f*i);

  vsl_b_ofstream bfs_out("vsl_list_io_test.bvl.tmp");
  TEST("Created vsl_list_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, l_int_out);
  vsl_b_write(bfs_out, l_float_out);
  bfs_out.close();

  std::list<int> l_int_in;
  std::list<float> l_float_in;

  vsl_b_ifstream bfs_in("vsl_list_io_test.bvl.tmp");
  TEST("Opened vsl_list_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, l_int_in);
  vsl_b_read(bfs_in, l_float_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_list_io_test.bvl.tmp");

  TEST("std::list<int> out == std::list<int> in", l_int_out, l_int_in);
  TEST("std::list<float> out == std::list<float> in", l_float_out, l_float_in);

  vsl_print_summary(std::cout, l_int_in);
  std::cout << std::endl;
}

TESTMAIN(test_list_io);
