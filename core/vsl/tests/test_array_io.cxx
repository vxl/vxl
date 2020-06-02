// This is core/vsl/tests/test_array_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vsl/vsl_binary_io.h"
#include "testlib/testlib_test.h"
#include "testlib/testlib_root_dir.h"
#include "vpl/vpl.h"

#include "vsl/vsl_array_io.hxx"


#define VSL_TEST_ARRAY_N 10


void
test_array_io()
{
  std::cout << "****************************\n"
            << "Testing std::array binary io\n"
            << "****************************\n";

  std::array<bool, VSL_TEST_ARRAY_N> a_bool_out;
  for (int i = 0; i < VSL_TEST_ARRAY_N; ++i)
    a_bool_out[i] = (i % 2 == 0);
  std::array<int, VSL_TEST_ARRAY_N> a_int_out;
  for (int i = 0; i < VSL_TEST_ARRAY_N; ++i)
    a_int_out[i] = i;
  std::array<float, VSL_TEST_ARRAY_N> a_float_out;
  for (int i = 0; i < VSL_TEST_ARRAY_N; ++i)
    a_float_out[i] = 0.1f * i;
  std::array<unsigned char, VSL_TEST_ARRAY_N> a_uchar_out;
  for (int i = 0; i < VSL_TEST_ARRAY_N; ++i)
    a_uchar_out[i] = (unsigned char)(i + 'A');

  vsl_b_ofstream bfs_out("vsl_array_io_test.bvl.tmp");
  TEST("Created vsl_array_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, a_bool_out);
  vsl_b_write(bfs_out, a_int_out);
  vsl_b_write(bfs_out, a_float_out);
  vsl_b_write(bfs_out, a_uchar_out);
  bfs_out.close();

  std::array<bool, VSL_TEST_ARRAY_N> a_bool_in;
  std::array<int, VSL_TEST_ARRAY_N> a_int_in;
  std::array<float, VSL_TEST_ARRAY_N> a_float_in;
  std::array<unsigned char, VSL_TEST_ARRAY_N> a_uchar_in;

  vsl_b_ifstream bfs_in("vsl_array_io_test.bvl.tmp");
  TEST("Opened vsl_array_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, a_bool_in);
  vsl_b_read(bfs_in, a_int_in);
  vsl_b_read(bfs_in, a_float_in);
  vsl_b_read(bfs_in, a_uchar_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink("vsl_array_io_test.bvl.tmp");

  std::ostringstream ss;
  ss << "std::array<bool, " << VSL_TEST_ARRAY_N << "> out == std::array<bool, " << VSL_TEST_ARRAY_N << "> in";
  auto bool_str = ss.str();

  ss.str("");
  ss << "std::array<int, " << VSL_TEST_ARRAY_N << "> out == std::array<int, " << VSL_TEST_ARRAY_N << "> in";
  auto int_str = ss.str();

  ss.str("");
  ss << "std::array<float, " << VSL_TEST_ARRAY_N << "> out == std::array<float, " << VSL_TEST_ARRAY_N << "> in";
  auto float_str = ss.str();

  ss.str("");
  ss << "std::array<uchar, " << VSL_TEST_ARRAY_N << "> out == std::array<uchar, " << VSL_TEST_ARRAY_N << "> in";
  auto uchar_str = ss.str();

  TEST(bool_str.c_str(), a_bool_out, a_bool_in);
  TEST(int_str.c_str(), a_int_out, a_int_in);
  TEST(float_str.c_str(), a_float_out, a_float_in);
  TEST(uchar_str.c_str(), a_uchar_out, a_uchar_in);

  vsl_print_summary(std::cout, a_bool_in);
  vsl_print_summary(std::cout, a_int_in);
  vsl_print_summary(std::cout, a_float_in);
  vsl_print_summary(std::cout, a_uchar_in);
  std::cout << std::endl;
}

TESTMAIN(test_array_io);
