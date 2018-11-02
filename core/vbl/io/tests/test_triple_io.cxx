// This is core/vbl/io/tests/test_triple_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/io/vbl_io_triple.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_triple_io()
{
  std::cout << "*************************************\n"
           << "Testing vbl_triple<double,int,int> io\n"
           << "*************************************\n";

  //// test constructors, accessors
  vbl_triple<double,int,int> v_out(4.0, -1, 5), v_in;

  vsl_b_ofstream bfs_out("vbl_triple_test_io.bvl.tmp");
  TEST("Created vbl_triple_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vbl_triple_test_io.bvl.tmp");
  TEST("Opened vbl_triple_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink("vbl_triple_test_io.bvl.tmp");

  TEST("v_out == v_in", v_out, v_in);

  vsl_print_summary(std::cout, v_in);
  std::cout << std::endl;
  vsl_indent_clear_all_data ();
}

TESTMAIN(test_triple_io);
