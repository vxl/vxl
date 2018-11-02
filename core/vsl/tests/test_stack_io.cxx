// This is core/vsl/tests/test_stack_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_stack_io.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_stack_io()
{
  std::cout << "***************************\n"
           << "Testing std::stack binary io\n"
           << "***************************\n";

  int n = 10;
  std::stack<int> s_int_out;
  for (int i=0;i<n;++i)
    s_int_out.push(i);

  vsl_b_ofstream bfs_out("vsl_stack_io_test.bvl.tmp");
  TEST("Created vsl_stack_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, s_int_out);
  bfs_out.close();

  std::stack<int> s_int_in;

  vsl_b_ifstream bfs_in("vsl_stack_io_test.bvl.tmp");
  TEST("Opened vsl_stack_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, s_int_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_stack_io_test.bvl.tmp");

  //kym - double = not defined for stack?
  //TEST("std::stack<int> out == std::stack<int> in", s_int_out, s_int_in);

  bool test_result = true;
  if (s_int_out.size() != s_int_in.size())
    test_result = false;
  else
  {
    for (unsigned int i=0; i<s_int_out.size(); i++)
    {
      if (s_int_out.top() != s_int_in.top())
         test_result = false;
       s_int_out.pop();
       s_int_in.pop();
    }
  }
  TEST("std::stack<int> out == std::stack<int> in", test_result, true);
  vsl_print_summary(std::cout, s_int_in);
  std::cout << std::endl;
}

TESTMAIN(test_stack_io);
