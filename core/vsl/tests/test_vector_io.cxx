// This is core/vsl/tests/test_vector_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vpl/vpl.h>

void test_vector_io()
{
  std::cout << "****************************\n"
           << "Testing std::vector binary io\n"
           << "****************************\n";

  int n = 10;
  std::vector<bool> v_bool_out(n);
  for (int i=0;i<n;++i) v_bool_out[i]=(i%2==0);
  std::vector<int> v_int_out(n);
  for (int i=0;i<n;++i) v_int_out[i]=i;
  std::vector<float> v_float_out(n);
  for (int i=0;i<n;++i) v_float_out[i]=0.1f*i;
  std::vector<unsigned char> v_uchar_out(n);
  for (int i=0;i<n;++i) v_uchar_out[i]=(unsigned char)(i+'A');

  vsl_b_ofstream bfs_out("vsl_vector_io_test.bvl.tmp");
  TEST("Created vsl_vector_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, v_bool_out);
  vsl_b_write(bfs_out, v_int_out);
  vsl_b_write(bfs_out, v_float_out);
  vsl_b_write(bfs_out, v_uchar_out);
  bfs_out.close();

  std::vector<bool> v_bool_in;
  std::vector<int> v_int_in;
  std::vector<float> v_float_in;
  std::vector<unsigned char> v_uchar_in;

  vsl_b_ifstream bfs_in("vsl_vector_io_test.bvl.tmp");
  TEST("Opened vsl_vector_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, v_bool_in);
  vsl_b_read(bfs_in, v_int_in);
  vsl_b_read(bfs_in, v_float_in);
  vsl_b_read(bfs_in, v_uchar_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_vector_io_test.bvl.tmp");

  TEST("std::vector<bool> out == std::vector<bool> in", v_bool_out, v_bool_in);
  TEST("std::vector<int> out == std::vector<int> in", v_int_out, v_int_in);
  TEST("std::vector<float> out == std::vector<float> in", v_float_out,v_float_in);
  TEST("std::vector<uchar> out == std::vector<uchar> in", v_uchar_out,v_uchar_in);

  vsl_print_summary(std::cout, v_bool_in);
  vsl_print_summary(std::cout, v_int_in);
  vsl_print_summary(std::cout, v_float_in);
  vsl_print_summary(std::cout, v_uchar_in);
  std::cout << std::endl;

  std::string gold_path=testlib_root_dir()+"/core/vsl/tests/golden_vector_io_test.bvl";
  vsl_b_ifstream bfs_in2(gold_path.c_str());

  // If this test fails, it could be due to a missing golden file, or one
  // which has got corrupted.
  TEST("Opened golden_test_binary_io.bvl for reading", (!bfs_in2), false);
  if (!(!bfs_in2))
  {
    vsl_b_read(bfs_in2, v_bool_in);
    vsl_b_read(bfs_in2, v_int_in);
    vsl_b_read(bfs_in2, v_float_in);
    vsl_b_read(bfs_in2, v_uchar_in);
    TEST("Read file correctly", (!bfs_in2), false);
    TEST("std::vector<bool> out == std::vector<bool> in", v_bool_out, v_bool_in);
    TEST("std::vector<int> out == std::vector<int> in", v_int_out, v_int_in);
    TEST("std::vector<float> out == std::vector<float> in", v_float_out,v_float_in);
    TEST("std::vector<uchar> out == std::vector<uchar> in", v_uchar_out,v_uchar_in);

    vsl_print_summary(std::cout, v_bool_in);
    vsl_print_summary(std::cout, v_int_in);
    vsl_print_summary(std::cout, v_float_in);
    vsl_print_summary(std::cout, v_uchar_in);
    std::cout << std::endl;
  }
}

TESTMAIN(test_vector_io);
