//-*- c++ -*-------------------------------------------------------------------
//
// Class: test_vbl_arg
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 21 Jan 97
//
//-----------------------------------------------------------------------------

#include <vbl/vbl_arg.h>
#include <vcl/vcl_cstring.h> // needed for strcmp()
#include <vcl/vcl_list.h>
#include <vcl/vcl_algorithm.h>

void Assert(char const* msg, bool expr)
{
  vcl_cout << msg << " - " << (expr?"passed":"failed") << "." << vcl_endl;
}

char const * my_argv_1[] = {
  "progname",
  "f",
  "-int", "3",
  "g",
  "-bool1",
  "-bool1",
  "-bool2",
  "-list",
  "1:2,10,21:2:25,-1:-2:-7",
  "h",
  "i",
  "j",
  0
};

int list_contents[] = {
  1,2,10,21,23,25,-1,-3,-5,-7,
};

int count_my_args(char const * const * my_argv)
{
  int c = 0;
  for(; *my_argv; ++my_argv)
    ++c;
  return c;
}

void test_do_vbl_arg()
{
  vbl_arg<int> int1("-int", "A help string", 1);
  vbl_arg<int> int2("-int2", "Another help string", 2);
  vbl_arg<bool> bool1("-bool1", "And another", false);
  vbl_arg<bool> bool2("-bool2", "And another", true);
  vbl_arg<bool> bool3("-bool3", "And a final help test just to finish off...", true);
  vbl_arg<vcl_list<int> > list1("-list", "List...");
  vbl_arg<char*> filename1;

  int my_argc = count_my_args(my_argv_1);
  vcl_cerr << "vbl_argc = " << my_argc
       << ", bool1 = " << bool1()
       << ", bool2 = " << bool2()
       << ", bool3 = " << bool3() << vcl_endl;
  char **my_argv = (char**) my_argv_1;

  vbl_arg_parse(my_argc, my_argv);
  
  bool b = int1() == 3;
  Assert("int1", b);
  Assert("int2", int2() == 2);
  Assert("filename == f", !strcmp(filename1(), "f"));

  unsigned true_list_length = sizeof list_contents / sizeof list_contents[0];
  vcl_list<int> l = list1();
  Assert("list length", l.size() == true_list_length);
  bool ok = true;
  for(int i = 0; i < true_list_length; ++i) {
    if (vcl_find(l.begin(), l.end(), list_contents[i]) == l.end()) {
      vcl_cerr << "Integer [" << list_contents[i] << "] not found in list\n";
      ok = false;
    }
  }
  Assert("list contents", ok);
}

extern "C"
void test_vbl_arg()
{
  test_do_vbl_arg();
}

int main()
{
  vcl_cout << "Running" << vcl_endl;
  test_vbl_arg();
  return 0;
}
