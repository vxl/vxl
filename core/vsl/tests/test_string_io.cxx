#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vsl/vsl_test.h>
#include <vsl/vsl_string_io.h>

void test_string_io()
{
  vcl_cout << "****************************" << vcl_endl;
  vcl_cout << "Testing vcl_string binary io" << vcl_endl;
  vcl_cout << "****************************" << vcl_endl;

  vcl_string outstring("This is a test");

  vsl_b_ofstream bfs_out("vsl_string_io_test.bvl.tmp",
    vcl_ios_out | vcl_ios_binary);
  TEST ("Created vsl_string_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, outstring);
  bfs_out.close();

  vcl_string instring;
  
  vsl_b_ifstream bfs_in("vsl_string_io_test.bvl.tmp",
    vcl_ios_in | vcl_ios_binary);
  TEST ("Opened vsl_string_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, instring);
  bfs_in.close();

  TEST ("vcl_string<int> out == vcl_string<int> in",
    outstring == instring, true);
}

TESTMAIN(test_string_io);
