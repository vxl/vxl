// This is vxl/vsl/tests/test_deque_io.cxx
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_deque_io.h>
#include <testlib/testlib_test.h>

void test_deque_io()
{
  vcl_cout << "***************************\n"
           << "Testing vcl_deque binary io\n"
           << "***************************\n";

  int n = 10;
  vcl_deque<int> d_int_out;
  for (int i=0;i<n;++i)
    d_int_out.push_back(i);


  vsl_b_ofstream bfs_out("vsl_deque_io_test.bvl.tmp");
  TEST("Created vsl_deque_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, d_int_out);
  bfs_out.close();

  vcl_deque<int> d_int_in;

  vsl_b_ifstream bfs_in("vsl_deque_io_test.bvl.tmp");
  TEST("Opened vsl_deque_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, d_int_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  // kym - double equals not defined for deque??
  //TEST("vcl_deque<int> out == vcl_deque<int> in", d_int_out, d_int_in);

  bool test_result = true;
  if (d_int_out.size() != d_int_in.size())
    test_result=false;
  else
  {
    for (unsigned int i=0; i< d_int_out.size(); i++)
    {
      if (d_int_out[i] != d_int_in[i])
        test_result=false;
    }
  }
  TEST("vcl_deque<int> out == vcl_deque<int> in", test_result, true);

  vsl_print_summary(vcl_cout, d_int_in);
  vcl_cout << vcl_endl;
}

TESTMAIN(test_deque_io);
