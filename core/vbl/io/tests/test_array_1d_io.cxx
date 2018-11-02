// This is core/vbl/io/tests/test_array_1d_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/io/vbl_io_array_1d.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_array_1d_io()
{
  std::cout << "******************************\n"
           << "Testing vbl_array_1d<float> io\n"
           << "******************************\n";

  //// test constructors, accessors
  constexpr int n = 50;
  vbl_array_1d<float> v_out, v_in;

  v_out.reserve(n+10);
  for (int i=0; i<n; i++)
  {
    v_out.push_back(0.1f*i*i);
  }

  vsl_b_ofstream bfs_out("vbl_array_1d_test_io.bvl.tmp");
  TEST("Created vbl_array_1d_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vbl_array_1d_test_io.bvl.tmp");
  TEST("Opened vbl_array_1d_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vbl_array_1d_test_io.bvl.tmp");

  //kym - double = not defined for vbl_array_1d
  //TEST("v_out == v_in", v_out, v_in);

  bool test_result = true;
  if (v_out.size() != v_in.size())
    test_result = false;
  else if (v_out.capacity() != v_in.capacity())
    test_result = false;
  else
  {
    std::size_t array_size = v_out.size();
    for (std::size_t i=0; i<array_size; i++)
    {
      if (v_out[i] != v_in[i])
        test_result = false;
    }
  }
  TEST("v_out == v_in", test_result, true);

  vsl_print_summary(std::cout, v_in);
  std::cout << std::endl;
  vsl_indent_clear_all_data();
}

TESTMAIN(test_array_1d_io);
