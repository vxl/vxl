#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_string.h>
#include <vcl_iostream.h>

#include <vsl/vsl_test.h>
#include <vsl/vsl_set_io.h>

void test_set_io()
{
  vcl_cout << "****************************" << vcl_endl;
  vcl_cout << "Testing vcl_set binary io" << vcl_endl;
  vcl_cout << "****************************" << vcl_endl;

  int n = 10;
  vcl_set<int> s_int_out;
  for (int i=0;i<n;++i) 
    s_int_out.insert(i);

  vcl_set<vcl_string> s_string_out;
  s_string_out.insert("one");
  s_string_out.insert("two");
  s_string_out.insert("three");
  s_string_out.insert("four");

  vsl_b_ofstream bfs_out("vsl_set_io_test.bvl.tmp", 
    vcl_ios_openmode(vcl_ios_out | vcl_ios_binary));
  TEST ("Created vsl_set_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, s_int_out);
  vsl_b_write(bfs_out, s_string_out);
  bfs_out.close();
  
  vcl_set<int> s_int_in;
  vcl_set<vcl_string> s_string_in;
  
  vsl_b_ifstream bfs_in("vsl_set_io_test.bvl.tmp", 
    vcl_ios_openmode(vcl_ios_in | vcl_ios_binary));
  TEST ("Opened vsl_set_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, s_int_in);
  vsl_b_read(bfs_in, s_string_in);
  bfs_in.close();

  TEST ("vcl_set<int> out == vcl_set<int> in", s_int_out == s_int_in, true);
  TEST ("vcl_set<vcl_string> out == vcl_set<vcl_string> in", 
    s_string_out == s_string_in, true);
  vsl_print_summary(vcl_cout, s_string_in);
  vcl_cout << vcl_endl;
}

TESTMAIN(test_set_io);
