// This is core/vsl/tests/test_vector_io.cxx
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vpl/vpl.h>

void test_vector_io()
{
  vcl_cout << "****************************\n"
           << "Testing vcl_vector binary io\n"
           << "****************************\n";

  int n = 10;
  vcl_vector<bool> v_bool_out(n);
  for (int i=0;i<n;++i) v_bool_out[i]=(i%2==0);
  vcl_vector<int> v_int_out(n);
  for (int i=0;i<n;++i) v_int_out[i]=i;
  vcl_vector<float> v_float_out(n);
  for (int i=0;i<n;++i) v_float_out[i]=0.1f*i;

  vsl_b_ofstream bfs_out("vsl_vector_io_test.bvl.tmp");
  TEST("Created vsl_vector_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, v_bool_out);
  vsl_b_write(bfs_out, v_int_out);
  vsl_b_write(bfs_out, v_float_out);
  bfs_out.close();

  vcl_vector<bool> v_bool_in;
  vcl_vector<int> v_int_in;
  vcl_vector<float> v_float_in;

  vsl_b_ifstream bfs_in("vsl_vector_io_test.bvl.tmp");
  TEST("Opened vsl_vector_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, v_bool_in);
  vsl_b_read(bfs_in, v_int_in);
  vsl_b_read(bfs_in, v_float_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_vector_io_test.bvl.tmp");

  TEST("vcl_vector<bool> out == vcl_vector<bool> in", v_bool_out, v_bool_in);
  TEST("vcl_vector<int> out == vcl_vector<int> in", v_int_out, v_int_in);
  TEST("vcl_vector<float> out == vcl_vector<float> in", v_float_out,v_float_in);

  vsl_print_summary(vcl_cout, v_bool_in);
  vsl_print_summary(vcl_cout, v_int_in);
  vsl_print_summary(vcl_cout, v_float_in);
  vcl_cout << vcl_endl;

  vcl_string gold_path=testlib_root_dir()+"/core/vsl/tests/golden_vector_io_test.bvl";
  vsl_b_ifstream bfs_in2(gold_path.c_str());

  // If this test fails, it could be due to a missing golden file, or one
  // which has got corrupted.
  TEST("Opened golden_test_binary_io.bvl for reading", (!bfs_in2), false);
  if (!(!bfs_in2))
  {
    TEST("vcl_vector<bool> out == vcl_vector<bool> in", v_bool_out, v_bool_in);
    TEST("vcl_vector<int> out == vcl_vector<int> in", v_int_out, v_int_in);
    TEST("vcl_vector<float> out == vcl_vector<float> in", v_float_out,v_float_in);

    vsl_print_summary(vcl_cout, v_bool_in);
    vsl_print_summary(vcl_cout, v_int_in);
    vsl_print_summary(vcl_cout, v_float_in);
    vcl_cout << vcl_endl;
  }
}

TESTMAIN(test_vector_io);
