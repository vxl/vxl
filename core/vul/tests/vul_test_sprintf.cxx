#include <vul/vul_sprintf.h>
#include <vcl_iostream.h>

void Assert(char const* msg, bool expr)
{
  vcl_cout << msg << " - " << (expr?"passed":"failed") << "." << vcl_endl;
}

extern "C"
void test_vul_sprintf(void)
{
  vul_sprintf buf0("[%d]", 123);
  vcl_cout << "buf0=" << buf0 << vcl_endl;
  Assert("buf0 == 123", buf0 == vcl_string("[123]"));
  
  vcl_string string_fmt = "[%s]";

  vul_sprintf buf1(string_fmt.c_str(), "awf");
  vcl_cout << "buf1=" << buf1 << vcl_endl;
  Assert("buf1 == [awf]", buf1 == "[awf]");
}

int main()
{
  vcl_cout << "Running" << vcl_endl;
  test_vul_sprintf();
  return 0;
}
