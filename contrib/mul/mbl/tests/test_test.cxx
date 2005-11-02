// This is mul/mbl/tests/test_test.cxx
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdlib.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h>
#include <mbl/mbl_test.h>
#include <testlib/testlib_test.h>

void test_test()
{
  vcl_cout << "******************\n"
           << " Testing mbl_test\n"
           << "******************\n";

  vpl_unlink((vul_file::get_cwd()+"/mul/mbl/mbl_test_save_measurement.txt").c_str());
  vpl_rmdir((vul_file::get_cwd()+"/mul/mbl").c_str());
  vpl_rmdir((vul_file::get_cwd()+"/mul").c_str());
  vpl_rmdir((vul_file::get_cwd()+"/mul").c_str());

  vpl_putenv((vcl_string("MBL_TEST_SAVE_MEASUREMENT_ROOT=")+vul_file::get_cwd()).c_str());

  char * envar = vcl_getenv("MBL_TEST_SAVE_MEASUREMENT_ROOT");
  vcl_string envar2(envar?envar:"");
  TEST ("putenv works", envar2, vul_file::get_cwd());
  vcl_cout << "Environment variable should be \"" << vul_file::get_cwd() <<
    "\".\nIs \"" << envar << "\".\n\n" << vcl_endl;

  mbl_test_save_measurement("mul/mbl/mbl_test_save_measurement", 5.0);
  mbl_test_save_measurement("mul/mbl/mbl_test_save_measurement", 10.0);
  mbl_test_save_measurement("mul/mbl/mbl_test_save_measurement", 15.0);

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
  vcl_cout << "ds: " << ds << "ts: " << ts << "bs: " << bs << "v: " << v << vcl_endl;
  TEST("Saved value 1 correctly", v, 5.0);
  data >> ds >> ts >> bs >> v;
  vcl_cout << "ds: " << ds << "ts: " << ts << "bs: " << bs << "v: " << v << vcl_endl; 
  TEST("Saved value 2 correctly", v, 10.0);
}

TESTMAIN(test_test);
