// This is core/vsl/tests/test_block_rle_io.cxx
#include <iostream>
#include <vector>
#include <algorithm>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_block_binary_rle.h>
#include <vsl/vsl_block_binary.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_deque_io.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vpl/vpl.h>

void test_block_rle_io()
{
  std::cout << "*********************************\n"
           << " Testing vsl_block_binary_rle io\n"
           << "*********************************\n";

  constexpr int n = 100;
  bool v_bool_out[n];
  for (int i=0;i<n;++i) v_bool_out[i]=i<20;
  std::vector<int> v_int_out(n);
  for (int i=0;i<n;++i) v_int_out[i]=n/20;
  std::vector<float> v_float_out(n);
  for (int i=0;i<n;++i) v_float_out[i]=0.1f*(i/20);
  std::vector<unsigned char> v_uchar_out(n);
  for (int i=0;i<n;++i) v_uchar_out[i]=(unsigned char)(i/20+'A');
  std::vector<vxl_int_64> v_int64_out(n);
  for (int i=0;i<n;++i) v_int64_out[i]=i<20;

  vsl_b_ofstream bfs_out_uncompressed("vsl_block_rle_uncomp_test.bvl.tmp");
  vsl_b_ofstream bfs_out_compressed("vsl_block_rle_comp_test.bvl.tmp");
  TEST("Created vsl_block_rle_uncomp_test.bvl.tmp for writing", (!bfs_out_uncompressed), false);
  TEST("Created vsl_block_rle_comp_test.bvl.tmp for writing", (!bfs_out_compressed), false);
  vsl_block_binary_write(bfs_out_uncompressed, v_bool_out, n);
  vsl_block_binary_write(bfs_out_uncompressed, &v_int_out.front(), n);
  vsl_block_binary_write(bfs_out_uncompressed, &v_float_out.front(), n);
  vsl_block_binary_write(bfs_out_uncompressed, &v_uchar_out.front(), n);
  vsl_block_binary_write(bfs_out_uncompressed, &v_int64_out.front(), n);
  std::streamoff uncompressed_length = bfs_out_uncompressed.os().tellp();
  bfs_out_uncompressed.close();

  vsl_block_binary_rle_write(bfs_out_compressed, v_bool_out, n);
  vsl_block_binary_rle_write(bfs_out_compressed, &v_int_out.front(), n);
  vsl_block_binary_rle_write(bfs_out_compressed, &v_float_out.front(), n);
  vsl_block_binary_rle_write(bfs_out_compressed, &v_uchar_out.front(), n);
  vsl_block_binary_rle_write(bfs_out_compressed, &v_int64_out.front(), n);
  std::streamoff compressed_length = bfs_out_compressed.os().tellp();
  bfs_out_compressed.close();
  double ratio = ((double) compressed_length) / uncompressed_length;

  std::cout << "Uncompressed size: " << uncompressed_length << " Compressed size: " << compressed_length
           << " Ratio: " << ratio << std::endl;
  TEST("Significant compression achieved", uncompressed_length > 100 && ratio < 0.3, true);

  {
    bool v_bool_in[n];
    std::vector<int> v_int_in(n);
    std::vector<float> v_float_in(n);
    std::vector<unsigned char> v_uchar_in(n);
    std::vector<vxl_int_64> v_int64_in(n);

    std::cout << "\n\n****** Testing vsl_block_binary_write/read round-trip\n\n";
    vsl_b_ifstream bfs_in_uncompressed("vsl_block_rle_uncomp_test.bvl.tmp");
    TEST("Opened vsl_block_rle_uncomp_test.bvl.tmp for reading", (!bfs_in_uncompressed), false);
    vsl_block_binary_read(bfs_in_uncompressed, v_bool_in, n);
    vsl_block_binary_read(bfs_in_uncompressed, &v_int_in.front(), n);
    vsl_block_binary_read(bfs_in_uncompressed, &v_float_in.front(), n);
    vsl_block_binary_read(bfs_in_uncompressed, &v_uchar_in.front(), n);
    vsl_block_binary_read(bfs_in_uncompressed, &v_int64_in.front(), n);
    TEST("Finished reading file successfully", (!bfs_in_uncompressed), false);
    bfs_in_uncompressed.close();


    TEST("std::vector<bool> out == std::vector<bool> in",
         std::equal(v_bool_out, v_bool_out+n, v_bool_in), true);
    TEST("std::vector<int> out == std::vector<int> in", v_int_out, v_int_in);
    TEST("std::vector<float> out == std::vector<float> in", v_float_out,v_float_in);
    TEST("std::vector<uchar> out == std::vector<uchar> in", v_uchar_out,v_uchar_in);
    TEST("std::vector<int64> out == std::vector<int64> in", v_int64_out,v_int64_in);

    vsl_print_summary(std::cout, v_int_in);
    vsl_print_summary(std::cout, v_float_in);
    vsl_print_summary(std::cout, v_uchar_in);
    vsl_print_summary(std::cout, v_int64_in);
    std::cout << std::endl;
  }
  {
    bool v_bool_in[n];
    std::vector<int> v_int_in(n);
    std::vector<float> v_float_in(n);
    std::vector<unsigned char> v_uchar_in(n);
    std::vector<vxl_int_64> v_int64_in(n);


    std::cout << "\n\n****** Testing vsl_block_binary_rle_write/read round-trip\n\n";
    vsl_b_ifstream bfs_in_compressed("vsl_block_rle_comp_test.bvl.tmp");
    TEST("Opened vsl_block_rle_comp_test.bvl.tmp for reading", (!bfs_in_compressed), false);
    vsl_block_binary_rle_read(bfs_in_compressed, v_bool_in, n);
    vsl_block_binary_rle_read(bfs_in_compressed, &v_int_in.front(), n);
    vsl_block_binary_rle_read(bfs_in_compressed, &v_float_in.front(), n);
    vsl_block_binary_rle_read(bfs_in_compressed, &v_uchar_in.front(), n);
    vsl_block_binary_rle_read(bfs_in_compressed, &v_int64_in.front(), n);
    TEST("Finished reading file successfully", (!bfs_in_compressed), false);
    bfs_in_compressed.close();

    TEST("std::vector<bool> out == std::vector<bool> in",
         std::equal(v_bool_out, v_bool_out+n, v_bool_in), true);
    TEST("std::vector<int> out == std::vector<int> in", v_int_out, v_int_in);
    TEST("std::vector<float> out == std::vector<float> in", v_float_out,v_float_in);
    TEST("std::vector<uchar> out == std::vector<uchar> in", v_uchar_out,v_uchar_in);
    TEST("std::vector<int64> out == std::vector<int64> in", v_int64_out,v_int64_in);

    vsl_print_summary(std::cout, v_int_in);
    vsl_print_summary(std::cout, v_float_in);
    vsl_print_summary(std::cout, v_uchar_in);
    vsl_print_summary(std::cout, v_int64_in);
    std::cout << std::endl;
  }
  {
    std::cout << "\n\n****** Testing golden vsl_block_binary_rle_write/read\n\n";
    bool v_bool_in[n];
    std::vector<int> v_int_in(n);
    std::vector<float> v_float_in(n);
    std::vector<unsigned char> v_uchar_in(n);
    std::vector<vxl_int_64> v_int64_in(n);

    std::string gold_path=testlib_root_dir()+"/core/vsl/tests/golden_block_rle_comp_test.bvl";
    vsl_b_ifstream bfs_in2(gold_path.c_str());

    // If this test fails, it could be due to a missing golden file, or one
    // which has got corrupted.
    TEST("Opened golden_vsl_block_rle_comp_test.bvl for reading", (!bfs_in2), false);
    if (!(!bfs_in2))
    {
      vsl_block_binary_rle_read(bfs_in2, v_bool_in, n);
      vsl_block_binary_rle_read(bfs_in2, &v_int_in.front(), n);
      vsl_block_binary_rle_read(bfs_in2, &v_float_in.front(), n);
      vsl_block_binary_rle_read(bfs_in2, &v_uchar_in.front(), n);
      vsl_block_binary_rle_read(bfs_in2, &v_int64_in.front(), n);
      TEST("Read file correctly", (!bfs_in2), false);
      TEST("std::vector<bool> out == std::vector<bool> in",
           std::equal(v_bool_out, v_bool_out+n, v_bool_in), true);
      TEST("std::vector<int> out == std::vector<int> in", v_int_out, v_int_in);
      TEST("std::vector<float> out == std::vector<float> in", v_float_out, v_float_in);
      TEST("std::vector<uchar> out == std::vector<uchar> in", v_uchar_out, v_uchar_in);
      TEST("std::vector<int64> out == std::vector<int64> in", v_int64_out, v_int64_in);

  //  vsl_print_summary(std::cout, v_bool_in);
      vsl_print_summary(std::cout, v_int_in);
      vsl_print_summary(std::cout, v_float_in);
      vsl_print_summary(std::cout, v_uchar_in);
      vsl_print_summary(std::cout, v_int64_in);
      std::cout << std::endl;
    }
  }
}

TESTMAIN(test_block_rle_io);
