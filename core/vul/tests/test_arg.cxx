//-*- c++ -*-------------------------------------------------------------------
//
// test_vul_arg
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 21 Jan 97
//
//-----------------------------------------------------------------------------

#include <vul/vul_arg.h>
#include <vcl_cstring.h> // needed for strcmp()
#include <vcl_list.h>
#include <vcl_algorithm.h>
#include <testlib/testlib_test.h>

char const * my_argv_1[] = {
  "progname",
  "f",
  "-int", "3",
  "g",
  "-bool1",
  "-bool1",
  "-bool2",
  "-list",
  "1:2,10,21:3:25,-1:-2:-7",
  "h",
  "i",
  "j",
  0
};

int list_contents[] = {
  1,2,10,21,24,-1,-3,-5,-7
};

int count_my_args(char const * const * my_argv)
{
  int c = 0;
  for (; *my_argv; ++my_argv)
    ++c;
  return c;
}

void test_do_vul_arg()
{
  vul_arg_info_list arglist;

  vul_arg<int> int1(arglist, "-int", "A help string", 1);
  vul_arg<int> int2(arglist, "-int2", "Another help string", 2);
  vul_arg<bool> bool1(arglist, "-bool1", "And another", false);
  vul_arg<bool> bool2(arglist, "-bool2", "And another", true);
  vul_arg<bool> bool3(arglist, "-bool3", "And a final help test just to finish off...", true);
  vul_arg<vcl_list<int> > list1(arglist, "-list", "List...");
  vul_arg<char*> filename1(arglist);

  int my_argc = count_my_args(my_argv_1);
  vcl_cout << "vul_argc = " << my_argc
           << ", int1 = " << int1()
           << ", int2 = " << int2()
           << ", bool1 = " << bool1()
           << ", bool2 = " << bool2()
           << ", bool3 = " << bool3()
           << ", list1 size = " << list1().size()
           << vcl_endl;
  char **my_argv = (char**) my_argv_1;

  arglist.parse(my_argc, my_argv, true);

  TEST("int1", int1(), 3);
  TEST("int2", int2(), 2);
  TEST("filename == f", vcl_strcmp(filename1(), "f"), 0);

  unsigned true_list_length = sizeof list_contents / sizeof list_contents[0];
  vcl_list<int> l = list1();
  TEST("list length", l.size(), true_list_length);
  bool ok = true;
  for (unsigned int i = 0; i < true_list_length; ++i) {
    if (vcl_find(l.begin(), l.end(), list_contents[i]) == l.end()) {
      vcl_cout << "Integer [" << list_contents[i] << "] not found in list\n";
      ok = false;
    }
  }
  TEST("list contents", ok, true);
}

void test_arg()
{
  test_do_vul_arg();
}

TESTMAIN(test_arg);
