// This is mul/mbl/tests/test_test.cxx
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdlib.h>
#include <vcl_cstring.h> // for std::strncpy()
#include <vcl_algorithm.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_test.h>
#include <testlib/testlib_test.h>

void test_test()
{
  vcl_cout << "***********************************\n"
           << " Testing mbl_test_save_measurement\n"
           << "***********************************\n";

  vpl_unlink((vul_file::get_cwd()+"/mul/mbl/mbl_test_save_measurement.txt").c_str());
  vpl_rmdir((vul_file::get_cwd()+"/mul/mbl").c_str());
  vpl_rmdir((vul_file::get_cwd()+"/mul").c_str());
  vpl_rmdir((vul_file::get_cwd()+"/mul").c_str());

  vcl_string cwd = vul_file::get_cwd();
  vcl_replace(cwd.begin(), cwd.end(), '\\', '/' ); // avoid backslash control char interpretation.

  // Avoid problems with borland's putenv.
  char buf[1024];
  vcl_strncpy(buf, (vcl_string("MBL_TEST_SAVE_MEASUREMENT_ROOT=")+cwd).c_str(), 1023);
  vpl_putenv(buf);

  char * envar = vcl_getenv("MBL_TEST_SAVE_MEASUREMENT_ROOT");
  vcl_string envar2(envar?envar:"");
  TEST ("putenv works", envar2, cwd);
  vcl_cout << "Environment variable should be \"" << cwd << "\".\n"
           << "Is \"" << envar << "\".\n\n" << vcl_endl;

  mbl_test_save_measurement("mul/mbl/mbl_test_save_measurement", 5.0);
  mbl_test_save_measurement("mul/mbl/mbl_test_save_measurement", 10.0);

  vcl_string fn = vul_file::get_cwd()+ "/mul/mbl/mbl_test_save_measurement.txt";
  vcl_cout << fn.c_str() << " contents:\n";

  {
    vcl_ifstream data(fn.c_str());
    char c;
    while (data.get(c))
      vcl_cout.put(c);
    vcl_cout << "EOF *****" << vcl_endl;
  }

  vcl_string ds, ts, bs;
  double v;

  vcl_ifstream data(fn.c_str());
  data >> ds >> ts >> bs >> v;
  TEST("Saved value 1 correctly", v, 5.0);
  data >> ds >> ts >> bs >> v;
  TEST("Saved value 2 correctly", v, 10.0);

  vcl_cout << "**************************************\n"
           << " Testing mbl_test_summaries_are_equal\n"
           << "**************************************\n";

  vcl_string A(
    "Some random data\n"
    "A 4\n"
    "B 3\n"
    "C 2\n"
    "AB 3\n");
  vcl_string B(
    "Some random data\n"
    "A 4\n"
    "B 3\n"
    "C 2\n"
    "AB 3\n");

  TEST("Simple case +ve", mbl_test_summaries_are_equal(A, B), true);
  TEST("Simple case +ve", mbl_test_summaries_are_equal(A + "D 1", B + "D 2"), false);

  TEST("Simple case +ve", mbl_test_summaries_are_equal(A, B), true);
  const char * ignore1[]={"D", 0 };
  TEST("Exclusions case +ve", mbl_test_summaries_are_equal(A + "D 1", B + "D 2", ignore1), true);
  const char * ignore2[]={"^ *D", 0 };
  TEST("RE Exclusions case +ve", mbl_test_summaries_are_equal(A + "D 1", B + "D 2", ignore2), true);
  TEST("RE Exclusions case -ne", mbl_test_summaries_are_equal(A + "AD 1", B + "AD 2", ignore2), false);


}

TESTMAIN(test_test);
