#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cstring.h>
#include <testlib/testlib_root_dir.h>

#include <testlib/testlib_test.h>
#include <vsl/vsl_binary_io.h>


void test_binary_io()
{
  vcl_cout << "*********************" << vcl_endl;
  vcl_cout << "Testing vsl binary io" << vcl_endl;
  vcl_cout << "*********************" << vcl_endl;

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
  vcl_string string_out = "Hello World!";
  const char* c_string_out = "A C string";

  vsl_b_ofstream bfs_out("vsl_binary_io_test.bvl.tmp");
  TEST ("Created vsl_binary_io_test.bvl.tmp for writing", (!bfs_out), false);
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
  vcl_string string_in;
  char c_string_in[80];

  // Test the internal consistency - can it load what it just saved?

  vsl_b_ifstream bfs_in("vsl_binary_io_test.bvl.tmp");
  TEST ("Opened vsl_binary_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, b_in);

  TEST ("bool out == bool in", b_out == b_in, true);
  vsl_print_summary(vcl_cout, b_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, c_in);
  TEST ("char out == char in", c_out == c_in, true);
  vsl_print_summary(vcl_cout, c_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, sc_in);
  TEST ("signed char out == signed char in", sc_out == sc_in, true);
  vsl_print_summary(vcl_cout, sc_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, uc_in);
  TEST ("unsigned char out == unsigned char in", uc_out == uc_in, true);
  vsl_print_summary(vcl_cout, uc_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, i_in);
  TEST ("int out == int in", i_out == i_in, true);
  vsl_print_summary(vcl_cout, i_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, ui_in);
  TEST ("unsigned int out == unsigned int in", ui_out == ui_in, true);
  vsl_print_summary(vcl_cout, ui_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, short_in);
  TEST ("short int out == short int in", short_out == short_in, true);
  vsl_print_summary(vcl_cout, short_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, ushort_in);
  TEST ("unsigned short int out == unsigned short int in",
    ushort_out == ushort_in, true);
  vsl_print_summary(vcl_cout, short_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, long_in);
  TEST ("long out == long in", long_out == long_in, true);
  vsl_print_summary(vcl_cout, long_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, ulong_in);
  TEST ("unsigned long out == unsigned long in",
    ulong_out == ulong_in, true);
  vsl_print_summary(vcl_cout, ulong_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, f_in);
  TEST ("float out == float in", f_out == f_in, true);
  vsl_print_summary(vcl_cout, f_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, d_in);
  TEST ("double out == double in", d_out == d_in, true);
  vsl_print_summary(vcl_cout, d_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, string_in);
  TEST ("string out == string in", string_out == string_in, true);
  vsl_print_summary(vcl_cout, string_out);
  vcl_cout << vcl_endl;

  vsl_b_read(bfs_in, c_string_in);
  TEST ("C string out == C string in", vcl_strcmp(c_string_out,c_string_in), 0);
  vsl_print_summary(vcl_cout, c_string_out);
  vcl_cout << vcl_endl;

  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();


  vcl_cout << "*****************************" << vcl_endl;
  vcl_cout << "Testing Golden vsl binary io" << vcl_endl;
  vcl_cout << "****************************" << vcl_endl;

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
  vcl_string string_in2;
  char c_string_in2[80];

  vcl_string gold_path=testlib_root_dir()+"/vxl/vsl/tests/golden_test_binary_io.bvl";
  vsl_b_ifstream bfs_in2(gold_path.c_str());

  // If this test fails, it could be due to a missing golden file, or one
  // which has got corrupted.
  TEST ("Opened golden_test_binary_io.bvl for reading",
    (!bfs_in2), false);
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
  }
  TEST ("Finished reading file successfully", (!bfs_in2), false);
  bfs_in2.close();

  TEST ("Golden bool out == bool in", b_out == b_in2, true);
  TEST ("Golden char out == char in", c_out == c_in2, true);
  TEST ("Golden signed char out == signed char in", sc_out == sc_in2, true);
  TEST ("Golden unsigned char out == unsigned char in", uc_out == uc_in2, true);
  TEST ("Golden int out == int in", i_out == i_in2, true);
  TEST ("Golden unsigned int out == unsigned int in", ui_out == ui_in2, true);
  TEST ("Golden short int out == short int in", short_out == short_in2, true);
  TEST ("Golden unsigned short int out == unsigned short int in",
    short_out == short_in2, true);
  TEST ("Golden long out == long in", long_out == long_in2, true);
  TEST ("Golden unsigned long out == unsigned long in",
    ulong_out == ulong_in2, true);
  TEST ("Golden float out == float in", f_out == f_in2, true);
  TEST ("Golden double out == double in", d_out == d_in2, true);
  TEST ("Golden string out == string in", string_out == string_in2, true);
  TEST ("Golden C string out == C string in",
    vcl_string(c_string_out) == vcl_string(c_string_in2), true);
}

TESTLIB_DEFINE_MAIN(test_binary_io);
