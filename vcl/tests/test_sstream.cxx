#include <vcl_string.h> // C++ specific includes first
#include <vcl_iostream.h>
#include <vcl_sstream.h>

#define Assert(x) {vcl_cout << "TEST " #x " : "; vcl_cout << ((x)?"PASSED":"FAILED")}

#define AssertEq(x) {vcl_cout<<"TEST ["<<fred<<"] == ["<<x<<"] : ";vcl_cout<<(fred==(x)?"PASSED":"FAILED")<<vcl_endl;}

int test_sstream_main(int /*argc*/,char* /*argv*/[])
{
  vcl_string x = "fred";
  vcl_stringstream ss(x);

  vcl_string fred;
  ss >> fred;
  AssertEq("fred");

  return 0;
}
