#include <string> // C++ specific includes first
#include <vcl_compiler.h>
#include <iostream>
#include <iostream>

#define Assert(x) {std::cout << "TEST " #x " : "; std::cout << ((x)?"PASSED":"FAILED")}

#define AssertEq(x) {std::cout<<"TEST ["<<fred<<"] == ["<<(x)<<"] : ";std::cout<<(fred==(x)?"PASSED":"FAILED")<<std::endl;}

int test_string_main(int /*argc*/,char* /*argv*/[])
{
  std::string fred;
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
