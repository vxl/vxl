// This is core/vsl/tests/test_tuple_io.cxx
#include <iostream>
#include <string>
#include <tuple>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vsl/vsl_binary_io.h"
#include "testlib/testlib_test.h"
#include "testlib/testlib_root_dir.h"
#include "vpl/vpl.h"

#include "vsl/vsl_tuple_io.hxx"


VSL_TUPLE_IO_INSTANTIATE(int);
VSL_TUPLE_IO_INSTANTIATE(int, double, std::string);
VSL_TUPLE_IO_INSTANTIATE(std::tuple<int>);


void
test_tuple_io()
{
  std::cout << "****************************\n"
            << "Testing std::tuple binary io\n"
            << "****************************\n";

  int x1 = 7;
  std::tuple<int> t_int_out(x1);

  double x2 = 2.5;
  std::string x3 = "test";
  std::tuple<int, double, std::string> t_int_double_string_out(x1, x2, x3);

  std::tuple<std::tuple<int>> t_t_int_out(t_int_out);

  vsl_b_ofstream bfs_out("vsl_tuple_io_test.bvl.tmp");
  TEST("Created vsl_tuple_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, t_int_out);
  vsl_b_write(bfs_out, t_int_double_string_out);
  vsl_b_write(bfs_out, t_t_int_out);
  bfs_out.close();

  std::tuple<int> t_int_in;
  std::tuple<int, double, std::string> t_int_double_string_in;
  std::tuple<std::tuple<int>> t_t_int_in;

  vsl_b_ifstream bfs_in("vsl_tuple_io_test.bvl.tmp");
  TEST("Opened vsl_tuple_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, t_int_in);
  vsl_b_read(bfs_in, t_int_double_string_in);
  vsl_b_read(bfs_in, t_t_int_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink("vsl_tuple_io_test.bvl.tmp");

  TEST("std::tuple<int> out == std::tuple<int> in", t_int_out, t_int_in);
  TEST("std::tuple<int, double, std::string> out == std::tuple<int, double, std::string> in",
       t_int_double_string_out,
       t_int_double_string_in);
  TEST("std::tuple<std::tuple<int> > out == std::tuple<std::tuple<int> > in", t_t_int_out, t_t_int_in);

  vsl_print_summary(std::cout, t_int_in);
  vsl_print_summary(std::cout, t_int_double_string_in);
  vsl_print_summary(std::cout, t_t_int_in);
  std::cout << std::endl;
}

TESTMAIN(test_tuple_io);
