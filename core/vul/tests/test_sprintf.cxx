#include <vul/vul_sprintf.h>
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>

void test_sprintf(void)
{
  vul_sprintf buf0("[%d]", 123);
  vcl_cout << "buf0=" << buf0 << vcl_endl;
  TEST("buf0 == 123", buf0, vcl_string("[123]"));

  vcl_string string_fmt = "[%s]";

  vul_sprintf buf1(string_fmt.c_str(), "awf");
  vcl_cout << "buf1=" << buf1 << vcl_endl;
  TEST("buf1 == [awf]", buf1, "[awf]");
}

//TESTMAIN(test_sprintf);
int test_sprintf(int, char*[])
{
  testlib_test_start("test_sprintf");

  test_sprintf();

  return testlib_test_summary();
}
