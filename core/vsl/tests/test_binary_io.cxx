// This is core/vsl/tests/test_binary_io.cxx
#include <iostream>
#include <cstring>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_quick_file.h>
#include <testlib/testlib_root_dir.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_binary_io()
{
  std::cout << "*********************\n"
           << "Testing vsl binary io\n"
           << "*********************\n";

  bool b_out = true;
  char c_out = 'y';
  signed char sc_out = 'k';
  unsigned char uc_out = 'q';
  int i_out = 3;
  unsigned int ui_out = 37;
  short short_out = 56;
  signed ushort_out = -32768;
  long long_out = -12345678;
  unsigned long ulong_out = 87654321;
  float f_out = 1.7f;
  double d_out = 3.4;
  std::string string_out = "Hello World!";
  const char* c_string_out = "A C string";
  std::size_t size_t_out = 1023;
  std::ptrdiff_t ptrdiff_t_out = 23;

  vsl_b_ofstream bfs_out("vsl_binary_io_test.bvl.tmp");
  TEST("Created vsl_binary_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, b_out);
  vsl_b_write(bfs_out, c_out);
  vsl_b_write(bfs_out, sc_out);
  vsl_b_write(bfs_out, uc_out);
  vsl_b_write(bfs_out, i_out);
  vsl_b_write(bfs_out, ui_out);
  vsl_b_write(bfs_out, short_out);
  vsl_b_write(bfs_out, ushort_out);
  vsl_b_write(bfs_out, long_out);
  vsl_b_write(bfs_out, ulong_out);
  vsl_b_write(bfs_out, f_out);
  vsl_b_write(bfs_out, d_out);
  vsl_b_write(bfs_out, string_out);
  vsl_b_write(bfs_out, c_string_out);
  vsl_b_write(bfs_out, size_t_out);
  vsl_b_write(bfs_out, ptrdiff_t_out);
  bfs_out.close();

  // Initialise each built in type to something different from
  // what is about to be loaded
  bool b_in = false;
  char c_in = '?';
  signed char sc_in = '?';
  unsigned char uc_in = '?';
  int i_in = 99;
  unsigned int ui_in =99;
  short short_in = 99;
  signed ushort_in = 99;
  long long_in =99;
  unsigned long ulong_in = 99;
  float f_in = 99.99f;
  double d_in = 99.9;
  std::string string_in;
  char c_string_in[80];
  std::size_t size_t_in = 99;
  std::ptrdiff_t ptrdiff_t_in = 99;

  // Test the internal consistency - can it load what it just saved?

  vsl_b_ifstream bfs_in("vsl_binary_io_test.bvl.tmp");
  TEST("Opened vsl_binary_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, b_in);

  TEST("bool out == bool in", b_out, b_in);
  vsl_print_summary(std::cout, b_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, c_in);
  TEST("char out == char in", c_out, c_in);
  vsl_print_summary(std::cout, c_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, sc_in);
  TEST("signed char out == signed char in", sc_out, sc_in);
  vsl_print_summary(std::cout, sc_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, uc_in);
  TEST("unsigned char out == unsigned char in", uc_out, uc_in);
  vsl_print_summary(std::cout, uc_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, i_in);
  TEST("int out == int in", i_out, i_in);
  vsl_print_summary(std::cout, i_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, ui_in);
  TEST("unsigned int out == unsigned int in", ui_out, ui_in);
  vsl_print_summary(std::cout, ui_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, short_in);
  TEST("short int out == short int in", short_out, short_in);
  vsl_print_summary(std::cout, short_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, ushort_in);
  TEST("unsigned short int out == unsigned short int in", ushort_out, ushort_in);
  vsl_print_summary(std::cout, short_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, long_in);
  TEST("long out == long in", long_out, long_in);
  vsl_print_summary(std::cout, long_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, ulong_in);
  TEST("unsigned long out == unsigned long in", ulong_out, ulong_in);
  vsl_print_summary(std::cout, ulong_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, f_in);
  TEST("float out == float in", f_out, f_in);
  vsl_print_summary(std::cout, f_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, d_in);
  TEST("double out == double in", d_out, d_in);
  vsl_print_summary(std::cout, d_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, string_in);
  TEST("string out == string in", string_out, string_in);
  vsl_print_summary(std::cout, string_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, c_string_in);
  TEST("C string out == C string in", std::strcmp(c_string_out,c_string_in), 0);
  vsl_print_summary(std::cout, c_string_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, size_t_in);
  TEST("std::size_t out == std::size_t in", size_t_out, size_t_in);
  vsl_print_summary(std::cout, size_t_out);
  std::cout << std::endl;

  vsl_b_read(bfs_in, ptrdiff_t_in);
  TEST("std::ptrdiff_t out == std::ptrdiff_t in", ptrdiff_t_out, ptrdiff_t_in);
  vsl_print_summary(std::cout, ptrdiff_t_out);
  std::cout << std::endl;

  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_binary_io_test.bvl.tmp");

  std::cout << "****************************\n"
           << "Testing Golden vsl binary io\n"
           << "****************************\n";

  // Test global consistancy - can it load the golden binary output file
  // This should be identical what has just been saved, which should
  // be loadable by all platforms
  // If one adds another standard output method, the golden file
  // must also be fixed (eg by copying a known working output file)

  // Initialise each built in type to something different from
  // what is about to be loaded
  bool b_in2 = false;
  char c_in2 = '?';
  signed char sc_in2 = '?';
  unsigned char uc_in2 = '?';
  int i_in2 = 99;
  unsigned int ui_in2 =99;
  short short_in2 = 99;
  short ushort_in2 = 99;
  long long_in2 =99;
  unsigned long ulong_in2 = 99;
  float f_in2 = 99.99f;
  double d_in2 = 99.9;
  std::string string_in2;
  char c_string_in2[80];
  std::size_t size_t_in2 = 99;
  std::ptrdiff_t ptrdiff_t_in2 = 99;

  std::string gold_path=testlib_root_dir()+"/core/vsl/tests/golden_test_binary_io.bvl";
  vsl_b_ifstream bfs_in2(gold_path.c_str());

  // If this test fails, it could be due to a missing golden file, or one
  // which has got corrupted.
  TEST("Opened golden_test_binary_io.bvl for reading", (!bfs_in2), false);
  if (!(!bfs_in2))
  {
    vsl_b_read(bfs_in2, b_in2);
    vsl_b_read(bfs_in2, c_in2);
    vsl_b_read(bfs_in2, sc_in2);
    vsl_b_read(bfs_in2, uc_in2);
    vsl_b_read(bfs_in2, i_in2);
    vsl_b_read(bfs_in2, ui_in2);
    vsl_b_read(bfs_in2, short_in2);
    vsl_b_read(bfs_in2, ushort_in2);
    vsl_b_read(bfs_in2, long_in2);
    vsl_b_read(bfs_in2, ulong_in2);
    vsl_b_read(bfs_in2, f_in2);
    vsl_b_read(bfs_in2, d_in2);
    vsl_b_read(bfs_in2, string_in2);
    vsl_b_read(bfs_in2, c_string_in2);
    vsl_b_read(bfs_in2, size_t_in2);
    vsl_b_read(bfs_in2, ptrdiff_t_in2);
  }
  TEST("Finished reading file successfully", (!bfs_in2), false);
  bfs_in2.close();

  TEST("Golden bool out == bool in", b_out, b_in2);
  TEST("Golden char out == char in", c_out, c_in2);
  TEST("Golden signed char out == signed char in", sc_out, sc_in2);
  TEST("Golden unsigned char out == unsigned char in", uc_out, uc_in2);
  TEST("Golden int out == int in", i_out, i_in2);
  TEST("Golden unsigned int out == unsigned int in", ui_out, ui_in2);
  TEST("Golden short int out == short int in", short_out, short_in2);
  TEST("Golden unsigned short int out == unsigned short int in", short_out, short_in2);
  TEST("Golden long out == long in", long_out, long_in2);
  TEST("Golden unsigned long out == unsigned long in", ulong_out, ulong_in2);
  TEST("Golden float out == float in", f_out, f_in2);
  TEST("Golden double out == double in", d_out, d_in2);
  TEST("Golden string out == string in", string_out, string_in2);
  TEST("Golden C string out == C string in", std::string(c_string_out), std::string(c_string_in2));
  TEST("Golden std::size_t out == std::size_t in", size_t_out, size_t_in2);
  TEST("Golden std::ptrdiff_t out == std::ptrdiff_t in", ptrdiff_t_out, ptrdiff_t_in2);


  std::cout << "****************************\n"
           << " Testing magic number check\n"
           << "****************************\n";

  std::ifstream gold_if(gold_path.c_str());
  TEST("vsl_b_stream_test on golden data", vsl_b_istream_test(gold_if), true);
  {
    std::ofstream f("vsl_binary_io_test.txt");
    f << "Some random text.\n";
  }
  std::ifstream none_if("Some_non_existant_file");
  TEST("vsl_b_stream_test on missing file fails", vsl_b_istream_test(none_if), false);
  {
    std::ofstream f("Some_empty_file");
  }
  TEST("vsl_b_stream_test on empty file fails", vsl_b_istream_test(none_if), false);
  std::ifstream text_if("vsl_binary_io_test.txt");
  TEST("vsl_b_stream_test on text file fails", vsl_b_istream_test(text_if), false);

}

TESTMAIN(test_binary_io);
