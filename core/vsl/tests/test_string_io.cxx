#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <testlib/testlib_test.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_string_io.h>

void test_string_io()
{
  vcl_cout << "****************************" << vcl_endl;
  vcl_cout << "Testing vcl_string binary io" << vcl_endl;
  vcl_cout << "****************************" << vcl_endl;

  vcl_string outstring("This is a test");

  vsl_b_ofstream bfs_out("vsl_string_io_test.bvl.tmp");
  TEST ("Created vsl_string_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, outstring);
  bfs_out.close();

  vcl_string instring;

  vsl_b_ifstream bfs_in("vsl_string_io_test.bvl.tmp");
  TEST ("Opened vsl_string_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, instring);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("vcl_string<int> out == vcl_string<int> in",
    outstring == instring, true);
}

TESTLIB_DEFINE_MAIN(test_string_io);
