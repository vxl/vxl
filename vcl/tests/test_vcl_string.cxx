
#include <vcl/vcl_string.h> // C++ specific includes first
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>

#define Assert(x) {cout << "TEST " #x " : "; bool b = (x); cout << (b?"PASSED":"FAILED")}

#define AssertEq(x) {cout << "TEST [" << fred << "] == [" << x << "] : "; bool b = fred == (x); cout << (b?"PASSED":"FAILED") << endl; }

int main()
{
  vcl_string fred;
  fred = "fred";

  AssertEq("fred");

  fred += ", una";
  AssertEq("fred, una");

  fred.replace(3,1, "on");
  AssertEq("freon, una");

  fred.erase(5, 2);
  AssertEq("freonuna");

  fred.erase(5);
  AssertEq("freon");

  return 0;
}
