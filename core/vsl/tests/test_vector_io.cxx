// This is vxl/vsl/tests/test_vector_io.cxx
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_vector_io()
{
  vcl_cout << "****************************\n"
           << "Testing vcl_vector binary io\n"
           << "****************************\n";

  int n = 10;
  vcl_vector<int> v_int_out(n);
  for (int i=0;i<n;++i) v_int_out[i]=i;
  vcl_vector<float> v_float_out(n);
  for (int i=0;i<n;++i) v_float_out[i]=0.1f*i;

  vsl_b_ofstream bfs_out("vsl_vector_io_test.bvl.tmp");
  TEST("Created vsl_vector_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, v_int_out);
  vsl_b_write(bfs_out, v_float_out);
  bfs_out.close();

  vcl_vector<int> v_int_in;
  vcl_vector<float> v_float_in;

  vsl_b_ifstream bfs_in("vsl_vector_io_test.bvl.tmp");
  TEST("Opened vsl_vector_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, v_int_in);
  vsl_b_read(bfs_in, v_float_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_vector_io_test.bvl.tmp");

  TEST("vcl_vector<int> out == vcl_vector<int> in", v_int_out, v_int_in);
  TEST("vcl_vector<float> out == vcl_vector<float> in", v_float_out,v_float_in);

  vsl_print_summary(vcl_cout, v_int_in);
  vsl_print_summary(vcl_cout, v_float_in);
  vcl_cout << vcl_endl;
}

TESTMAIN(test_vector_io);
