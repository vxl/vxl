#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <testlib/testlib_test.h>
#include <vbl/io/vbl_io_array_1d.h>

void test_array_1d_io()
{
  vcl_cout << "***********************" << vcl_endl;
  vcl_cout << "Testing vbl_array_1d<float> io" << vcl_endl;
  vcl_cout << "***********************" << vcl_endl;

  //// test constructors, accessors
  const int n = 50;
  vbl_array_1d<float> v_out, v_in;

  v_out.reserve(n+10);
  for (int i=0; i<n; i++)
  {
    v_out.push_back(i*i);
  }

  vsl_b_ofstream bfs_out("vbl_array_1d_test_io.bvl.tmp");
  TEST ("Created vbl_array_1d_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vbl_array_1d_test_io.bvl.tmp");
  TEST ("Opened vbl_array_1d_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  //kym - double = not defined for vbl_array_1d
  //TEST ("v_out == v_in", v_out == v_in, true);

  bool test_result = true;
  if (v_out.size() != v_in.size())
    test_result = false;
  else if (v_out.capacity() != v_in.capacity())
    test_result = false;
  else
  {
    unsigned array_size = v_out.size();
    for (unsigned i=0; i<array_size; i++)
    {
      if (v_out[i] != v_in[i])
        test_result = false;
    }
  }
  TEST ("v_out == v_in", test_result, true);

  vsl_print_summary(vcl_cout, v_in);
  vcl_cout << vcl_endl;
}

void test_array_1d_prime()
{
  test_array_1d_io();
}


TESTLIB_DEFINE_MAIN(test_array_1d_prime);
