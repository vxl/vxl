#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_string.h>

#include <testlib/testlib_test.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_map_io.h>

void test_map_io()
{
  vcl_cout << "****************************" << vcl_endl;
  vcl_cout << "Testing vcl_map binary io" << vcl_endl;
  vcl_cout << "****************************" << vcl_endl;

  int n = 10;
  vcl_map<int, int, vcl_less<int> > m_int_int_out;
  for (int i=0;i<n;++i)
    m_int_int_out[i] = i*i+1;

  vcl_map<int, vcl_string, vcl_less<int> > m_int_string_out;
  m_int_string_out[1] = vcl_string("one");
  m_int_string_out[2] = vcl_string("two");
  m_int_string_out[3] = vcl_string("three");
  m_int_string_out[4] = vcl_string("four");

  vcl_map<vcl_string, int, vcl_less<vcl_string> > m_string_int_out;
  m_string_int_out[vcl_string("one")] = 1;
  m_string_int_out[vcl_string("two")] = 2;
  m_string_int_out[vcl_string("three")] = 3;
  m_string_int_out[vcl_string("four")] = 4;
  m_string_int_out[vcl_string("five")] = 5;
  m_string_int_out[vcl_string("six")] = 6;

  vsl_b_ofstream bfs_out("vsl_map_io_test.bvl.tmp");
  TEST ("Created vsl_map_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, m_int_int_out);
  vsl_b_write(bfs_out, m_int_string_out);
  vsl_b_write(bfs_out, m_string_int_out);
  bfs_out.close();

  vcl_map<int,int, vcl_less<int> > m_int_int_in;
  vcl_map<int,vcl_string, vcl_less<int> > m_int_string_in;
  vcl_map<vcl_string,int, vcl_less<vcl_string> > m_string_int_in;

  vsl_b_ifstream bfs_in("vsl_map_io_test.bvl.tmp");
  TEST ("Opened vsl_map_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, m_int_int_in);
  vsl_b_read(bfs_in, m_int_string_in);
  vsl_b_read(bfs_in, m_string_int_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("vcl_map<int,int> out == vcl_map<int,int> in",
    m_int_int_out == m_int_int_in, true);
  TEST ("vcl_map<int,vcl_string> out == vcl_map<int,vcl_string> in",
    m_int_string_out == m_int_string_in, true);
  TEST ("vcl_map<vcl_string,int> out == vcl_map<vcl_string,int> in",
    m_string_int_out == m_string_int_in, true);

  vsl_print_summary(vcl_cout, m_int_string_in);
  vcl_cout << vcl_endl;
  vsl_print_summary(vcl_cout, m_string_int_in);
  vcl_cout << vcl_endl;
}

TESTLIB_DEFINE_MAIN(test_map_io);
