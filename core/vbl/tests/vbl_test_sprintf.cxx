#include <vcl/vcl_compiler.h>
#include <vbl/vbl_sprintf.h>
#include <vcl/vcl_iostream.h>

void Assert(char const* msg, bool expr)
{
  cout << msg << " - " << (expr?"passed":"failed") << "." << endl;
}

extern "C"
void test_vbl_sprintf(void)
{
  vbl_sprintf buf0("[%d]", 123);
  cerr << "buf0=" << buf0 << endl;
  Assert("buf0 == 123", buf0 == vcl_string("[123]"));
  
  vcl_string string_fmt = "[%s]";

  vbl_sprintf buf1(string_fmt.c_str(), "awf");
  cerr << "buf1=" << buf1 << endl;
  Assert("buf1 == [awf]", buf1 == "[awf]");
}

main()
{
  cout << "Running" << endl;
  test_vbl_sprintf();
  return 0;
}
