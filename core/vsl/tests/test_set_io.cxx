// This is vxl/vsl/tests/test_set_io.cxx
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_set_io.h>
#include <testlib/testlib_test.h>

void test_set_io()
{
  vcl_cout << "*************************\n"
           << "Testing vcl_set binary io\n"
           << "*************************\n";

  int n = 10;
  vcl_set<int> s_int_out;
  for (int i=0;i<n;++i)
    s_int_out.insert(i);

  vcl_set<vcl_string> s_string_out;
  s_string_out.insert("one");
  s_string_out.insert("two");
  s_string_out.insert("three");
  s_string_out.insert("four");

  vsl_b_ofstream bfs_out("vsl_set_io_test.bvl.tmp");
  TEST ("Created vsl_set_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, s_int_out);
  vsl_b_write(bfs_out, s_string_out);
  bfs_out.close();

  vcl_set<int> s_int_in;
  vcl_set<vcl_string> s_string_in;

  vsl_b_ifstream bfs_in("vsl_set_io_test.bvl.tmp");
  TEST ("Opened vsl_set_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, s_int_in);
  vsl_b_read(bfs_in, s_string_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("vcl_set<int> out == vcl_set<int> in", s_int_out == s_int_in, true);
  TEST ("vcl_set<vcl_string> out == vcl_set<vcl_string> in",
    s_string_out == s_string_in, true);
  vsl_print_summary(vcl_cout, s_string_in);
  vcl_cout << vcl_endl;
}

TESTLIB_DEFINE_MAIN(test_set_io);
