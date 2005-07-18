// This is mul/mbl/tests/test_gamma.cxx
#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h>
#include <mbl/mbl_test.h>
#include <testlib/testlib_test.h>

void test_test()
{
  vcl_cout << "*******************\n"
           << " Testing mbl_test\n"
           << "*******************\n";
  
  vpl_unlink((vul_file::get_cwd()+"/mul/mbl/mbl_test_save_measurement.txt").c_str());
  vpl_rmdir((vul_file::get_cwd()+"/mul/mbl").c_str());
  vpl_rmdir((vul_file::get_cwd()+"/mul").c_str());
  vpl_rmdir((vul_file::get_cwd()+"/mul").c_str());

  vpl_putenv((vcl_string("MBL_TEST_SAVE_MEASUREMENT_ROOT=")+vul_file::get_cwd()).c_str());

  mbl_test_save_measurement("mul/mbl/mbl_test_save_measurement", 5.0);
  mbl_test_save_measurement("mul/mbl/mbl_test_save_measurement", 10.0);

  vcl_string fn = vul_file::get_cwd()+ "/mul/mbl/mbl_test_save_measurement.txt";
  perror("");
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

}

TESTMAIN(test_test);
