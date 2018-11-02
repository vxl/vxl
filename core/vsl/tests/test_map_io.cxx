// This is core/vsl/tests/test_map_io.cxx
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_map_io.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_map_io()
{
  std::cout << "*************************\n"
           << "Testing std::map binary io\n"
           << "*************************\n";

  int n = 10;
  std::map<int, int, std::less<int> > m_int_int_out;
  for (int i=0;i<n;++i)
    m_int_int_out[i] = i*i+1;

  std::map<int, std::string, std::less<int> > m_int_string_out;
  m_int_string_out[1] = std::string("one");
  m_int_string_out[2] = std::string("two");
  m_int_string_out[3] = std::string("three");
  m_int_string_out[4] = std::string("four");

  std::map<std::string, int, std::less<std::string> > m_string_int_out;
  m_string_int_out[std::string("one")] = 1;
  m_string_int_out[std::string("two")] = 2;
  m_string_int_out[std::string("three")] = 3;
  m_string_int_out[std::string("four")] = 4;
  m_string_int_out[std::string("five")] = 5;
  m_string_int_out[std::string("six")] = 6;

  vsl_b_ofstream bfs_out("vsl_map_io_test.bvl.tmp");
  TEST("Created vsl_map_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, m_int_int_out);
  vsl_b_write(bfs_out, m_int_string_out);
  vsl_b_write(bfs_out, m_string_int_out);
  bfs_out.close();

  std::map<int,int, std::less<int> > m_int_int_in;
  std::map<int,std::string, std::less<int> > m_int_string_in;
  std::map<std::string,int, std::less<std::string> > m_string_int_in;

  vsl_b_ifstream bfs_in("vsl_map_io_test.bvl.tmp");
  TEST("Opened vsl_map_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, m_int_int_in);
  vsl_b_read(bfs_in, m_int_string_in);
  vsl_b_read(bfs_in, m_string_int_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_map_io_test.bvl.tmp");

  TEST("std::map<int,int> out == in", m_int_int_out, m_int_int_in);
  TEST("std::map<int,std::string> out == in", m_int_string_out, m_int_string_in);
  TEST("std::map<std::string,int> out == in", m_string_int_out, m_string_int_in);

  vsl_print_summary(std::cout, m_int_string_in);
  std::cout << std::endl;
  vsl_print_summary(std::cout, m_string_int_in);
  std::cout << std::endl;
}

TESTMAIN(test_map_io);
