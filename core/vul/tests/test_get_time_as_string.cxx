// This is core/vul/tests/test_get_time_as_string.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_get_timestamp.h>
#include <testlib/testlib_test.h>


static void test_get_time_as_string()
{
  std::string str0 = vul_get_time_as_string(vul_asc);
  std::cout << "vul_get_time_as_string(" << (int)vul_asc << "): \"" << str0 << '"' << std::endl;

  std::string str1 = vul_get_time_as_string(vul_numeric_msf);
  std::cout << "vul_get_time_as_string(" << (int)vul_numeric_msf << "): \"" << str1 << '"' << std::endl;
}

TEST_MAIN(test_get_time_as_string);
