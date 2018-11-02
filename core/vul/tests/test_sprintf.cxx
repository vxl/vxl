#include <iostream>
#include <vul/vul_sprintf.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>

void test_sprintf(void)
{
  vul_sprintf buf0("[%d]", 123);
  std::cout << "buf0=" << buf0 << std::endl;
  TEST("buf0 == 123", buf0, std::string("[123]"));

  std::string string_fmt = "[%s]";

  vul_sprintf buf1(string_fmt.c_str(), "awf");
  std::cout << "buf1=" << buf1 << std::endl;
  TEST("buf1 == [awf]", buf1, "[awf]");
}

TEST_MAIN(test_sprintf);
