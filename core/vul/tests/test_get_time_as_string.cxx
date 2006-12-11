// This is core/vul/tests/test_get_time_as_string.cxx
#include <vcl_iostream.h>
#include <vul/vul_get_timestamp.h>
#include <testlib/testlib_test.h>


void test_get_time_as_string()
{
  vcl_string str0 = vul_get_time_as_string(vul_asc);
  vcl_cout << "vul_get_time_as_string(" << vul_asc << "): \"" << str0 << "\"" << vcl_endl;

  vcl_string str1 = vul_get_time_as_string(vul_numeric_msf);
  vcl_cout << "vul_get_time_as_string(" << vul_numeric_msf << "): \"" << str1 << "\"" << vcl_endl;
}


int test_get_time_as_string(int, char*[])
{
  testlib_test_start("test_get_time_as_string");

  test_get_time_as_string();

  return testlib_test_summary();
}
