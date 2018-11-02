// This is core/vsl/tests/test_set_io.cxx
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_set_io.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_set_io()
{
  std::cout << "*************************\n"
           << "Testing std::set binary io\n"
           << "*************************\n";

  int n = 10;
  std::set<int> s_int_out;
  for (int i=0;i<n;++i)
    s_int_out.insert(i);

  std::set<std::string> s_string_out;
  s_string_out.insert("one");
  s_string_out.insert("two");
  s_string_out.insert("three");
  s_string_out.insert("four");

  vsl_b_ofstream bfs_out("vsl_set_io_test.bvl.tmp");
  TEST("Created vsl_set_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, s_int_out);
  vsl_b_write(bfs_out, s_string_out);
  bfs_out.close();

  std::set<int> s_int_in;
  std::set<std::string> s_string_in;

  vsl_b_ifstream bfs_in("vsl_set_io_test.bvl.tmp");
  TEST("Opened vsl_set_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, s_int_in);
  vsl_b_read(bfs_in, s_string_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_set_io_test.bvl.tmp");

  TEST("std::set<int> out == in", s_int_out, s_int_in);
  TEST("std::set<std::string> out == in", s_string_out, s_string_in);
  vsl_print_summary(std::cout, s_string_in);
  std::cout << std::endl;
}

TESTMAIN(test_set_io);
